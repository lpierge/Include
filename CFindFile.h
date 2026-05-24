/*$
	CFindFile.h
	Classe base per la ricerca files (SDK/MFC).
	Luca Piergentili, 14/02/00
	lpiergentili@yahoo.com
*/
#ifndef _CFINDFILE_H
#define _CFINDFILE_H 1

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include "window.h"
#include "win32api.h"
#include "CNodeList.h"
#include "CDateTime.h"

#define _A_ALLFILES					(_A_NORMAL|_A_RDONLY|_A_HIDDEN|_A_SYSTEM)
#define _A_ALLFILESANDDIRECTORIES	(_A_NORMAL|_A_RDONLY|_A_HIDDEN|_A_SYSTEM|_A_SUBDIR|_A_ARCH)

// handleapi.h -> #define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)
#define VALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)1)

/*
	PFNFINDFILECALLBACK
	tipo per la funzione callback, deve essere di tipo libero (statico)
	la classe che usa l'oggetto CFindFile deve definire due callabacks, una statica, da passare
	qui insiema al suo puntatore this, ed una reale che sara' quella che verra invocata tramite 
	il puntatore a this
	la callback riceve 3 parametri:
	1) WPARAM = puntatore al this del chiamante
	2) LPARAM = puntatore alla struttura FINDFILE corrente (il file trovato)
	3) LPARAM = il di tipo chiamata: 0=inizio, 1=corrente, 2=fine
*/
typedef LONG (CALLBACK* PFNFINDFILECALLBACK)(WPARAM,LPARAM,LPARAM,LPARAM);

/*
	FINDFILE
	struttura per l'elemento della lista dei files
*/
struct FINDFILE {
	UINT		attrib;					// attributo
	SYSTEMTIME	datetime;				// data/ora
	DWORD		size;					// dimensione (in bytes)
	char		name[_MAX_FILEPATH+1];	// nome file (pathname incluso)
	HANDLE		handle;					// handle per findfirst/next
};

/*
	CFindFileList
	classe per la lista dei files
*/
class CFindFileList : public CNodeList
{
public:
	CFindFileList() : CNodeList() {}
	virtual ~CFindFileList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new FINDFILE);
	}
	void* Initialize(void* pVoid)
	{
		FINDFILE* pData = (FINDFILE*)pVoid;
		if(!pData)
			pData = (FINDFILE*)Create();
		if(pData)
			memset(pData,'\0',sizeof(FINDFILE));
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((FINDFILE*)iter->data)
			delete ((FINDFILE*)iter->data),iter->data = (FINDFILE*)NULL;
		return(TRUE);
	}
	int Size(void) {return(sizeof(FINDFILE));}
#ifdef _DEBUG
	const char* Signature(void) {return("CFindFileList");}
#endif
};

/*
	CFindFile
*/
class CFindFile
{
public:
	CFindFile();
	virtual ~CFindFile() {};

	// controlla l'esistenza
	inline BOOL	Exist			(LPCSTR lpcszFileName) {return(FileExists(lpcszFileName));}	// nome file
	BOOL		ExistEx			(LPCSTR lpcszSkel);											// nome file/skeleton

	// ricerca semplice (while)
	LPCSTR		Find			(LPCSTR lpcszSkel = "\\*.*");		// skeleton

	// ricerca ricorsiva (while/lista)
	LPCSTR		FindEx			(LPCSTR	lpcszStartDir = "\\",		// dir iniziale (terminare con '\')
								 LPCSTR	lpcszSkel = "*.*",			// skeleton
								 BOOL	bRecursive = TRUE,			// ricorsivo
								 UINT	uAttribute = _A_ALLFILES,	// attributi
								 UINT*	puiAttribute = NULL,		// attributo dell'elemento trovato (file o directory) da confrontare (|) con FILE_ATTRIBUTE_DIRECTORY
								 DWORD*	pdwSize = NULL,				// dimensione dell'elemento trovato
								 VOID*	pVoidPtr = NULL				// puntatore ai dati (struct) definiti dal chiamante, per passare dati tra chiamante e callback
								);

	// ricerca ricorsiva (lista)
	UINT		FindFile		(LPCSTR	lpcszStartDir = "\\",		// dir iniziale (terminare con '\')
								 LPCSTR	lpcszSkel = "*.*",			// skeleton
								 BOOL	bRecursive = TRUE,			// ricorsivo
								 UINT	uAttribute = _A_ALLFILES,	// attributo
								 VOID*	pVoidPtr = NULL				// puntatore ai dati (struct) definiti dal chiamante, per passare dati tra chiamante e callback
								);
	LPCSTR		GetFileName		(int	nIndex);
	FINDFILE*	GetFindFile		(int	nIndex);


	// ricerca ricorsiva (lista, ex-interna: chiamata da quelle di cui sopra)
	HANDLE		Search			(LPCSTR	lpcszStartDir,
								 LPCSTR	lpcszSkel,
								 BOOL	bRecursive,
								 UINT	uAttribute,
								 VOID*	pVoidPtr
								);

	// restituisce il puntatore alla lista interna dei files trovati
	CFindFileList* GetList		(void) {return(&m_listFileNames);}

	// numero files trovati
	inline UINT	Count			(void) const {return(m_nCount);}

	// ripulisce
	// ATTENZIONE: oltre ad azzerare la lista dei files trovati, azzera il ptr alla callback ed al this, che dovranno 
	// essere quindi reimpostati con una nuova chiamata alla SetCallback()
	void		Reset			(BOOL bClose = TRUE);

	// data/ora
	inline void	SetFileTimeFormat(DATEFORMAT DateFormat) {m_enumDateFormat = DateFormat;}
	inline BOOL	SetFileTime		(LPCSTR lpcszFileName,WORD uDate,WORD uTime) {return(SetFileDateTime(lpcszFileName,uDate,uTime));}
	inline BOOL	GetFileTime		(LPCSTR lpcszFileName,LPWORD lpuDate,LPWORD lpuTime) {return(GetFileDateTime(lpcszFileName,lpuDate,lpuTime,NULL));}

	// pathnames
	BOOL		SplitPathName	(LPCSTR lpcszPathName,LPSTR lpszDirectory,UINT cbDirectory,LPSTR lpszFileName,UINT cbFileName,BOOL bUseCurrenDirectory = TRUE);
	static BOOL	CreatePathName	(LPSTR lpszPathName,UINT cbPathName = (UINT)-1);

	/*
		SetCallback()
		imposta la callback che viene chiamata durante la ricerca con Search()
		la callback riceve in WPARAM il puntatore passato all'inizio, ossia il this della classe chiamnte, nel primo 
		LPARAM il puntatore alla struttura FINDFILE con i dati del file trovato e nel secondo LPARAM il tipo di chiamata 
		(0=inizio ricerca, 1=elemento corrente trovato, 2=fine ricerca)
		se la callback ritorna 1 allora Search() aggiunge il file alla lista interna, che verra' poi restituito dalle 
		Find...(), mentre se ritorna 0 il file viene scartato
	*/
	void		SetCallback		(PFNFINDFILECALLBACK lpfnCallBack,void* pVoid = NULL) {m_lpfnCallBack = lpfnCallBack; m_thisPtr = pVoid;}

private:
	
	// occhio perche' NON usa il campo uAttribute
	BOOL		First			(LPCSTR	lpcszFileName,
								 UINT	uAttribute,
								 FINDFILE* f = NULL
								);
		
	BOOL		Next			(FINDFILE* f = NULL);

	BOOL		Close			(HANDLE hHandle = INVALID_HANDLE_VALUE);

	DATEFORMAT					m_enumDateFormat;
	int							m_nCount;
	int							m_nInternalCount;
	BOOL						m_bFindFirstCall;
	BOOL						m_bFindExFirstCall;
	FINDFILE					m_stFindFile;
	WIN32_FIND_DATA				m_stWin32FindData;
	CFindFileList				m_listFileNames;
	void*						m_thisPtr;			
	PFNFINDFILECALLBACK			m_lpfnCallBack;
};

#endif // _CFINDFILE_H
