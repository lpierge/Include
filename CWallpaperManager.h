/*$
	CWallpaperManager.h
	Classe per gestire una inizializzazione unica per l'oggetto COM usato per cambiare lo sfondo del desktop.
	Luca Piergentili, Feb '26
*/
#ifndef _CWALLPAPERMANAGER_H
#define _CWALLPAPERMANAGER_H 1

#include "window.h"
#include "win32api.h"

#include <shobjidl.h>
#include <shlguid.h>
#include <combaseapi.h>
#pragma comment(lib,"Ole32.lib")
#pragma comment(lib,"Shell32.lib")

/*
	CWallpaperManager
*/
class CWallpaperManager
{
public:

	CWallpaperManager() : m_pDesktopWallpaper(NULL),m_bComInitialized(FALSE)
	{
		// CoInitialize puo' restituire S_OK o S_FALSE (se gia' inizializzato)
		// entrambi vanno bene, ma deve sapere se poi bisogna chiamare CoUninitialize
		HRESULT hr = ::CoInitialize(NULL);
		if(SUCCEEDED(hr))
			m_bComInitialized = TRUE;

		hr = ::CoCreateInstance(CLSID_DesktopWallpaper,NULL,CLSCTX_ALL,IID_PPV_ARGS(&m_pDesktopWallpaper));
		if(FAILED(hr))
			m_pDesktopWallpaper = NULL;
	}

	~CWallpaperManager()
	{
		// rilascia i puntatori COM
		if(m_pDesktopWallpaper)
		{
			m_pDesktopWallpaper->Release();
			m_pDesktopWallpaper = NULL;
		}

		// chiude la libreria COM solo dopo aver rilasciato
		if(m_bComInitialized)
		{
			::CoUninitialize();
			m_bComInitialized = FALSE;
		}
	}

	// accesso
	IDesktopWallpaper* GetInterface(void)
	{
		return m_pDesktopWallpaper;
	}

	BOOL IsValid(void) const
	{
		return(m_pDesktopWallpaper!=NULL);
	}

private:

	IDesktopWallpaper* m_pDesktopWallpaper;
	BOOL m_bComInitialized;

	// impedisce la copia della classe
	CWallpaperManager(const CWallpaperManager&);
	CWallpaperManager& operator=(const CWallpaperManager&);
};

#endif // _CWALLPAPERMANAGER_H
