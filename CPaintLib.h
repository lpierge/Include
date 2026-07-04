/*$
	CPaintLib.h
	Classe derivata per l'interfaccia con la libreria paintlib.
	Luca Piergentili, 01/09/00
	lpiergentili@yahoo.com

	Ad memoriam - Nemo me impune lacessit.

	Nel progetto: propieta'-> C++ -> generale -> directory include -> aggiungere \paintlib\include;
*/
#ifndef _CPAINTLIB_H
#define _CPAINTLIB_H 1

#include "window.h"
#include "ImageConfig.h"

#ifdef HAVE_PAINTLIB_LIBRARY

#include "ImageLibraryName.h"
#include "CImage.h"
#include "paintLib.h"

// il riferimento alla libreria per il linker sta in paintLib.h
// distingue tra Debug e Release

/*
	CPaintLib
*/
class CPaintLib : public CImage
{
public:
	CPaintLib();
	virtual ~CPaintLib() {Unload();}

	LPCSTR			GetLibraryName		(void) {return(PAINTLIB_LIB_NAME);}
//					GetFullPathFileName
//					GetFileName
//					GetFileSize

	int				GetWidth			(void) {return(m_Image.GetWidth());}
	int				GetHeight			(void) {return(m_Image.GetHeight());}
	
	float			GetXRes				(void) {return(m_InfoHeader.xres);}
	float			GetYRes				(void) {return(m_InfoHeader.yres);}
	void			SetXRes				(float nXRes);
	void			SetYRes				(float nYRes);
//					GetURes
//					SetURes
	void			GetDPI				(float& nXRes,float& nYRes);

//					GetCompression
//					SetCompression
//					GetQuality
//					SetQuality
//					GetQualityRange

//					GetAlignment
//					GetBytesWidth
//					GetBytesWidth
//					GetPhotometric

//					GetMaxPaletteColors
	UINT			GetNumColors		(void) {return(m_Image.GetNumColors());}
//					CountBWColors
//					CountRGBColors

	UINT			GetBPP				(void) {return(m_Image.GetBitsPerPixel());}
	UINT			GetOriginalBPP		(void) {return(m_InfoHeader.bppOriginal);}
	UINT			ConvertToBPP		(UINT nBitsPerPixel,UINT nFlags = 0,RGBQUAD* pPalette = NULL,UINT nColors = 0);

	COLORREF		GetPixel			(int x,int y);
	void			SetPixel			(int x,int y,COLORREF colorref);
	void*			GetPixels			(void) {return(m_Image.GetBits());}
	LPBITMAPINFO	GetBMI				(void) {return((LPBITMAPINFO)m_Image.GetBMI());}
	UINT			GetMemUsed			(void) {return(m_Image.GetMemNeeded((LONG)m_Image.GetWidth(),(LONG)m_Image.GetHeight(),(WORD)m_Image.GetBitsPerPixel()));}

//					CreateBitmap

	BOOL			IsRGBOrder			(void) {return(FALSE);}

//					GetDIB
//					SetDIB
	int				GetDIBOrder			(void) {return(1);} // restituisce pixels bottom-up
//					GetDIBNumColors
//					CreateDIBPalette
	BOOL			SetPalette			(UINT nIndex,UINT nColors,RGBQUAD* pPalette);

//					GetType
	DWORD			UpdateHeaderInfo(void);

	CImage*			CreateImage			(void) {return(new CPaintLib());}

	BOOL			Create				(BITMAPINFO* pBitmapInfo,void *pData = NULL);
	BOOL			Load				(LPCSTR lpcszFileName,DWORD& dwError);
	BOOL			LoadFromUrl			(LPCSTR lpcszUrl);
	BOOL			Unload				(void);
	BOOL			IsLoaded			(void) {return(m_bIsValid);}
	BOOL			Save				(LPCSTR lpcszFileName,LPCSTR lpcszFormat,DWORD dwFlags = 0);

//					Draw
//					Copy
//					Paste
//					Crop
	UINT			Stretch				(RECT& drawRect,BOOL bAspectRatio = TRUE);
//					Rescale

//					LockData
//					UnlockData

//					Blur
//					Brightness
//					ColorShift
//					ColorSwap
	UINT			Contrast			(void);
//					Deskew
//					Despeckle
//					Echo
//					EdgeEnhance
//					Emboss
//					Equalize
//					FindEdge
//					GammaCorrection
//					GhostTrail
//					Grain
	UINT			Grayscale			(void);
//					HalftoneBW
//					HalftoneColor
//					Hue
	UINT			Intensity			(void);
//					IntensityDetect
//					JitterHorizontal
//					JitterSinusoidal
//					Median
//					MirrorHorizontal
//					MirrorVertical
	UINT			Negate				(void);
//					Noise
//					Pixelate
//					PixelSort
//					Posterize
	UINT			Rotate90Left		(void);
	UINT			Rotate90Right		(void);
	UINT			Rotate180			(void);
//					Saturation
//					Sharpen

protected:
	BOOL			LoadBitmap			(LPCSTR lpcszFileName);
	BOOL			SaveBitmap			(LPCSTR lpcszFileName,LPCSTR lpcszFormat,DWORD dwFlags);
	BOOL			Rotate				(double);
	BOOL			IsValid				(LPCSTR lpcszFunctionName);

	PLWinBmp		m_Image;
	BOOL			m_bIsValid;
};

#endif // HAVE_PAINTLIB_LIBRARY
#endif // _CPAINTLIB_H
