/*$
	CSplashWindow.h
	Visualizza uno splash screen a dissolvenza con l'immagine specificata.
	Luca Piergentili, Marzo '26

	NOTA TECNICA:

	La classe CSplashWindows, per visualizzare uno spash screen nell'applicazione principale, originariamente
	venne scritta in modalita' Win32.
	Pero', in seguito all'integrazione di MFC nell'applicazione (introduzione di CWinApp, etc.), si manifesto'
	un problema di inversione con i colori dell'immagine, con lo splash screen mostrando i canali del rosso e
	del blu invertiti (gialli diventando azzurri, rossi diventando blu e viceversa), a prescindere dalla libreria
	grafica utilizzata (FreeImage, paintLib, NexgenIPL o ImageMagick).

	La causa piu' probabile credo fosse l'integrazione con MFC nell'applicazione principale, di tipo console,
	ed il fatto che la classe, cosi' come l'applicazione principale, all'inizio solo prevedeva la modalita' Win32
	no MFC). In altre parole, il problema sembrava risiedere nel conflitto di interpretazione dei dati tra la GDI
	(Win32) e MFC, in un contesto alterato da CWinApp.

	La soluzione piu' indolore possibile fu quindi quella di passare a MFC nativo, usando la classe ATL::CImage
	(GDI+) integrata nel sistema operativo, che possiede un decoder PNG interno e che si interfaccia nativamente
	con il PaintDC di MFC.
	Venne inoltre effettuato uno swap preventivo dei pixel, subito dopo il caricamento del file da disco, invertendo
	manualmente i byte del rosso e del blu, il che blinda l'ordine corretto (BGRA) richiesto da Windows x86 prima
	che l'immagine venga passata al metodo Draw().

	Per evitare conflitti di compilazione dovuti all'ambiguita' del nome "CImage" (presente sia come classe custom
	del progetto principale che come classe di sistema in <atlimage.h>), l'header di Microsoft e' stato incluso
	esclusivamente all'interno del file .cpp. 
	La classe CSplashWindow espone verso l'esterno solo un puntatore opaco (struct CSplashWindowImpl* m_pImpl). In
	tal modo, l'inclusione di CSplashWindow.h nel resto del programma non inquina il namespace globale, lasciando
	intatto l'ambito della CImage originale del progetto.
*/
#ifndef _CSPLASHWINDOW_H
#define _CSPLASHWINDOW_H 1

#if defined(_AFX) || defined(_AFXDLL)

// usa la forward declaration per una struttura i cui dettagli sono nascosti nel .cpp, per non includere qui l'header <atlimage.h>
// includendo qui <atlimage.h>, la successiva inclusione di questo file <CSplashWindow.h> nel chiamante potrebbe causare ambiguita'
// con i simboli definiti da ATL
struct CSplashWindowImpl; 

/*
	CSplashWindow()
	(versione MFC/ATL)
*/
class CSplashWindow : public CWnd
{
	DECLARE_MESSAGE_MAP()

public:
	CSplashWindow();
	virtual ~CSplashWindow();

	// configurazione
	BOOL	LoadFromFile	(LPCSTR lpszPath);
	void	SetTimings		(DWORD dwVisibleMs,DWORD dwFadeMs);

	// creazione e visualizzazione
	BOOL	CreateAndShow	(void);

protected:
	afx_msg void OnTimer		(UINT_PTR nIDEvent);
	afx_msg void OnPaint		(void);
	afx_msg void OnLButtonDown	(UINT nFlags,CPoint point) {DestroyWindow();}
	afx_msg void OnKeyDown		(UINT nChar,UINT nRepCnt,UINT nFlags) {DestroyWindow();}
	afx_msg BOOL OnEraseBkgnd	(CDC* pDC) {return(TRUE);} // evita lo sfarfallio

private:
	CSplashWindowImpl* m_pImpl; // il puntatore che contiene i dati nascosti
	DWORD	m_dwVisibleMs;
	DWORD	m_dwFadeMs;
	DWORD	m_dwStartTime;
	BYTE	m_nCurrentAlpha;
};

#else

#include "window.h"
#include "CImage.h"

/*
	CSplashWindow()
	(versione Win32/CImage)
*/
class CSplashWindow : public CWnd
{
public:
	CSplashWindow(CImage* pImage);
	virtual ~CSplashWindow();

	// configurazione
	BOOL	LoadFromFile		(LPCSTR lpszPath);
	BOOL	LoadFromResource	(UINT nResID); // solo BMP (RT_BITMAP)
	void	SetTimings			(DWORD dwVisibleMs,DWORD dwFadeMs);

	// creazione e visualizzazione
	BOOL	CreateAndShow		(void);

protected:
	CImage*	m_pImage;
	DWORD	m_dwVisibleMs;
	DWORD	m_dwFadeMs;
	BYTE	m_nCurrentAlpha;
	DWORD	m_dwStartTime;
	BOOL	m_bIsFading;

	// Timer IDs
	enum {IDT_CHECK_FADE = 1};

	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer		(UINT_PTR nIDEvent);
	afx_msg void OnPaint		(void);
	afx_msg void OnLButtonDown	(UINT nFlags,CPoint point) {DestroyWindow();}
	afx_msg void OnKeyDown		(UINT nChar,UINT nRepCnt,UINT nFlags) {DestroyWindow();}
	afx_msg BOOL OnEraseBkgnd	(CDC* pDC) {return(TRUE);} // evita lo sfarfallio
};

#endif

#endif // _CSPLASHWINDOW_H
