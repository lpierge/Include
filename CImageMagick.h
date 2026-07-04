/*$
	CImageMagick.h
	Classe derivata per l'interfaccia con la libreria ImageMagick.
	Luca Piergentili, 20/01/26

	Note:
	- ImageMagick piu' che una libreria grafica e' un vero e proprio sistema. L'idea iniziale di includerlo 
	  nell'interfaccia nacque esclusivamente dall'esigenza di poter leggere i files .avif, dato che in linea
	  generale, lavorare con ImageMagick e' un martirio.
	  In seguito, tale esigenza venne soddisfatta passando da FreeImage a FreeImageRe, ma l'interfaccia con
	  ImageMagick e' stata lasciata comunque come esempio di integrazione.
	  La sezione dei filtri di ImageMagick e' molto piu' completa (ma anche molto piu' complessa) rispetto
	  alle altre librerie, al momento l'integrazione e' puramente simbolica.
	
	- Definisce il namespace IM attorno all'include per MagickWand.h per evitare il conflitto con simboli
	  definiti con lo stesso nome nelle altra interfaccie (FreeImage ->RemoveAlphaChannel). Tale conflitto 
	  si manifesta nella classe CImageFactory, dove vengono inclusi gli headers di tutte le librerie.
	  Qui i simboli di ImageMagick devono essere preceduti dal suffisso "IM::", mentre in CImageMagick.cpp
	  bisogna usare la direttiva "using namespace IM;" subito dopo l'include di CImageMagick.h.
*/
#ifndef _CIMAGEMAGICK_H_
#define _CIMAGEMAGICK_H_ 1

#include "window.h"
#include "ImageConfig.h"

#ifdef HAVE_IMAGEMAGICK_LIBRARY

#include "ImageLibraryName.h"
#include "CImage.h"
#include <string.h>
namespace IM {
#include "MagickWand/MagickWand.h"
}

// il riferimento alla libreria per il linker sta qui sotto:

// crea la referenza alla DLL
// in realta ImageMagick usa uno sbotto di DLL, qui vengono incluse solo le due basiche
// referenzia solo la versione Release
#pragma comment(lib,"CORE_RL_MagickWand_.lib")
#pragma comment(lib,"CORE_RL_MagickCore_.lib")
#ifdef PRAGMA_MESSAGE_VERBOSE
  #pragma message("\t\t\t" __FILE__ "(" STR(__LINE__) "): automatically linking with ImageMagick DLLs")
#endif

/*
	CImageMagick
*/
class CImageMagick : public CImage
{
private:
    // variabile UNICA per tutte le istanze
	// ogni nuova istanza chiama MagickWandGenesis() nel costruttore, ma ImageMagick ha un sistema 
	// di inizializzazione globale, il che significa che si deve mantenere traccie delle istanze
//	static LONG s_nInstanceCount;

public:
//	static LONG GetInstanceCount(void) {return(s_nInstanceCount);}

    CImageMagick();
    virtual ~CImageMagick();

	LPCSTR			GetLibraryName		(void) {return(IMAGEMAGICK_LIB_NAME);}
//					GetFullPathFileName
//					GetFileName
//					GetFileSize

	int		        GetWidth			(void);
	int	            GetHeight			(void);

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
	UINT			GetBytesWidth		(UINT nWidth,UINT nBpp,UINT nAlig);
//					GetBytesWidth
//					GetPhotometric

//					GetMaxPaletteColors
	UINT			GetNumColors		(void);
//					CountBWColors
//					CountRGBColors

	UINT			GetBPP				(void);
	UINT			GetOriginalBPP		(void);
	UINT			ConvertToBPP		(UINT nBitsPerPixel, UINT nFlags = 0, RGBQUAD* pPalette = nullptr, UINT nColors = 0);

	COLORREF		GetPixel			(int x,int y);
	void			SetPixel			(int x,int y,COLORREF colorref);
	void*			GetPixels			(void);
	LPBITMAPINFO	GetBMI				(void);
	UINT			GetMemUsed			(void);

//					CreateBitmap

	BOOL			IsRGBOrder			(void) {return(FALSE);}

	HDIB			GetDIB				(UINT* pSize = NULL);
	BOOL			SetDIB				(HDIB hDib,int nOrientation = 1);
	int				GetDIBOrder			(void) {return(-1);} // restituisce pixels top-down
//					GetDIBNumColors
//					CreateDIBPalette
//					SetPalette

//					GetType
//					GetHeaderInfo
    DWORD			UpdateHeaderInfo	(void);

	//				GetColorSpace

	CImage*			CreateImage			(void); // NO, per il funzionamento interno di ImageMagick {return(new CImageMagick());}

	BOOL			Create				(BITMAPINFO* pBitmapInfo,void* pData = NULL);
	BOOL			Load				(LPCSTR lpcszFileName,DWORD& dwError);
//					LoadFromUrl
	BOOL			Unload				(void);
//					IsLoaded
	BOOL			Save				(LPCSTR lpcszFileName,LPCSTR lpcszFormat = nullptr,DWORD dwFlags = 0);

//					Draw
//					Copy
//					Paste
	CImageObject*	Crop				(int x,int y,int width,int height);
	UINT			Stretch				(RECT& rc,BOOL keepAspectRatio = TRUE);
	CImageObject*	Rescale				(int nWidth,int nHeight);

//					Text

//					LockData
//					UnlockData

	UINT			Blur				(void);
	UINT			Brightness			(void);
//					ColorShift
//					ColorSwap
//					Contrast
//					Echo
//					EdgeEnhance
//					Emboss
//					Equalize
//					FindEdge
//					GammaCorrection
//					GhostTrail
//					Grain
//					Grayscale
//					HalftoneBW
//					HalftoneColor
//					Hue
//					Intensity
//					JitterHorizontal
//					JitterSinusoidal
//					Median
	UINT			MirrorHorizontal	(void);
	UINT			MirrorVertical		(void);
//					Noise
//					Pixelate
//					PixelSort
//					Posterize
//					Rotate90Left
//					Rotate90Right
//					Rotate180
//					Saturation
//					Sharpen

protected:
    IM::MagickWand*	m_pWand;
    IM::PixelWand*	m_pPixel;						// pixel temporaneo per Get/SetPixel
    BITMAPINFO*		m_pBMI;							// header DIB
    void*           m_pPixelBuffer;					// buffer per GetPixels()
    size_t          m_bufferSize;					// dimensione corrente buffer
    BYTE*           m_pBMIHeader;					// buffer per BITMAPINFO
    size_t          m_BMIHeaderSize;				// dimensione corrente header

	BOOL			IsValid(LPCSTR lpcszFunctionName);
    
private:
	BOOL			EnsurePixelBuffer(void);		// assicura buffer sincronizzato
	BOOL			SyncBufferToMagickWand(void);	// buffer -> MagickWand
	BOOL			SyncMagickWandToBuffer(void);	// MagickWand -> buffer

	BOOL			m_bDataModified;
};

#endif // HAVE_IMAGEMAGICK_LIBRARY
#endif // _CIMAGEMAGICK_H_
