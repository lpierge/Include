/*$
	CFreeImage.h
	Classe derivata per interfaccia con FreeImage (http://freeimage.sourceforge.io/).
	Luca Piergentili, Nov '25

	Note:
	 - FreeImage NON usa DIB Windows ma un formato proprietario, quindi occhio.

	 - Esistono due versioni di FreeImage:
	   la originale (https://freeimage.sourceforge.io/) e
	   la resurrected (https://github.com/agruzdev/FreeImageRe) con supporto, tra le altre cose, per formato avif.

	   In locale, l'installazione del package e l'header relativo (FreeImage.h) per l'interfaccia si trovano in:
	   originale   -> L:\FreeImage   -> L:\FreeImage\Dist\x32\FreeImage.h
	   resurrected -> L:\FreeImageRe -> L:\FreeImageRe\Source\FreeImage.h

	   Impostare quindi nel progetto di Visual Studio il path corrispondente per gli include.		
	   
	   Riguardo alle librerie/dll, per la versione originale usare:
		FreeImage.lib/.dll a partire da L:\FreeImage\Dist\x32\
	   per la versione resurrected usare:
		FreeImage.lib/.dll, heif.lib/.dll e zlib.lib/.dll a partire da L:\FreeImageRe\build

	   Copiare quindi nel path per le librerie le .lib/.dll della FreeImage corrispondenti.

	- La macro FREEIMAGE_RESURRECTED circoscrive le differenze di codice tra versione originale/resurrected.
*/
#ifndef _CFREEIMAGE_H
#define _CFREEIMAGE_H 1

#include "window.h"
#include "ImageConfig.h"

#ifdef HAVE_FREEIMAGE_LIBRARY

// determina l'uso della FreeImage originale/resurrected
#define FREEIMAGE_RESURRECTED 1

#include "ImageLibraryName.h"
#include "CImage.h"
#include <string.h>
#include "FreeImage.h"

// il riferimento alla libreria per il linker sta qui sotto:

// crea la referenza alla DLL
// la versione originale nomina con "FreeImage", la resurrected con "FreeImageRe"
// qui usa la seconda e distingue tra versione di Debug e Release
#ifndef FREEIMAGE_LIB
  #ifdef DEBUG
    #pragma comment(lib,"FreeImageRed.lib")
  #else
    #pragma comment(lib,"FreeImageRe.lib")
  #endif
  #ifdef PRAGMA_MESSAGE_VERBOSE
    #pragma message("\t\t\t" __FILE__ "(" STR(__LINE__) "): automatically linking with FreeImageRe")
  #endif
#else
  #error current configuration is for FreeImage DLL, not static library
#endif

/*
	CFreeImage
*/
class CFreeImage : public CImage 
{
public:
	CFreeImage();
	virtual ~CFreeImage();

	LPCSTR			GetLibraryName		(void) {return(FREEIMAGE_LIB_NAME);}

//					GetFullPathFileName
//					GetFileName
//					GetFileSize

	int				GetWidth			(void) {return m_pImage ? FreeImage_GetWidth(m_pImage) : 0;}
	int				GetHeight			(void) {return m_pImage ? FreeImage_GetHeight(m_pImage) : 0;}

	float			GetXRes				(void);
	float			GetYRes				(void);
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
	PHOTOMETRIC		GetPhotometric		(void);

//					GetMaxPaletteColors
	UINT			GetNumColors		(void);
//					CountBWColors
//					CountRGBColors

	UINT			GetBPP				(void) {return m_pImage ? FreeImage_GetBPP(m_pImage) : 0;}
	UINT			GetOriginalBPP		(void) {return(m_InfoHeader.bppOriginal);}
	UINT			ConvertToBPP		(UINT nBitsPerPixel,UINT nFlags = 0,RGBQUAD* pPalette = NULL,UINT nColors = 0);

	COLORREF		GetPixel			(int x,int y);
	void			SetPixel			(int x,int y,COLORREF colorref);
	void*			GetPixels			(void);
	LPBITMAPINFO	GetBMI				(void);
	UINT			GetMemUsed			(void);

//					CreateBitmap

	BOOL			IsRGBOrder			(void) {return(FALSE);}

//					GetDIB
	BOOL			SetDIB				(HDIB hDib,int nOrientation = 1);
	int				GetDIBOrder			(void) {return(1);} // restituisce pixels bottom-up
//					GetDIBNumColors
//					CreateDIBPalette
	BOOL			SetPalette			(UINT nIndex,UINT nColors,RGBQUAD* pPalette);

//					GetType
	DWORD			UpdateHeaderInfo	(void);

	CImage*			CreateImage			(void) {return(new CFreeImage());}

	BOOL			Create				(BITMAPINFO* pBitmapInfo,void* pData);
	BOOL			Load				(LPCSTR lpcszFileName,DWORD& dwError);
//					LoadFromUrl
	BOOL			Unload				(void);
	BOOL			IsLoaded			(void) {return(m_pImage ? TRUE : FALSE);}
	BOOL			Save				(LPCSTR lpcszFileName,LPCSTR lpcszFormat,DWORD dwFlags = 0L);

//					Draw
//					Copy
//					Paste
//					Crop
	UINT			Stretch				(RECT& rc,BOOL keepAspectRatio = TRUE);
//					Rescale

//					LockData
//					UnlockData

	UINT			Blur				(void);
	UINT			Brightness			(void);
//					ColorShift
	UINT			ColorSwap			(void);
	UINT			Contrast			(void);
//					Deskew
//					Despeckle
//					Echo
//					EdgeEnhance
//					Emboss
//					Equalize
//					FindEdge
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
//					Median
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
//					Saturation
	UINT			Sharpen				(void);

protected:
	void			GaussianBlur32		(FIBITMAP* src,FIBITMAP* dst,int radius);
	UINT			SharpenUnsharpMask	(int intensity);
	BOOL			IsValid				(LPCSTR lpcszFunctionName);

	FIBITMAP*		m_pImage;
};

#endif // HAVE_FREEIMAGE_LIBRARY
#endif // _CFREEIMAGE_H