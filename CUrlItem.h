/*$
	CUrlItem.h
	Classe per la gestione del file delle URLs (wchg), implementa il caricamento delle
	URLs e la gestione della lista relativa.
	Luca Piergentili, 16/12/25

	Il codice e' specifico per riempire la lista con il contenuto del file url.txt, dove
	ogni record ha la seguente struttura:

	"2","YOUR_API_KEY","0","https://api.pexels.com/v1/search?query=cosplay+japan&size=large&per_page=15&page=1",auto
*/
#ifndef _CURLITEM_H
#define _CURLITEM_H 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "window.h"
#include "CNodeList.h"

#define URLITEM_SERVICE_LEN		2
#define URLITEM_APIKEY_LEN		128
#define URLITEM_CACHE_LEN		10
#define URLITEM_URL_LEN			MAX_URL_LENGTH
#define URLITEM_URL_REPLY
#define URLITEM_URL_LOCALFILE	_MAX_FILEPATH

/*
	JSONDATA
*/
typedef struct _jsondata {
	uint64_t hash;								// hash per campi che possano necessitarlo, tipo titolo etc.
	char	id[32];								// identificativo generico
	char	author[128];						// autore dell'immagine
	char	theme[128];							// tema/argomento/carattere, etc.
	char	title[_MAX_PATH+1];					// titolo dell'immagine
	char	source[MAX_URL_LENGTH+1];			// provenienza immagine
} JSONDATA;

/*
	METAQUERY
*/
typedef struct _meta_query {
	char	local;								// flag per trattamento file locale: d = do not overwrite (proliferate), o = overwrite the existing file, s = skip existing files 
	char	connection;							// flag per tipo connessione: c = Close, k = Keep-alive
	char	cachebuster[16];					// nome da usare per il parametro cache buster, il valore relativo viene definito dal programma
} METAQUERY;

/*
	URLITEM

	elemento per la url e dati relativi
*/
typedef struct _urlitem {
	int		service;							// tipo servizio: 0 = generico, 1 = picsum, 2 = pexel, 3 = reddit, 4 = danbooru
	int		origin;								// per mantenere il tipo di servizio originale (le pexels/reddit/danbooru vengono elaborate e convertite a generiche)
	int		counter;							// numero di volte che e' stata usata
	BOOL	metaurl;							// se e' un meta-file (.json contenente immagini) o no (un immagine)
	char	apikey[URLITEM_APIKEY_LEN+1];		// API Key relativa al servizio, se prevista
	int		cache;								// quanti minuti mantenere in cache prima di richiedere, 0 = scaricare sempre, -1 = scaricare solo una volta
	char	url[URLITEM_URL_LEN+1];				// URL (immagine/.json) da scaricare
	char	referer[URLITEM_URL_LEN+1];			// valore header HTTP Referer, se necessario (danbooru)
	int		reply;								// risposta (HTTP) del server
	char	localfile[URLITEM_URL_LOCALFILE+1];	// file locale x immagine scaricata
	char	subdir[_MAX_PATH+1];				// sub-directory (rispetto a quella di download) dove scaricare i files
	TERN	shown;								// flag per stato visualizzazione immagine -> il False NON e' per non visualizzata ma per errore: True = visualizzata, Undef = non visualizzata
	METAQUERY metaquery;						// per i parametri specificati con la meta-query (!)
	JSONDATA jsondata;							// metadati provenienti dal .json
} URLITEM;

/*
	URLITEMRECORD

	struttura usata per mappare i dati letti dal file (in formato CSV) con le funzioni di csvlib sulla struttura URLITEM
*/
typedef struct _urlitemrecord {
	char	service[URLITEM_SERVICE_LEN+1];
	char	apikey[URLITEM_APIKEY_LEN+1];
	char	cache[URLITEM_CACHE_LEN+1];
	char	url[URLITEM_URL_LEN+1];
	char	subdir[_MAX_PATH+1];
} URLITEMRECORD;

/*
	CUrlItemList

	classe per la lista delle url da scaricare
*/
class CUrlItemList : public CNodeList
{
public:
	CUrlItemList() : CNodeList() {}
	virtual ~CUrlItemList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new URLITEM);
	}	
	void* Initialize(void* pVoid)
	{
		URLITEM* pData = (URLITEM*)pVoid;
		if(!pData)
			pData = (URLITEM*)Create();
		if(pData)
			memset(pData,'\0',sizeof(URLITEM));
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((URLITEM*)iter->data)
			delete ((URLITEM*)iter->data),iter->data = (URLITEM*)NULL;
		return(TRUE);
	}
	int Size(void) {return(sizeof(URLITEM));}
#ifdef _DEBUG
	const char* Signature(void) {return("CUrlItemList");}
#endif
};

/*
	CUrlListManager
	
	classe per la manipolazione dei dati caricati nella lista di cui sopra, basicamente il motore che nutre il chiamante con le url da scaricare
*/
class CUrlListManager
{
public:
	CUrlListManager();
	~CUrlListManager() {}

	void			Clean		(void);									// riazzera tutto
	BOOL			Load		(LPCSTR lpcszFilename);					// carica la lista di cui sopra con i record del file delle url
	URLITEM*		Next		(void);									// restituisce, in un ciclo rotatorio/infinito, i records della lista
	int				Sanitize	(int nHowManyTimes);					// ripulisce gli elementi gia' usati, per URLs tipo Pexels 1 URL=1 file
	int				Count		(void)	{return(m_urlItemList.Count());}// restituisce il totale dei record presenti nella lista
	CUrlItemList*	GetUrlsList	(void)	{return(&m_urlItemList);}		// restituisce il ptr alla lista (interna) per le url
	LPCSTR			GetUrlFile	(void)	{return(m_szUrlFile);}			// nome file url.txt
	LPCSTR			GetLastError(void)	{return(m_szError);}

private:
	char			m_szUrlFile[_MAX_FILEPATH+1];						// percorso completo del file di input (url.txt)
	int				m_nCurrent;											// indice corrente per l'elemento della lista delle url
	int				m_nSanitizer;										// ogni quanto ripulire
	char			m_szError[256];										// descrizione errore
	CUrlItemList	m_urlItemList;										// lista per le url
};

#endif // _CURLITEM_H
