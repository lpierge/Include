/*
	CTrayIconW.h
	Classe per la gestione dell'icona nel system tray (MFC).
	Versione aggiornata dell'originale (CTrayIcon.cpp) per poter usare caratteri Wide.
	(Unicode esplicito)
	Luca Piergentili, 07/06/26

	Vedi le note nell originale (CTrayIcon.h).
*/
#ifndef _CTRAYICON_H
#define _CTRAYICON_H 1

#include "window.h"
#include <wchar.h>
#include "CWindowsVersion.h"
#include "CTrayIconPosition.h"
#include "CPPTooltipW.h"
#include <shellapi.h>

// vedi le definizioni delle macro sottostanti in "window.h"

#if defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0500)
  // Windows moderno (da Windows 2000/XP in poi) accetta fino ai seguenti limiti
  #define TRAYICON_MAX_TOOLTIP_TEXT	127											// supportato da Win2000 in poi
  #define BALLOON_MAX_TOOLTIP_TITLE	63											// rimane come precedente
  #define BALLOON_MAX_TOOLTIP_TEXT	1024										// per l'esteso (il nativo taglia a 255)
#else
  // Windows e' un po' cazzone e piu' di <n> caratteri non li accetta
  // (fallback storico per compatibilita' assoluta (pre-Windows 2000)
  #define TRAYICON_MAX_TOOLTIP_TEXT	63											// testo tooltip per icona (tray), 63 o 127 per comctl32.dll >= 5.0 (transizione Windows 95/98 e Windows 2000/XP)
  #define BALLOON_MAX_TOOLTIP_TITLE	63											// titolo balloon
  #define BALLOON_MAX_TOOLTIP_TEXT	1024										// testo balloon
#endif

#define _MAX_MENU_STRING_LENGTH		TRAYICON_MAX_TOOLTIP_TEXT					// max len per voce menu

#define BALLOON_DEFAULT_TIMEOUT		10											// timeout (in sec.) di default per la rimozione
#define BALLOON_USE_NATIVE			0											// usa il balloon nativo		
#define BALLOON_USE_EXTENDED		1											// usa il balloon esteso

/*
	CTrayIconW
*/
class CTrayIconW : public CWnd
{
public:
	CTrayIconW();
	virtual ~CTrayIconW();

	BOOL		Create				(HWND	hWnd,
									HICON	hIcon,
									UINT	nMsg,
									UINT	nMenuId,
									UINT	nBlankIconId = (UINT)-1L,
									LPCWSTR	lpcwszTooltip = NULL);

	BOOL		Destroy				(void);

	BOOL		SetIcon				(HICON hIcon);
	BOOL		SetIcon				(UINT nIconId);
	HICON		GetIcon				(void) const {return(m_hIcon);}

	BOOL		SetToolTip			(LPCWSTR lpcwszTooltip = NULL);
	
	LPCWSTR		GetToolTip			(void) const {return(m_szToolTipTextW);}
	
	void		SetCssStyles		(LPCWSTR lpcwszCssStyles);
	
	void		SetBalloonWrap		(int nChars) {m_nWrap = (nChars >= 10 && nChars <= 100 ? nChars : 50);}
	void		SetBalloonType		(int nType) {m_nBalloonType = nType;}
	void		SetBalloonCloseIcon	(int nID) {m_nCloseIconID = nID;}
	void		SetOnBalloonClick	(HWND hWndNotify,UINT nMsgNotify) {m_hWndClickNotify = hWndNotify; m_nMsgClickNotify = nMsgNotify;}
	
	void		Balloon				(
									LPCWSTR	lpcwszTitle = NULL,
									LPCWSTR	lpcwszText = NULL,
									UINT	nIconType = MB_ICONINFORMATION,
									UINT	nTimeout = BALLOON_DEFAULT_TIMEOUT,
									UINT	nIconID = (UINT)-1L,
									LPCWSTR	lpcwszIconFileName = NULL,
									CSize	iconSize = CSize(16,16),
									UINT	nCloseIconID = (UINT)-1L
									);

	static VOID CALLBACK BalloonTimerProc(HWND hWnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);

protected:
	void		OnDestroy		(void);

	BOOL			m_bIsValid;										// flag avvenuta creazione
	NOTIFYICONDATAW	m_NotifyIconDataW;								// struttura dati per l'icona della tray area (Wide)
	HWND			m_hWndParent;									// handle della finestra a cui inviare i messaggi
	HICON			m_hIcon;										// handle dell'icona
	HICON			m_hBalloonIcon;									// handle dell'icona per il balloon (esteso)
	BOOL			m_bSharedIcon;									// per sapere quando rilasciare l'handle di cui sopra
	UINT			m_nMenuId;										// id risorsa per il menu popup
	UINT			m_nBlankIconId;									// id risorsa per l'icona da usare per il tracking nella tray area
	wchar_t			m_szToolTipTextW[TRAYICON_MAX_TOOLTIP_TEXT+1];	// testo per il tooltip per l'icona (Wide)
	UINT			m_nTimeout;										// timeout per il balloon
	UINT			m_nTimerId;										// timer per il balloon
	CTrayIconPosition m_TrayPosition;								// per determinare le coordinate dell'icona nella tray area
	CPPToolTipW		m_TrayTooltip;									// per il (tooltip) balloon esteso
	CWindowsVersion	m_winVer;										// per determinare il tipo di balloon
	int				m_nBalloonType;									// tipo balloon in uso (0=nativo, 1=esteso)
	UINT			m_nCloseIconID;									// id risorsa per icona chiusura, solo per esteso
	wchar_t			m_szCssStylesW[512];							// stili css per l'html utilizzato nel balloon (esteso) (Wide)
	int				m_nWrap;										// ogni quanti caratteri andare a capo nel balloon
	HWND			m_hWndClickNotify;								// finestra a cui inviare il messaggio
	UINT			m_nMsgClickNotify;								// ID del messaggio custom (es. WM_USER + X)

	DECLARE_MESSAGE_MAP()
};

#endif // _CTRAYICON_H