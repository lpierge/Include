/*$
	CHttpDownloader.h
	Classe per il download via HTTP.
	Evoluzione del vecchio codice usato da CrawlPaper che gestiva solamente HTTP/1.0.
	Supporta il protocollo HTTP/1.1 tramite l'integrazione con l'API WinINet di Windows.
	Luca Piergentili, Giugno '25
*/
#ifndef _CHTTPDOWNLOADER_H
#define _CHTTPDOWNLOADER_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strings.h"
#include "window.h"
#include "CWinsock.h"
#include "CHostCache.h"
#include <wininet.h>
#include <shlwapi.h>
#include "url.h"
#include <vector>

#define MAX_REDIRECTS				5			// numero massimo di reindirizzamenti da seguire
#define CONNECT_RECEIVE_TIMEOUT_MS	30000		// timeout per connessione e ricezione, 30 secondi (in millisecondi)
#define MAX_PATH_LEN				_MAX_PATH	// lunghezza massima per i percorsi dei file in Windows
#define READ_BUFFER_SIZE			4096		// dimensione del buffer per la lettura dei dati dal flusso di rete, 4KB

/*
	NOTA:
	Negli esempi per gli headers, la stringa * / * (tutto attaccato) e' stata cambiata in *.* per non rompere i commenti, 
	la forma corretta e' '*' + '/' + '*', tutto attaccato, la forma '*.*' e' preistorica e si considera come un errore.

	Non esiste una lunghezza massima per i valori dei campi dell'header. La limitazione principale non riguarda il singolo 
	header, ma la dimensione totale dell'intero blocco di header http, solitamente imposta dal server web o dai proxy (come 
	Cloudflare) per prevenire attacchi DoS e comportamenti anomali.
	I limiti più comuni sono: 4, 8 ed in alcuni casi fino a 16 KB, se la dimensione totale di tutti gli header nella richiesta 
	supera questo limite, il server rifiutera' la richiesta, spesso con un errore 400 Bad Request.
*/
#define	HTTPHEADER_NAME_LEN		128
#define	HTTPHEADER_VALUE_LEN	1024
#define ETAG_HASH_LEN			128

/*
	HTTPHEADER
*/
struct HTTPHEADER {
	char name[HTTPHEADER_NAME_LEN+1];
	char value[HTTPHEADER_VALUE_LEN+1];
};

/*
	CHttpHeaderList

	classe per la lista dei campi per gli headers HTTP del client
	la lista dei campi dell'header http del server e' contenuta
	nella struttura HTTPREPLY
*/
class CHttpHeaderList : public CNodeList
{
public:
	CHttpHeaderList() : CNodeList() {}
	virtual ~CHttpHeaderList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new HTTPHEADER);
	}
	void* Initialize(void* pVoid)
	{
		HTTPHEADER* pData = (HTTPHEADER*)pVoid;
		if(!pData)
			pData = (HTTPHEADER*)Create();
		if(pData)
			memset(pData,'\0',sizeof(HTTPHEADER));
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((HTTPHEADER*)iter->data)
			delete ((HTTPHEADER*)iter->data),iter->data = (HTTPHEADER*)NULL;
		return(TRUE);
	}
	int Size(void) {return(sizeof(HTTPHEADER));}
#ifdef _DEBUG
	const char* Signature(void) {return("CHttpHeaderList");}
#endif
};

/*
	HTTPREPLY
*/
typedef struct _httpreply {
	DWORD		dwStatusCode;				// codice WinInet (GetLastError())
	DWORD		dwHttpCode;					// codice risposta http
	CHttpHeaderList m_serverHttpHeaderList;	// lista headers di risposta del server
} HTTPREPLY;

/*
	tipo connessione HTTP (close/keep-alive)
*/
#define HTTP_CONNECTION_CLOSE		0
#define HTTP_CONNECTION_KEEPALIVE	1

#define MAX_COOKIE_NAME_LEN			128
#define MAX_COOKIE_ATTR_LEN			4096
#define MAX_COOKIE_DOMAIN_LEN		256
#define MAX_COOKIE_PATH_LEN			512
#define MAX_COOKIE_LEN				(MAX_COOKIE_NAME_LEN + MAX_COOKIE_ATTR_LEN + MAX_COOKIE_DOMAIN_LEN + MAX_COOKIE_PATH_LEN)

/*
	COOKIEINFO
	
	struttura basica per i cookie, esistono molti altri attributi che qui non vengono inclusi
*/
typedef struct _cookieinfo {
    char	name[MAX_COOKIE_NAME_LEN+1];
    char	value[MAX_COOKIE_ATTR_LEN+1];
    char	domain[MAX_COOKIE_DOMAIN_LEN+1];// dominio per cui il cookie è valido (es. .example.com)
    char	path[MAX_COOKIE_PATH_LEN+1];	// percorso per cui il cookie è valido (es. /blog/)
    time_t	created;						// timestamp UNIX (secondi dal 1970)
    time_t	lastused;						// timestamp UNIX (secondi dal 1970)
    time_t	expires;						// timestamp UNIX (secondi dal 1970)
    BOOL	secure;							// solo per connessioni HTTPS
    BOOL	httpOnly;						// non accessibile via script client
} COOKIEINFO;

// tipo per il puntatore alla funzione per il log
typedef void (*LogMessageCallback)(DWORD dwFlags,LPCSTR pszHostDomain,LPCSTR pszBaseDir,LPCSTR pszFormat,...);

// per log e membro, sovrascrive o meno il file
#define HTTP_FLAG_DONOTOVWR	0x0000			// no
#define HTTP_FLAG_OVERWRITE	0x0001			// si
#define HTTP_FLAG_OVWRIFBIG	0x0002			// solo se >
#define HTTP_FLAG_SKIPEXIST 0x0004			// salta se gia' esiste solo in base a nome (ignorando data/ora, dimensione, etc.)
//							0x0008
//							0x0010

/*
	DBCALLBACK

	struttura per l'intercambio dati, attraverso la callback, con il chiamante,
	per i dati che necessitano essere memorizzati per uso futuro
	vedi le note nella funzione callback del chiamante
	nota: qui definisce i campi a full-size, ma nel database sono piu' piccoli
*/
#define FILESIZE_MAX_LEN 10
typedef struct _dbcallback {
	int	action;
	int returncode;
	char url[MAX_URL_LENGTH+1];
	char file[FILESIZE_MAX_LEN+_MAX_FILEPATH+1];	// file size + file name
	char date[MAX_DATE_STRING+1];
	char etag[ETAG_HASH_LEN+1];
	char type[HTTPHEADER_VALUE_LEN+1];
} DBCALLBACK;

// tipo per il puntatore alla funzione callback del chiamante per gestire
// i dati memorizzati nel database
typedef BOOL (*DatabaseCallback)(DBCALLBACK* pDbCallback);

// azioni (ricava o registra da/nel database)
#define DBCALLBACK_GET	1
#define DBCALLBACK_PUT	2
#define DBCALLBACK_STAT	3

// tipo per il puntatore alla funzione callback del chiamante per la verifica/cambio
// del nome del file di output (il nome in locale per la risorsa da scaricare)
// vengono passati:
// lpcszOriginalName	nome del file che verra' utilizzato in locale per la risorsa
// lpszModifiedName		buffer dove mettere l'eventuale nuovo nome
// nModifiedName		dimensione del buffer
// pContext				ptr generico per agganciare dati del chiamante se volesse fornire un contesto da passare alla callback
typedef BOOL (*NameRulerCallback)(LPCSTR lpcszOriginalName,LPSTR lpszModifiedName,UINT nModifiedName,PVOID pContext);

/*
	CHttpDownloader
*/
class CHttpDownloader
{
public:
	// passare la funzione per la callback al costruttore o assicurarsi che la SetLogCallback() 
	// sia il primo membro della classe ad essere chiamato, subito dopo la dichiarazione dell'oggetto
	// i flags sono per la funzione per il log
	CHttpDownloader(LogMessageCallback pfnCallback = NULL,DWORD dwflags = 0);
	virtual ~CHttpDownloader();

	void		SetLogCallback					(DWORD dwFlags,LogMessageCallback pfnCallback)	{m_dwCallbackFlags = dwFlags; m_pfnLogCallback = pfnCallback;}
	void		SetDatabaseCallback				(DatabaseCallback pfnDatabaseCallback)			{m_pfnDatabaseCallback = pfnDatabaseCallback;}
	void		SetNameRulerCallback			(NameRulerCallback pfnNameRulerCallback,PVOID pContext) {m_pfnNameRulerCallback = pfnNameRulerCallback; m_pNameRulerContext = pContext;}

	BOOL		SetBaseDirectory				(LPCSTR lpcszDirectory)							{strcpyn(m_szBaseDirectory,lpcszDirectory,sizeof(m_szBaseDirectory)); return(CreateBaseDirectoryTree());}
	LPCSTR		GetBaseDirectory				(void)											{return(m_szBaseDirectory);}

	void		SetOverwriteExisting			(int nOverwrite)								{m_nOverwrite = nOverwrite;}
	void		SetReplaceFileExt				(BOOL bReplaceFileExt)							{m_bReplaceFileExt = bReplaceFileExt;}

	BOOL		SetHttpVersion					(int nMajor,int nMinor);
	void		GetHttpVersion					(int& nMajor,int& nMinor)						{nMajor = m_httpVersion.dwMajorVersion; nMinor = m_httpVersion.dwMinorVersion;}
	BOOL		IsHTTPVersion					(int nMajor,int nMinor)							{return(m_httpVersion.dwMajorVersion==nMajor && m_httpVersion.dwMinorVersion==nMinor);};

	int			SetConnectionType				(int nType);

	void		SetUserAgent					(LPCSTR lpcszUserAgent,BOOL bBuildFullString);
	LPCSTR		GetUserAgent					(void)											{return(m_szUserAgent);}

	void		UseCookies						(BOOL bUseIt)									{m_bUseCookies = bUseIt;}

    int			SetCurrentHost					(LPCSTR lpcszUrl);
    LPCSTR		GetCurrentHost					(void)											{return(m_szCurrentHost);}
	int			CheckHostByName					(LPCSTR hostName,LPSTR lpszIP,size_t nIP);

    LPCSTR		GetCurrentBaseDomain			(void) const									{return(m_szCurrentBaseDomain);}

	LPCSTR		GetServerHttpHeader				(LPCSTR lpcszHeader);
	BOOL		AddClientHttpHeader				(LPCSTR lpcszName,LPCSTR lpcszValue);
	void		RemoveClientHttpHeaders			(LPCSTR lpcszName = NULL);

	BOOL		DownloadUrl						(LPSTR pszUrl,UINT nUrlSize,LPSTR lpszOutputfile,UINT nSize,LPSTR lpszNonCollidingFileName = NULL,UINT nNonCollidingSize = (UINT)-1);
	DWORD		GetHttpCode						(void)											{return(m_httpReply.dwHttpCode);}
	DWORD		GetHttpStatus					(void)											{return(m_httpReply.dwStatusCode);}

private:
	DWORD		OpenSession						(void);
	void		CloseSession					(void);

	BOOL		EnableHttpVersioning			(void);

	BOOL		GetFileByUrl					(LPCSTR lpcszUrl,LPSTR szFilename,UINT nFilename,LPSTR szLastModified,UINT nLastModified,LPSTR szETag,UINT nETag,LPSTR szContentType,UINT nContentType,DWORD& dwFilesize);
	BOOL		PutFileByUrl					(LPCSTR lpcszUrl,LPCSTR lpcszFilename);
	void		ContentTypeStats				(void);

	void		AddHttpHeadersToRequest			(LPSTR lpszUrl);
	int			LoadServerHttpHeaders			(void);
	void		CleanServerHttpHeaders			(void);
	void		ParseServerHeaderLine			(LPCSTR line);

	BOOL		SkipDownloadByOverwriteFlag		(LPSTR szResource,UINT nSize,UINT& nHttpSuggestedCode);
	int			CheckIfUrlOrfile				(LPSTR lpszUrl,UINT nUrlSize,LPSTR lpszOutputfile,UINT nSize);
    BOOL		ComposeFilenameFromUrl			(LPCSTR pszUrl,char* pszOutputFilenameBuffer,size_t bufferSize,LPCSTR pszContentType);
	BOOL		HttpToFileDateTime				(LPCSTR szHttpDate,FILETIME* pFileTime);
	void		UpdateFileDateTime				(LPCSTR szFilePath);

	BOOL		CreateBaseDirectoryTree			(void);

    void		HandleReceivedCookies			(LPCSTR pszCurrentUrl);
	BOOL		ParseSetCookieHeader			(LPCSTR pszSetCookieString,COOKIEINFO* pCookie,BOOL& bIsCookieExpired);
	BOOL		DeleteCookieKeys				(LPCSTR pszFullCookieFilePath,const char* sectionName,const COOKIEINFO* pCookie);
	BOOL		SaveCookieToFile				(LPCSTR pszOutputDirectory,const COOKIEINFO* pCookie,BOOL IsCookieExpired);
	std::vector<COOKIEINFO>	LoadCookiesForUrl	(LPCSTR pszOutputDirectory,LPCSTR pszUrl,LPCSTR pszHostDomainForLog);

    void		LogHttpHeaders					(HINTERNET hHandle,DWORD dwInfoLevel,int nType);

    HINTERNET			m_hInternetSession;
	HINTERNET			m_hConnect;
    HINTERNET			m_hRequest;
	int					m_nConnectionType;
	HTTP_VERSION_INFO	m_httpVersion;
	HTTPREPLY			m_httpReply;
	CHttpHeaderList		m_clientHttpHeaderList;
	BOOL				m_bHaveValidSession;
	int					m_nOverwrite;
	BOOL				m_bReplaceFileExt;
	char				m_szUserAgent[512];
	BOOL				m_bUseCookies;
	DWORD				m_dwCallbackFlags;
	LogMessageCallback	m_pfnLogCallback;
	DatabaseCallback	m_pfnDatabaseCallback;
	NameRulerCallback	m_pfnNameRulerCallback;
	PVOID				m_pNameRulerContext;
	char				m_szBaseDirectory[MAX_PATH_LEN+1];
    char				m_szCurrentHost[MAX_PATH_LEN+1];
    char				m_szCloseKeepAliveHost[MAX_PATH_LEN+1];
    char				m_szCurrentBaseDomain[MAX_PATH_LEN+1];
	char				m_szHostForReferer[MAX_URL_HOST+1];
	CWinsock			m_Socket;
	CHostCache			m_hostCache;
};

#endif // _CHTTPDOWNLOADER_H
