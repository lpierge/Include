/*$
	CImage.h
	Classe base per l'interfaccia con le librerie.
	Luca Piergentili, 01/09/00
	lpiergentili@yahoo.com

	Classe base per l'oggetto immagine, deriva dalla classe madre (CImageObject) e definisce i metodi
	richiesti, alcuni in proprio, altri li delega alle derivate che li implementano a seconda delle
	caratteristiche delle librerie che interfacciano.
	La classe madre (CImageObject) definisce come deve essere l'oggetto immagine mentre CImage e' la
	implementazione che fa da ponte (interfaccia) verso le librerie esterne, ossia usa le librerie
	esterne per implementare l'oggeto definito dalla classe madre.

	GetHeaderInfo() e GetColorSpace() sono di utilita' generale e vengono quindi definite come static
	in modo che possano essere usate senza istanziare la classe.

	Ad memoriam - Nemo me impune lacessit.
*/
#ifndef _CIMAGE_H
#define _CIMAGE_H 1

#include "pragma.h"
#include "window.h"
#include "CImageObject.h"
#include <vector>

//	MSVC:
//	#pragma push_macro("LOCAL")
//	#undef LOCAL
//	#define LOCAL
// 
//	GLib, GTK:
#ifdef LOCAL
    #define C__LOCAL_SAVED LOCAL
    #undef LOCAL
    #define C__LOCAL_WAS_DEFINED
#endif

/*
	LOCAL
	unico scopo distinguere visualmente a prima lettura i membri specifici, 
	ossia che NON derivano da CImageObject, ma che sono esclusivamente propri
	possono o no essere virtuali verso le classi derivate relative
*/
#define LOCAL

/*
	STATIC
	unico scopo distinguere visualmente a prima lettura i membri specifici, che 
	NON derivano da CImageObject e che possono essere chiamati SENZA istanziare 
	la classe
*/
#define STATIC static

/*
	VISUAL_GRAYSCALE_PARAMS
*/
#define VISUAL_GRAYSCALE_PARAMS 100,10,99.5f

/*
	PNG_FORMAT_INFO
*/
typedef struct _png_format_info {
    int bpp;        // bits per pixel reali (24, 32, 8, ecc.)
    int bpc;        // bits per channel (1,2,4,8,16)
	int	width;		// larghezza
	int height;		// altezza
    int channels;   // numero di canali
    BYTE colorType; // PNG color type
} PNG_FORMAT_INFO;

/*
	CImage
*/
class CImage : public CImageObject
{
public:
	CImage();
	virtual ~CImage() {}

	virtual LPCSTR			GetLibraryName		(void) {return("CImage");}
	virtual LPCSTR			GetFullPathFileName	(void) {return(m_szFileName);}
	virtual LPCSTR			GetFileName			(void);
	virtual DWORD			GetFileSize			(void) {return(m_InfoHeader.filesize);}

	virtual int				GetWidth			(void) {return(0);}
	virtual int				GetHeight			(void) {return(0);}
	
	virtual float			GetXRes				(void) {return(m_InfoHeader.xres);}
	virtual float			GetYRes				(void) {return(m_InfoHeader.yres);}
	virtual void			SetXRes				(float nXRes) {m_InfoHeader.xres = nXRes;}
	virtual void			SetYRes				(float nYRes) {m_InfoHeader.yres = nYRes;}
	virtual int				GetURes				(void) {return(m_InfoHeader.restype);}
	virtual void			SetURes				(UINT nRes);
	virtual void			GetDPI				(float& nXRes,float& nYRes) {nXRes = m_InfoHeader.xres; nYRes = m_InfoHeader.yres;}

	virtual int				GetCompression		(void) {return(m_InfoHeader.compression);}
	virtual void			SetCompression		(int nCompression) {m_InfoHeader.compression = nCompression;}
	virtual int				GetQuality			(void) {return(m_InfoHeader.quality);}
	virtual void			SetQuality			(int nQuality) {m_InfoHeader.quality = nQuality;}
	virtual BOOL			GetQualityRange		(int& nMin,int& nMax) {nMin = nMax = -1; return(FALSE);}
	
	virtual int				GetAlignment		(void) {return(WINDOWSALIGNMENT);}
	virtual UINT			GetBytesWidth		(UINT nWidth,UINT nBitsPerPixel,UINT nAlig) {return(WIDTHBYTES((nWidth * nBitsPerPixel),nAlig));}
	virtual UINT			GetBytesWidth		(void) {return(GetBytesWidth(GetWidth(),GetBPP(),GetAlignment()));}
	virtual PHOTOMETRIC		GetPhotometric		(void);

	virtual int				GetMaxPaletteColors	(void) {return(MAXPALETTECOLORS);}
	virtual UINT			GetNumColors		(void);
	virtual BOOL			CountBWColors		(unsigned int* pColors,unsigned char nNumColors);
	virtual BOOL			CountRGBColors		(COLORREF* pColors,unsigned int* pCountColors,unsigned char nNumColors);

	virtual UINT			GetBPP				(void) {return(0);}
	virtual UINT			GetOriginalBPP		(void) {return(0);}
	virtual UINT			ConvertToBPP		(UINT nBitsPerPixel,UINT nFlags = 0,RGBQUAD* pPalette = NULL,UINT nColors = 0);

	virtual COLORREF		GetPixel			(int /*x*/,int /*y*/) {return(RGB(255,255,255));}
	virtual void			SetPixel			(int /*x*/,int /*y*/,COLORREF /*colorref*/) {}
	virtual void*			GetPixels			(void) {return(NULL);} // void* (dati, pixel grezzi)
	virtual LPBITMAPINFO	GetBMI				(void) {return(NULL);} // BITMAPINFO* (header + palette)
	virtual UINT			GetMemUsed			(void) {return(0);}

	virtual BOOL			IsRGBOrder			(void) {return(TRUE);}

	// DIB order:	
	// standard formati moderni (jpg, png, etc) -> top-down (-1), ossia l'origine (0,0) e' l'angolo in alto a sinistra
	// bitmap classiche di Windows (DIB)        -> bottom-up (1), ossia l'origine (0,0) e' l'angolo in basso a sinistra
	// la definizione qui e' simbolica, ogni libreria DEVE definire il suo proprio

	virtual int				GetDIBOrder			(void) {return(0);} // si DEVE implementare nelle derivate
	virtual WORD			GetDIBNumColors		(LPSTR lpbi);
	virtual HPALETTE		CreateDIBPalette	(LPBITMAPINFO lpbmi);
	virtual BOOL			SetPalette			(UINT /*nIndex*/,UINT /*nColors*/,RGBQUAD* /*pPalette*/) {return(FALSE);}
	virtual HDIB			GetDIB				(UINT* pSize = NULL);
	virtual BOOL			SetDIB				(HDIB hDib,int nOrientation = 1);
	
	virtual IMAGE_TYPE		GetType				(void) {return(m_InfoHeader.type);}
	virtual DWORD			UpdateHeaderInfo	(void) {return(0L);} // si DEVE implementare nelle derivate, NON qui

	STATIC LOCAL DWORD		GetHeaderInfo		(LPCSTR lpcszFileName,LPIMAGEHEADERINFO pHeaderInfo); // - static - si implementa solo qui, NON nelle derivate
	STATIC LOCAL int		CheckPNGHeader		(LPCSTR lpcszFileName,PNG_FORMAT_INFO* pngInfo);
	LOCAL LPCSTR			GetColorSpace		(void);  // si implementa solo qui, NON nelle derivate
	STATIC LOCAL LPCSTR		GetColorSpace		(COLOR_TYPE_STD color_type_std); // - static -  si implementa solo qui, NON nelle derivate

	LOCAL HBITMAP			CreateBitmap		(void); // si implementa solo qui, NON nelle derivate
	LOCAL virtual CImage*	CreateImage			(void) {return(NULL);} // si DEVE implementare nelle derivate per creare una nuova istanza (propria)
	
	virtual BOOL			Create				(BITMAPINFO* /*pBitmapInfo*/,void* /*pData*/ = NULL) {return(FALSE);}
	virtual BOOL			Load				(LPCSTR lpcszFileName,DWORD& dwError) {dwError=(DWORD)-1L; return(FALSE);}
	virtual BOOL			LoadFromUrl			(LPCSTR lpcszUrl) {return(FALSE);}
	virtual BOOL			Unload				(void) {return(FALSE);}
	virtual BOOL			IsLoaded			(void) {return(FALSE);}
	virtual BOOL			Save				(LPCSTR /*lpcszFileName*/,LPCSTR /*lpcszFormat*/,DWORD /*dwFlags*/ = 0L) {return(FALSE);}
	LOCAL BOOL				Flush				(LPCSTR lpcszFileName);
	virtual BOOL			Draw				(HDC hDC, int x, int y);
	virtual HANDLE			Copy				(RECT& rect);
	virtual BOOL			Paste				(int x,int y,HANDLE hDib);
	LOCAL BOOL				PasteWithOuterShadow(int x, int y, HANDLE hDIB, int nShadow, RECT rcTarget, COLORREF clrShadow);
	virtual CImageObject*	Crop				(int x,int y,int width,int height);
	virtual UINT			Stretch				(RECT& /*rcSize*/,BOOL /*bAspectRatio*/ = TRUE) {return(GDI_ERROR);}
	virtual CImageObject*	Rescale				(int nWidth,int nHeight) {return(RescaleLanczos3(nWidth,nHeight));}

	virtual BOOL			LockData			(void) {return(TRUE);}
	virtual BOOL			UnlockData			(BOOL /*bModified*/ = FALSE) {return(TRUE);}

	LOCAL virtual void		SetLastErrorDescriptionEx(LPCSTR pszFormat,...);
/*
						CImage	paintLib	NexgenIPL	FreeImage	ImageMagick		# parametri, range
	Blur					.		.			X			X			X				(1)			. (NexgenIPL), 1:10 (FreeImage), 0:50 (CImageMagick)
	Brightness				X		.			X			X			X				(1)			-100:100
	ColorShift				X		.			.			.			.				(5)			-255:255, -255:255, -255:255, 0:2, T/F
	ColorSwap				X		.			.		   <x>			.				(1)			0:6
	Contrast				X		X			X			X			.				(1)			-100:100
	Echo					X		.			.			.			.				(4)			0:10, 0:100, 0:100, T/F
	EdgeEnhance				.		.			X			.			.				(0)	
	Emboss					.		.			X			.			.				(0)
	Equalize				X		.			.			.			.				(0)			0:1
	FindEdge				.		.			X			.			.				(1)			0:100
	GammaCorrection			X		.			X			X			.				(1)			0:10 (CImage), 0:5 (NexgenIPL), 0:10 (FreeImage)
	GhostTrail				X		.			.			.			.				(4)			1:10, 1:50, 0.0:1.0, T/F
	Grain					X		.			.			.			.				(1)			0:100
	Grayscale						X			X			X			.				(0) 
	HalftoneBW				X		.			.			.			.				(0) 
	HalftoneColor			X		.			.			.			.				(0) 
	Hue						X		.			X			X			.				(1)			-180:180
	Intensity				.		X			.			.			.				(1)			-100:100
	JitterHorizontal		X		.			.			.			.				(1)			0:16
	JitterSinusoidal		X		.			.			.			.				(1)			0:50
	Median					X		.			-			.			.				(1)			0.0:1.0 (CImage), 1:30 (NexgenIPL)
	MirrorHorizontal		X		.			X			X			X				(0) 
	MirrorVertical			X		.			X			X			X				(0) 
	Negate					X		X			.			X			.				(0) 
	Noise					X		.			.			.			.				(2)			0:2, 0:100
	Pixelate				X		.			.			.			.				(2)			2:32
	PixelSort				X		.			.			.			.				(4)			0:3, 0:3, 0:255, 0:255
	Posterize				X		.			X			X			.				(1)			2:255
	Rotate90Left			.		X			X			X			.				(0) 
	Rotate90Right			.		X			X			X			.				(0) 
	Rotate180				.		X			X			X			.				(0) 
	Saturation				X		.			X			.			.				(1)			-100:100
	Sharpen					X		.			X			X			.				(1)			-100:100 (CImage), . (NexgenIPL), 0:100 (FreeImage)
	Test					X		.			.			.			.				(0)
*/

	LOCAL LPCSTR			EnumFilters			(int& iterator);
	LOCAL LPCSTR			EnumValidFilters	(int& iterator,int& index);
	LOCAL int				CountFilters		(void);

	virtual UINT			Blur				(void); // {return(NotImplemented("Blur"));}
	virtual UINT			Brightness			(void);
	virtual UINT			ColorShift			(void);
	virtual UINT			ColorSwap			(void);
	virtual UINT			Contrast			(void);
	virtual UINT			Echo				(void);
	virtual UINT			EdgeEnhance			(void) {return(NotImplemented("EdgeEnhance"));}
	virtual UINT			Emboss				(void) {return(NotImplemented("Emboss"));}
	virtual UINT			Equalize			(void);
	virtual UINT			FindEdge			(void) {return(NotImplemented("FindEdge"));}
	virtual UINT			GammaCorrection		(void);
	virtual UINT			GhostTrail			(void);
	virtual UINT			Grain				(void);
	virtual UINT			Grayscale			(void) {return(NotImplemented("Grayscale"));}
	virtual UINT			HalftoneBW			(void);
	virtual UINT			HalftoneColor		(void);
	virtual UINT			Hue					(void);
	virtual UINT			Intensity			(void) {return(NotImplemented("Intensity"));}
	virtual UINT			JitterHorizontal	(void);
	virtual UINT			JitterSinusoidal	(void);
	virtual UINT			Median				(void);
	LOCAL virtual UINT		Mirror				(UINT nDirection/* 0 = horiz, 1 = vert */);
	virtual UINT			MirrorHorizontal	(void) {return(Mirror(1));}
	virtual UINT			MirrorVertical		(void) {return(Mirror(0));}
	virtual UINT			Negate				(void);
	virtual UINT			Noise				(void);
	virtual UINT			Pixelate			(void);
	virtual UINT			PixelSort			(void);
	virtual UINT			Posterize			(void);
	virtual UINT			Rotate90Left		(void) {return(NotImplemented("Rotate90Left"));}
	virtual UINT			Rotate90Right		(void) {return(NotImplemented("Rotate90Right"));}
	virtual UINT			Rotate180			(void) {return(NotImplemented("Rotate180"));}
	virtual UINT			Saturation			(void);
	virtual UINT			Sharpen				(void);
	virtual UINT			Test				(void);

	STATIC LOCAL double		HuetoRGB			(double m1,double m2,double h);
	STATIC LOCAL COLORREF	HLStoRGB			(const double& H,const double& L,const double& S);
	STATIC LOCAL void		RGBtoHSL			(COLORREF rgb,double* H,double* S,double* L);

	LOCAL BOOL				IsTrueGrayscale		(void);
	LOCAL BOOL				IsVisualGrayscale	(void);
	LOCAL BOOL				IsVisualGrayscaleStatistical(int maxPixelsToCheck /*= 100*/,int tolerance /*= 3*/,float grayThresholdPercent /*= 95.0f*/);

private:

	// helpers
	void					ApplyBlur2D		(BYTE* dst, const BYTE* src, UINT width, UINT height, UINT stride, int bytesPerPixel, int radius, bool topDown);

	UINT					ColorShift		(int redShift,int greenShift,int blueShift,int mode,BOOL preserveLuminance);
	int						Clamp			(int value,int minVal,int maxVal);

	UINT					Echo			(int numLayers /* = 2*/,int zoomPercent/* = 10*/,int opacityPercent/* = 30*/,BOOL applyBlur /* = FALSE*/);
	COLORREF				BlendColors		(COLORREF color1,COLORREF color2,float alpha);

	UINT					GhostTrail		(int numGhosts /*= 3*/, int maxOffset /*= 10*/, float baseAlpha /*= 0.3f*/, BOOL chromatic /*= FALSE*/);
	void					Ghosterize		(BYTE* dst,const BYTE* src,UINT width,UINT height,UINT stride,UINT bpp,int offsetX,int offsetY,float alpha,BOOL chromatic,int ghostIndex);

	UINT					PixelSort		(int direction/* = 0*/,int criterion/* = 0*/,int threshold/* = 128*/,int maxLength/* = 0*/);
	void					SortHorizontal	(BYTE* pixels,UINT width,UINT height,UINT stride,int bpp,int criterion,int threshold,int maxLength);
	void					SortVertical	(BYTE* pixels,UINT width,UINT height,UINT stride,int bpp,int criterion,int threshold,int maxLength);
	void					SortRadial		(BYTE* pixels,UINT width,UINT height,UINT stride,int bpp,int criterion,int threshold,int maxLength)		{}
	void					SortDiagonal	(BYTE* pixels,UINT width,UINT height,UINT stride,int bpp, int criterion,int threshold,int maxLength)	{}
	void					SortSegment		(BYTE* row,int start,int end,int bpp,int criterion,int maxLength);
	int						GetPixelValue	(BYTE* pixel,int criterion);

    void					ApplyMedianChannel(BYTE* dst,const BYTE* src,UINT width, UINT height, UINT stride,int bytesPerPixel, int channel,int halfKernel, int kernelSize,bool topDown, bool hasAlpha);
    void					InitHistogramForColumn(int histogram[256], const BYTE* src,UINT width, UINT height, UINT stride,int bytesPerPixel, int channel,UINT startX, UINT startY,int halfKernel, bool topDown, bool hasAlpha);
    void					UpdateHistogramForShift(int histogram[256], const BYTE* src,UINT width, UINT height, UINT stride,int bytesPerPixel, int channel,UINT currentX, UINT currentY,int halfKernel, bool topDown, bool hasAlpha);
	int						FindMedianFromHistogram(const int histogram[256], int medianTarget);

    COLORREF				CalculateBlockMedian(const BYTE* src, UINT width, UINT height, UINT stride,int bytesPerPixel, int startX, int startY,int blockWidth, int blockHeight);
    void					FillBlock		(BYTE* dst, UINT width, UINT height, UINT stride,int bytesPerPixel, int startX, int startY,int blockWidth, int blockHeight, COLORREF color);

	UINT					EqualizeGHE		(void);
	UINT					EqualizeRGB		(void);
	UINT					EqualizeRGBIntensity(double intensity);

	CImage*					_RescaleLanczos3(int newWidth, int newHeight);
	CImage*					RescaleLanczos3	(int newWidth, int newHeight);

	// TEST

	UINT					NotImplemented	(LPCSTR pMethod);
};

//	MSVC:
//	#pragma pop_macro("LOCAL")
// 
//	GLib, GTK:
#undef LOCAL
#ifdef C__LOCAL_WAS_DEFINED
    #define LOCAL C__LOCAL_SAVED
    #undef C__LOCAL_SAVED
    #undef C__LOCAL_WAS_DEFINED
#endif

#endif // _CIMAGE_H
