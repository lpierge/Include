/*$
	CNexgenIPL.h
	Classe derivata per interfaccia con NexgenIPL (v.2.9.6).
	Luca Piergentili, 01/09/00
	lpiergentili@yahoo.com

	Ad memoriam - Nemo me impune lacessit.
*/
#ifndef _CNEXGENIPL_H
#define _CNEXGENIPL_H 1

#include "window.h"
#include "ImageConfig.h"

#ifdef HAVE_NEXGENIPL_LIBRARY

#include "ImageLibraryName.h"
#include "CImage.h"
#include "NexgenIPL.h"

#include "martyrdom.h"

// il riferimento alla libreria per il linker sta in BTNexgenIPL.h
// solo versione Release

/*
	CNexgenIPL
*/
class CNexgenIPL : public CImage 
{
public:
	CNexgenIPL();
	virtual ~CNexgenIPL() {Unload();}

	LPCSTR			GetLibraryName		(void) {return(NEXGENIPL_LIB_NAME);}
//					GetFullPathFileName
//					GetFileName
//					GetfileSize

	int				GetWidth			(void) {return(m_pImage ? m_pImage->GetWidth() : 0);}
	int				GetHeight			(void) {return(m_pImage ? m_pImage->GetHeight() : 0);}
	
//					GetXRes
//					GetYRes
//					SetXRes
//					SetYRes
//					GetURes
//					SetURes
//					GetDPI

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
	UINT			GetNumColors		(void) {return(m_pImage ? m_pImage->GetNumColorEntries(GetBPP()) : 0);}
//					CountBWColors
//					CountRGBColors

	UINT			GetBPP				(void) {return(m_pImage ? m_pImage->GetBitsPerPixel() : 0);}
	UINT			GetOriginalBPP		(void) {return(m_InfoHeader.bppOriginal);}
	UINT			ConvertToBPP		(UINT nBitsPerPixel,UINT nFlags = 0,RGBQUAD* pPalette = NULL,UINT nColors = 0);

	COLORREF		GetPixel			(int x,int y) {return(m_pImage ? m_pImage->GetColorFromPixel(x,y) : RGB(255,255,255));}
	void			SetPixel			(int x,int y,COLORREF colorref) {if(m_pImage) m_pImage->SetColorForPixel(x,y,colorref);}
	void*			GetPixels			(void) {return(m_pImage ? m_pImage->GetBits() : NULL);}
	LPBITMAPINFO	GetBMI				(void) {return(m_pImage ? m_pImage->GetBitmapInfo() : NULL);}
	UINT			GetMemUsed			(void) {return(m_pImage ? m_pImage->GetImageSize(m_pImage->GetWidth(),m_pImage->GetHeight(),m_pImage->GetBitsPerPixel()) : 0);}

//					CreateBitmap

	BOOL			IsRGBOrder		(void) {return(FALSE);}

	HDIB			GetDIB				(UINT* pSize = NULL);
	BOOL			SetDIB				(HDIB hDib,int nOrientation = 1);
	int				GetDIBOrder			(void) {return(-1);} // restituisce pixels top-down
//					GetDIBNumColors
//					CreateDIBPalette
	BOOL			SetPalette			(UINT nIndex,UINT nColors,RGBQUAD* pPalette);

//					GetType
	DWORD			UpdateHeaderInfo	(void);

	CImage*			CreateImage			(void) {return(new CNexgenIPL());}

	BOOL			Create				(BITMAPINFO* pBitmapInfo,void *pData = NULL);
	/* LOCAL */void GetMartyrName		(char* szDest,size_t nDestSize);
	/* LOCAL */BOOL	LoadFile			(LPCSTR lpcszFileName);
	BOOL			Load				(LPCSTR lpcszFileName,DWORD& dwError);
//					LoadFromUrl
	BOOL			Unload				(void);
	BOOL			IsLoaded			(void) {return(m_pImage ? TRUE : FALSE);}
	BOOL			Save				(LPCSTR lpcszFileName,LPCSTR lpcszFormat,DWORD dwFlags = 0);

//					Draw
//					Copy
//					Paste
//					Crop
	UINT			Stretch				(RECT& drawRect,BOOL bAspectRatio = TRUE);
//					Rescale

//					LockData
//					UnlockData

	UINT			Blur				(void);
	UINT			Brightness			(void);
//					ColorShift
//					ColorSwap
	UINT			Contrast			(void);
//					Deskew
//					Despeckle
//					Echo
	UINT			EdgeEnhance			(void);
	UINT			Emboss				(void);
//					Equalize
	UINT			FindEdge			(void);
	UINT			GammaCorrection		(void);
//					GhostTrail
//					Grain
	UINT			Grayscale			(void);
//					HalftoneBW
//					HalftoneColor
	UINT			Hue					(void);
//					Intensity
//					IntensityDetect
//					JitterHorizontal
//					JitterSinusoidal
//	UINT			Median				(void);
	UINT			MirrorHorizontal	(void);
	UINT			MirrorVertical		(void);
	UINT			Negate				(void);
//					Noise
//					Pixelate
//					PixelSort
	UINT			Posterize			(void);
	UINT			Rotate90Left		(void);
	UINT			Rotate90Right		(void);
	UINT			Rotate180			(void);
	UINT			Saturation			(void);
	UINT			Sharpen				(void);

private:
	BOOL			FromDIBWrapper		(HDIB hDib,int nOrientation);

protected:
	BOOL			SaveTIFF			(LPCSTR lpcszInputFile);
	BOOL			IsValid				(LPCSTR lpcszFunctionName);
	
	BTCImageData*	m_pImage;
	BTCImageObject	m_NexgenObject;
};

#endif // HAVE_NEXGENIPL_LIBRARY
#endif // _CPNEXGENIPL__H
