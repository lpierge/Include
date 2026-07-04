/*$
	CFilenameFactory.h
	Classe base per la gestione dei nomi files (SDK/MFC).
	Luca Piergentili, 02/09/03
	lpiergentili@yahoo.com
*/
#ifndef _CFILENAMEFACTORY_H
#define _CFILENAMEFACTORY_H 1

#include <stdio.h>
#include <stdlib.h>
#include "window.h"
#include <wchar.h>

#define ABBREVIATE_DEFAULT_STRING	"[...]"
#define ABBREVIATE_DEFAULT_STRINGW	L"[...]"
#define ABBREVIATE_MAX_FNAME		64L

/*
	CFilenameFactory
*/
class CFilenameFactory
{
public:
	CFilenameFactory()			{memset(m_szFileName,'\0',sizeof(m_szFileName));memset(m_szFileNameW,'\0',sizeof(m_szFileNameW));}
	virtual ~CFilenameFactory()	{}

	// versione ANSI e Wide (Unicode)

	LPCSTR	Abbreviate			(LPCSTR	lpcszFilename,						// nome file (con eventuale pathname)
								int		nMaxLength		= ABBREVIATE_MAX_FNAME,	// massima lunghezza (oltre la quale tronca), se <= 0 elimina solo il pathname
								BOOL	bSaveExt		= FALSE,			// per includere o meno l'estensione del file
								BOOL	bStripPath		= TRUE,				// per eliminare o meno il pathname dal nome file di output
								LPCSTR	pDefaultString	= NULL,				// se non specificato diversamente assume <ABBREVIATE_DEFAULT_STRING>
								LPSTR	pFilename		= NULL,				// buffer di output
								UINT	nFilenameSize	= 0					// dim. buf. output
								);
	LPCWSTR	AbbreviateW			(LPCWSTR lpcwszFilename,
								int		nMaxLength		= ABBREVIATE_MAX_FNAME,
								BOOL	bSaveExt		= FALSE,
								BOOL	bStripPath		= TRUE,
								LPCWSTR	pDefaultString	= NULL,
								LPWSTR	pFilename		= NULL,
								UINT	nFilenameSize	= 0
								);

	LPCSTR	GetNext				(LPCSTR	lpcszFilename,						// (solo) nome file, senza pathname
								LPCSTR	lpcszPathname = NULL,				// pathname (senza o con \ finale)
								LPSTR	pFilename = NULL,					// buffer di output
								UINT	nFilenameSize = 0					// dim. buf. output
								);

	BOOL	GetNextWithinRange	(LPCSTR	lpcszFilename,						// nome file completo di pathname
								UINT	nMaxProg,							// numero massimo per progressivo per nome file
								LPSTR	pFilename,							// buffer di output
								UINT	nFilenameSize						// dim. buf. output
								);

private:
	void	BuildCandidateName	(LPSTR	pFilename,							// merge dei componenti del nome file
								UINT	nFilenameSize,
								LPCSTR	dirPart,
								LPCSTR	namePart,
								LPCSTR	extPart,
								UINT	nProg/*, 
								UINT	nPaddingWidth*/
								);

	char	m_szFileName[_MAX_FILEPATH+1];
	wchar_t	m_szFileNameW[_MAX_FILEPATH+1];
};

#endif // _CFILENAMEFACTORY_H
