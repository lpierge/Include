/*$
	win32gdi.h
	Helpers e generiche relative al maneggio dell'interfaccia grafica.
	Luca Piergentili, Feb '26

	Vedi le note in win32gdi.c
*/
#ifndef _WIN32GDI_H
#define _WIN32GDI_H 1

#include "window.h"
#include <stdlib.h>
#include <math.h>
#include "textdef.h"
#include "textedbmp.h"

/* interfaccia C++ */
#ifdef __cplusplus
  extern "C" {
#endif

#define		USE_PIXEL_POINTS		1
/*#define	USE_TYPOGRAPHIC_POINTS	1*/

/*
	ICONHEADER
*/
typedef struct _iconheader {
	WORD nReserved;			/* Reserved. Should always be 0. */
	WORD nImageType;		/* image type: 1 for icon (.ICO) image, 2 for cursor (.CUR) image. Other values are invalid. */
	WORD nImageCount;		/* number of images in the file. */
} ICONHEADER;

/*
	ICONIMAGE
*/
typedef struct _iconimage {
	BYTE	nWidth;			/* image width in pixels. Can be any number between 0 to 255. Value 0 means image width is 256 pixels. */
	BYTE	nHeight;		/* image height in pixels. Can be any number between 0 to 255. Value 0 means image height is 256 pixels. */
	BYTE	nColors;		/* number of colors in the color palette. Should be 0 if the image is truecolor. */
	BYTE	nReserved;		/* Reserved. Should be 0.[Notes 1] */
	WORD	nColorPlanes;	/* In .ICO format: Specifies color planes. Should be 0 or 1.[Notes 2],in .CUR format: Specifies the horizontal coordinates of the hotspot in number of pixels from the left. */
	WORD	nBitesxPixel;	/* In .ICO format: Specifies bits per pixel. [Notes 3], in .CUR format: Specifies the vertical coordinates of the hotspot in number of pixels from the top. */
	DWORD	nBitmapSize;	/* size of the bitmap data in bytes */
	DWORD	nBitmapOfs;		/* offset of bitmap data address in the file */
} ICONIMAGE;

int GetBiggerIconSize(LPCSTR lpcszIconFile,int nPreferredSize);

/*
	PSETPROCESSDPIAWARENESSCONTEXT
*/
typedef BOOL (WINAPI *PSETPROCESSDPIAWARENESSCONTEXT)(DPI_AWARENESS_CONTEXT);

void			EnableHighDPI				(void);

#define MONITOR_DATA_DEVICE_NAME_LEN 128

/*
	MONITOR_RESOLUTION
*/
typedef struct _monitor_resolution {
	BOOL	isPrimary;				/* monitor primario o secondario */
	HMONITOR hMonitor;				/* handle del monitor */
	int		deviceNumber;			/* il numero monitor reale, quello che sta alla fine della stringa "\\.\DISPLAY1" */
	char	deviceName[MONITOR_DATA_DEVICE_NAME_LEN+1]; /* nome del device */
	int		offset;					/* valore coord. rcMonitor.left */
	int		screen_width;			/* area schermo */
	int		screen_height;
	int		virtual_screen_width;	/* area schermo virtuale */
	int		virtual_screen_height;
	int		dpi_x;					/* DPI */
	int		dpi_y;
} MONITOR_RESOLUTION;

/*
	MONITOR_DATA
*/
typedef struct _monitor_data {
	BOOL	isPrimary;				/* flag per monitor primario */
	HMONITOR hMonitor;				/* handle del monitor */
	int		deviceNumber;			/* il numero monitor reale, quello che sta alla fine della stringa "\\.\DISPLAY1" */
	char	deviceName[MONITOR_DATA_DEVICE_NAME_LEN+1]; /* nome del device */
	RECT	monitorArea;			/* area totale */
	RECT	workingArea;			/* area di lavoro (totale - taskbar, gadgets, etc.) */
	long	currentGPUModeWidth;	/* risoluzione modalita' grafica corrente */
	long	currentGPUModeHeight;	/* risoluzione modalita' grafica corrente */
	int		freqHz;					/* frequenza */
} MONITOR_DATA;

/*
	ENUM_MONITOR_CONTEXT
*/
typedef struct _enum_monitor_context {
	MONITOR_DATA* pArray;			/**/
	int count;						/* totale dei monitor */
    int target;						/* contatore per monitor cercato */
    int current;					/* contatore x monitor corrente */
    HMONITOR hMonitor;				/* handle del monitor */
} ENUM_MONITOR_CONTEXT;

/*
	per gestione monitor
*/
void			GetMonitorResolution		(UINT* nWidth,UINT* nHeight);
void			GetPrimaryMonitorResolution	(MONITOR_RESOLUTION* pMonitorResolution);
int				GetVirtualMonitorArea		(RECT* rVirtualArea);
inline int		GetTotalMonitors			(void) {return(GetSystemMetrics(SM_CMONITORS));}

BOOL			EnumerateOneMonitor			(int nMonitorProg,MONITOR_RESOLUTION* pMonitorResolution);
BOOL CALLBACK	EnumOneMonitorCallback		(HMONITOR hMonitor,HDC hdcMonitor,LPRECT lprcMonitor,LPARAM lParam);
int				EnumerateAllMonitors		(MONITOR_DATA** ppMonitorData);
BOOL CALLBACK	EnumAllMonitorsCallback		(HMONITOR hMonitor,HDC hdcMonitor,LPRECT lprcMonitor,LPARAM dwData);
int				EnumerateDisplayDevices		(DISPLAY_DEVICE** ppDisplayDevice);

BOOL			GetMonitorIDFromGDIName		(LPCSTR szGDIName,LPWSTR szOutMonitorID,UINT cchMax);
BOOL			GetGDINameFromMonitorID		(LPCWSTR wszMonitorID,char* szOutGDIName,UINT cchMax,int* pnOutMonitorNumber);

/*
	TASKBAR_ORIENTATION
*/
typedef enum _taskbar_orientation {
    TKB_UNKNOWN = 0,
    TKB_LEFT,
    TKB_TOP,
    TKB_RIGHT,
    TKB_BOTTOM
} TASKBAR_ORIENTATION;

/*
	TASKBAR_INFO
*/
typedef struct _taskbar_info {
	HWND	hWnd;					/* handle della ventana */
	HMONITOR hMonitor;				/* handle del monitor associato */
	BOOL	isTaskbarPrimary;		/* se e' primaria */
	BOOL	isRelatedMonitorPrimary;/* se e' relativa al monitor principale */
    BOOL    isVisible;				/* flags status */
    BOOL    isAutoHide;
    BOOL    isAlwaysOnTop;
    TASKBAR_ORIENTATION orientation;/* orientazione: left, top, bottom, right */
    RECT    rect;					/* coordinate riquadro taskbar */
    int     width;					/* larghezza taskbar */
    int     height;					/* altezza taskbar */
} TASKBAR_INFO;

/*
	ENUM_TASKBAR_INFO
*/
typedef struct _enum_taskbar_info {
	TASKBAR_INFO** pArray;			/* puntatore per l'array di TASKBAR_INFO */
	int count;						/* totale elementi */
} ENUM_TASKBAR_INFO;

/*
	per gestione taskbar
*/
BOOL			IsTaskbarActuallyVisible	(HWND hTaskbar);
BOOL			GetTaskbarInfo				(HWND hTaskbar,TASKBAR_INFO* info);
BOOL CALLBACK	EnumTaskbarProc				(HWND hwnd,LPARAM lParam);
int				EnumerateAllTaskbars		(ENUM_TASKBAR_INFO* pTaskbarData);

/*
	per gestione finestre
*/
BOOL			SetWindowTransparency		(HWND hWnd,BYTE alpha);

/*
	colori console
*/
#define FG_BLACK    0
#define FG_BLUE     1
#define FG_GREEN    2
#define FG_CYAN     3
#define FG_RED      4
#define FG_MAGENTA  5
#define FG_YELLOW   6
#define FG_WHITE    7
#define FG_GRAY     8

#define BG_BLACK    0
#define BG_BLUE     1
#define BG_GREEN    2
#define BG_CYAN     3
#define BG_RED      4
#define BG_MAGENTA  5
#define BG_YELLOW   6
#define BG_WHITE    7

/*
	SETCONSOLECOLORS()

	Imposta il colore per il testo e lo sfondo della console, da usare
	con SetConsoleTextAttribute().
	Notare che SetConsoleTextAttribute() richiede un HANDLE da ricavare
	con GetStdHandle(), NON un HWND ricavato con GetConsoleWindow().

	esempio:
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout,SETCONSOLECOLORS(nBgColor,nFgColor));
*/
#define SETCONSOLECOLORS(bg,fg) (((bg) << 4) | (fg))

/*
	helpers per gestione aree, rect, etc.
*/
int				CompareRectArea				(const RECT* r1,const RECT* r2);
int				CompareRectWidth			(const RECT* r1,const RECT* r2);
int				CompareRectHeight			(const RECT* r1,const RECT* r2);
void			NormalizeRect				(const RECT* r,RECT* rNormalized);
int				CalculateCoverDimensions	(const RECT* rArea,const RECT* rImage,RECT* rResult);
int				CalculateFitDimensions		(const RECT* rArea,const RECT* rImage,RECT* rResult);
void			RemapTextPositionToFill		(const RECT* rArea,const RECT* rImage,const RECT* rImgScaled,TEXT_ALIGNMENT mode,int margin_x,int margin_y,long* outX,long* outY,double* ratio);
BOOL			CalculateDisplayTextSize	(LPCSTR lpcszDeviceName,int nFontSize,LPCSTR lpcszFontName,LPCSTR lpcszText,int* nTextWidth,int* nTextHeight,SIZE* szPadding);
BOOL			CalculateDisplayTextPosition(TEXT_POSITION* textPosition,int textWidth,int textHeight,int nImageWidth,int nImageHeight,TASKBAR_INFO* taskbarInfo,SIZE* szPadding);

/* interfaccia C++ */
#ifdef __cplusplus
  }
#endif

#endif /* _WIN32GDI_H */
