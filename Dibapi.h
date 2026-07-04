// dibapi.h
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

//LPI aggiunte varie, Luca Piergentili, 01/09/00

#ifndef _INC_DIBAPI
#define _INC_DIBAPI

/* Handle to a DIB */
#if 0 /* originale */
  DECLARE_HANDLE(HDIB);
#else
#ifndef _HDIB_DECLARED
  DECLARE_HANDLE(HDIB);
  #define _HDIB_DECLARED 1
#endif
#endif

/* DIB constants */
#define PALVERSION		0x300

//LPI
#define MAXPALCOLORS	256
//LPI

/* DIB Macros*/
#ifndef IS_WIN30_DIB
  #define IS_WIN30_DIB(lpbi)	((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))
#endif
#define RECTWIDTH(lpRect)	((lpRect)->right - (lpRect)->left)
#define RECTHEIGHT(lpRect)	((lpRect)->bottom - (lpRect)->top)

//LPI
// WIDTHBYTES performs DWORD-aligning of DIB scanlines.  The "bits"
// parameter is the bit count for the scanline (biWidth * biBitCount),
// and this macro returns the number of DWORD-aligned bytes needed
// to hold those bits.
//#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)
#define WIDTH_BYTES(bits)	(((bits) + 31) / 32 * 4)
//LPI

//LPI
BOOL	WINAPI	SaveDIBToFile	(HDIB hDib, const char* filename);
HDIB	WINAPI	WindowToDIB		(CWnd *pWnd,CRect* pScreenRect);
HDIB	WINAPI	BitmapToDIB		(HBITMAP hBitmap,HPALETTE hPal);
HBITMAP	WINAPI	DIBToBitmap		(HDIB hDib,HPALETTE hPal);
//LPI

/* Function prototypes */
BOOL      WINAPI  PaintDIB (HDC, LPRECT, HDIB, LPRECT, CPalette* pPal);
BOOL      WINAPI  CreateDIBPalette(HDIB hDIB, CPalette* cPal);
LPSTR     WINAPI  FindDIBBits (LPSTR lpbi);
DWORD     WINAPI  DIBWidth (LPSTR lpDIB);
DWORD     WINAPI  DIBHeight (LPSTR lpDIB);
WORD      WINAPI  PaletteSize (LPSTR lpbi);
WORD      WINAPI  DIBNumColors (LPSTR lpbi);
HGLOBAL   WINAPI  CopyHandle (HGLOBAL h);

BOOL      WINAPI  SaveDIB (HDIB hDib, CFile& file);
HDIB      WINAPI  ReadDIBFile(CFile& file);

#endif //!_INC_DIBAPI
