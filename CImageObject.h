/*$
	CImageObject.h
	Classe base per la definizione dell'oggetto immagine.
	Luca Piergentili, 01/09/00
	lpiergentili@yahoo.com

	Implementa la classe base (astratta, con membri virtual puri) per la definizione dell'oggetto 
	di tipo immagine.
	E' la classe madre che stabilisce i metodi per le implementazioni (interfaccie) relative alle 
	librerie che verranno usate.
	Implementa il codice per la gestione dei filtri, dato che l'array relativo viene definito qui,
	affinche' le derivate possano poi riempirlo con i filtri che implementano, ed il codice per la 
	gestione minima dei tipi di file per i formati grafici.

	Ad memoriam - Nemo me impune lacessit.
*/
#ifndef _CIMAGEOBJECT_H
#define _CIMAGEOBJECT_H 1

#include <stdio.h>
#include <stdlib.h>
#include "strings.h"
#include "window.h"
#include "CNodeList.h"

//		NO_ERROR					0			// error.h, winerror.h
//		GDI_ERROR					0xFFFFFFFFL	// wingdi.h
//		ERROR_INVALID_FUNCTION					// winerror.h
//		ERROR_FILE_NOT_FOUND
//		ERROR_NOT_ENOUGH_MEMORY
//		ERROR_OUTOFMEMORY
//		ERROR_BAD_FORMAT
//		ERROR_INVALID_DATA
//		ERROR_INVALID_PARAMETER
#define ERROR_WRONG_BPP_FORMAT		1
#define	ERROR_INVALID_IMAGE			19650826L
#define	ERROR_UNSUPPORTED_FORMAT	19650827L
#define	ERROR_OUT_OF_MEMORY			19650828L

#define MAXPALETTECOLORS			256
#define PALVERSION					0x300
#define IS_WIN30_DIB(lpbi)			((*(LPDWORD)(lpbi))==sizeof(BITMAPINFOHEADER))

#ifndef WINDOWSALIGNMENT
  #define WINDOWSALIGNMENT			4
#endif
#ifndef WIDTHBYTES
  #define WIDTHBYTES(bits,alig)		(((bits)+((alig*8)-1))/(alig*8)*alig)
#endif

enum ORDERRGB{
	RGB_RED = 0,
	RGB_GREEN = 1,
	RGB_BLUE = 2
};
	
enum PHOTOMETRIC {
	NOPHOTOMETRIC,
	PHOTOMETRICMINISBLACK,
	PHOTOMETRICMINISWHITE,
	PHOTOMETRICPALETTE,
	PHOTOMETRICRGB
};

enum RESOLUTION {
	RESUNITNONE,
	RESUNITINCH,
	RESUNITCENTIMETER
};

#ifndef _HDIB_DECLARED
  DECLARE_HANDLE(HDIB);
  #define _HDIB_DECLARED 1
#endif

// tipo di operazione supportata dal formato
#define IMAGE_READ_FLAG		0x00000001
#define IMAGE_WRITE_FLAG	0x00000002
#define IMAGE_WEB_FLAG		0x00000004

/*
	IMAGE_TYPE
	elenco formati
*/
enum IMAGE_TYPE {
	NULL_PICTURE	= 0,
	AVI_PICTURE		= 1,
	BMP_PICTURE		= 2,
	CUR_PICTURE		= 3,
	DCX_PICTURE		= 4,
	GIF_PICTURE		= 5,
	ICO_PICTURE		= 6,
	JPEG_PICTURE	= 7,
	MPEG_PICTURE	= 8,
	PCD_PICTURE		= 9,
	PCT_PICTURE		= 10,
	PCX_PICTURE		= 11,
	PGM_PICTURE		= 12,
	PICT_PICTURE	= 13,
	PNG_PICTURE		= 14,
	PPM_PICTURE		= 15,
	PSD_PICTURE		= 16,
	TGA_PICTURE		= 17,
	TIFF_PICTURE	= 18,
	WEBP_PICTURE	= 19,
	WEMF_PICTURE	= 20,
	AVIF_PICTURE	= 21,
	HEIC_PICTURE	= 22
};

/*
	IMAGEHEADERINFO
	struttura per l'header dell'immagine
*/
typedef struct _imageheaderinfo {
	IMAGE_TYPE		type;			// formato
	float			xres;			// risoluzione
	float			yres;			// risoluzione
	int				restype;		// tipo risoluzione
	int				compression;	// tiff
	int				quality;		// jpeg 0-100
	unsigned long	filesize;		// dimensione del file su disco
	unsigned long	width;			// larghezza immagine
	unsigned long	height;			// altezza immagine
	int				bpp;			// bits x pixel
	int				bppOriginal;	// bpp originali
	int				bppConverted;	// bpp della versione caricata in memoria, se convertita
	unsigned long	memused;		// la dimensione in memoria dell'immagine (decompressa), per immagini senza padding (buffer contiguo): width × height × (bpp/8), per immagini DIB/Windows (con padding): stride × height
	int				colorSpace;		// tipo spazio colore (usa l'enum COLOR_TYPE_STD, vedi sotto)
	int				colors;			/*  numero di colori nella palette:
										0	per immagini non-palette (JPEG, PNG RGB, BMP 24-bit, ecc.)
										2	per immagini 1-bit
										16	per immagini 4-bit palette
										256	per GIF 8-bit
										JPEG restituisce 0 in colors perche' NON usa palette ma uno spazio colore continuo (RGB, YCbCr, CMYK, grayscale)
										il campo colors e' significativo solo per: GIF (palette), PNG con colorType=3 (palette), BMP con bpp <= 8 (palette), TIFF con PHOTOMETRIC_PALETTE, PCX/TGA palette-based
									*/
} IMAGEHEADERINFO, *LPIMAGEHEADERINFO;
typedef const LPIMAGEHEADERINFO LPCIMAGEHEADERINFO;

/*
	COLOR_TYPE_STD
	descrizioni valori del campo <colors> della struttura IMAGE_HEADERINFO
	NON cambiare l'ordine, ne' la numerazione
*/
typedef enum {
    COLOR_BW = 0,		// bianco e nero
    COLOR_GRAYSCALE = 0,// scala di grigi
    COLOR_PALETTE = 1,	// palette colori
    COLOR_RGB = 2,		// RGB (24-bit)
    COLOR_RGBA = 3,		// RGB + Alpha (32-bit)
    COLOR_CMYK = 4,		// CMYK
    COLOR_YCBCR = 5,	// YCbCr (JPEG)
    COLOR_LAB = 6,		// CIE L*a*b*
    COLOR_ALPHA = 7,	// solo alpha/mask
    COLOR_UNKNOWN = 8
} COLOR_TYPE_STD;

/*
	IMAGE_FILETYPE
	elemento per la lista dei formati supportati (tipo, estensione, descrizione)
*/
struct IMAGE_FILETYPE {
public:
	IMAGE_FILETYPE()
	{
		Reset();
	}

	IMAGE_FILETYPE(IMAGE_TYPE type,const char* ext,const char* desc,DWORD flags = 0L)
	{
		this->type = type;
		strcpyn(this->ext,ext,_MAX_EXT+1);
		strcpyn(this->desc,desc,_MAX_PATH+1);
		this->flags = flags;
	}
	
	virtual ~IMAGE_FILETYPE() {}

	void Reset(void)
	{
		type = NULL_PICTURE;
		memset(ext,'\0',_MAX_EXT+1);
		memset(desc,'\0',_MAX_PATH+1);
		flags = 0L;
	}

	IMAGE_TYPE	type;
	char		ext[_MAX_EXT+1];
	char		desc[_MAX_PATH+1];
	DWORD		flags;
};
typedef IMAGE_FILETYPE IMAGETYPE,*LPIMAGETYPE;

/*
	CImageTypeList
	classe per la lista dei formati supportati
*/
class CImageTypeList : public CNodeList
{
public:
	CImageTypeList() : CNodeList() {}
	virtual ~CImageTypeList() {CNodeList::DeleteAll();}
	
	void* Create(void)
	{
		IMAGETYPE* pData = new IMAGETYPE;
		return(pData);
	}
	
	void* Initialize(void* pVoid)
	{
		IMAGETYPE* pData = (IMAGETYPE*)pVoid;
		if(!pData)
			pData = (IMAGETYPE*)Create();
		if(pData)
			pData->Reset();
		return(pData);
	}

	BOOL PreDelete(ITERATOR iter)
	{
		if((IMAGETYPE*)iter->data)
		{
			delete ((IMAGETYPE*)iter->data);
			iter->data = (IMAGETYPE*)NULL;
		}
		return(TRUE);
	}

	int Size(void) {return(-1);}

#ifdef _DEBUG
	const char* Signature(void) {return("CImageTypeList");}
#endif
};

#define ADDFILETYPE(type,ext,desc,flags,ptr) {ptr = new IMAGETYPE(type,ext,desc,flags); if(ptr) m_ImageTypeList.Add(ptr);}

/*
	FILTER_FUNCTION

	polimorfismo a gógó: definisce il tipo per il puntatore alla funzione che eseguira' il codice per il filtro
	lo definisce di tipo CImageObject perche' questa e' la classe da cui deriva tutto il resto, poi a run-time, 
	verra' selezionato (poliformicamente) il membro della classe derivata che implementa effettivamente l'oggetto
*/
class CImageObject;
typedef UINT (CImageObject::*filter_t)(void);
typedef filter_t FILTER_FUNCTION;

#define FILTER_NAME_LEN		64
#define FILTER_PARAMS_LEN	64

/*
	FLTR_TYPE_...

	Bitmask per i filtri, indica gli effetti del filtro sull'immagine.
*/
#define FLTR_TYPE_COLOR			0x0001  // modifica RGB - Layout-safe
#define FLTR_TYPE_GEOM_ISO		0x0002  // cambia le posizioni ma mantiene Dimensioni (es. Flip, Mirror, Rot 180) - Layout-safe
#define FLTR_TYPE_GEOM_RESIZE	0x0004  // cambia le dimensioni ma mantiene l'Aspect Ratio - Semi-safe
#define FLTR_TYPE_GEOM_RATIO	0x0008	// cambia l'Aspect Ratio (es. Rotate 90) - Layout-killer
#define FLTR_TYPE_STRUCT		0x0010  // cambia la struttura interna (es. Blur, Sharpen) - Layout-safe
#define FLTR_TYPE_DESTRUCTIVE	0x0020  // perde informazioni (es. Crop, Posterize, Pixelate) - Layout-safe
#define FLTR_TYPE_PERF_LOW		0x0040  // implica un sovraccarico nel calcolo - Layout-safe
#define FLTR_TYPE_BLACKWHITE	0x0080  // conversione B&W/Grayscale che modifica RGB - Layout-safe

/*
	FILTER_PARAMS

	Tenere presente che in C++ la gestione del puntatore a funzione e' differente per via della v-table.
	Il puntatore, in tal caso, non e' semplicemente un indirizzo di memoria ma una mappatura sui membri
	della classe, quindi non puo' essere usato NULL per i confronti, ma nullptr, e per effettuare la 
	chiamata della funzione via puntatore, la sintassi deve essere: "(<ptr>->*fn)();", dove <ptr> e' il
	puntatore all'oggetto istanziato.
*/
typedef struct _filter_params {
	char	function[FILTER_NAME_LEN+1];// nome della funzione per il filtro (case sensitive)
	double	min;						// valore minimo per il parametro (per filtri con un solo parametro)
	double	max;						// valore massimo per il parametro                "
	double	value;						// valore di default del parametro                "
	char	minmax[FILTER_PARAMS_LEN+1];// stringa contenente il range dei parametri, x filtri con piu' di un parametro
	char	values[FILTER_PARAMS_LEN+1];// stringa contenente i parametri x filtri con piu' di un parametro (i controlli sul range vanno fatti nel filtro e non esiste valore di default)
	BOOL	native;						// se il filtro e' originario della libreria o si esegue via fallback nella classe zia (CImageObject->madre, CImage->zia :)
	FILTER_FUNCTION filter;				// puntatore alla funzione per il filtro
	BYTE	filterbitmask;				// bitmask per i flag indicanti l'effetto del filtro sull'immagine
} FILTER_PARAMS;				

// per filtro COlorSwap()
#define COLORSWAP_IDENTITY    0  // RGB -> RGB
#define COLORSWAP_RB          1  // RGB -> BGR
#define COLORSWAP_ROTATE_FWD  2  // RGB -> GBR
#define COLORSWAP_GB          3  // RGB -> RBG
#define COLORSWAP_ROTATE_REV  4  // RGB -> BRG
#define COLORSWAP_RG          5  // RGB -> GRB
#define COLORSWAP_RANDOM      6  // permutazione random

/*
	CImageObject
*/
class CImageObject
{
public:
	CImageObject();
	virtual ~CImageObject() {}

	BOOL					SetFilterParams			(LPCSTR lpcszFilterName,double nMin,double nMax,double nDefault,const char* szRanges,const char* szValues,TERN tNative,FILTER_FUNCTION fn);
	BOOL					SetFilterParams			(LPCSTR lpcszFilterName,double nValue,const char* szValues = NULL);
	BOOL					GetFilterParams			(LPCSTR lpcszFilterName,double& nValue,char* szValues = NULL,size_t nValuesSize = 0);
	BOOL					GetFilterParams			(LPCSTR lpcszFilterName,double& nMin,double& nMax,double& nDefault,char* szMinMax,size_t nMinMaxSize,char* szDefault,size_t nDefaultSize,BOOL& bNative);
	FILTER_FUNCTION			GetFilterFunction		(LPCSTR lpcszFilterName,BYTE& bitmask);
	BOOL					IsFilterImplemented		(LPCSTR lpcszFilterName);

	static double			NormalizeParamValue		(double baseMin,double baseMax,double val,double filterMin,double filterMax);

	static BOOL				IsGraphicsFormat		(LPCSTR lpcszFileName); // formato grafico valido, non necessariamente supportato
	BOOL					IsSupportedFormat		(LPCSTR lpcszFileName); // formato grafico supportato 
	int						CountSupportedFormats	(void);					// totale formati grafici supportati
	LPIMAGETYPE				EnumSupportedFormats	(int& nIterator);		// enumerazione formati grafici supportati

	// metodi
	virtual LPCSTR			GetLibraryName			(void) = 0;
	virtual LPCSTR			GetFullPathFileName		(void) = 0;
	virtual LPCSTR			GetFileName				(void) = 0;
	virtual DWORD			GetFileSize				(void) = 0;

	virtual int				GetWidth				(void) = 0;
	virtual int				GetHeight				(void) = 0;
	
	virtual float			GetXRes					(void) = 0;
	virtual float			GetYRes					(void) = 0;
	virtual void			SetXRes					(float) = 0;
	virtual void			SetYRes					(float) = 0;
	virtual int				GetURes					(void) = 0;
	virtual void			SetURes					(UINT nRes) = 0;
	virtual void			GetDPI					(float& nXRes,float& nYRes) = 0;

	virtual int				GetCompression			(void) = 0;
	virtual void			SetCompression			(int) = 0;
	virtual int				GetQuality				(void) = 0;
	virtual void			SetQuality				(int) = 0;
	virtual BOOL			GetQualityRange			(int& nMin,int& nMax) = 0;

	virtual int				GetAlignment			(void) = 0;
	virtual UINT			GetBytesWidth			(UINT nWidth,UINT nBitsPerPixel,UINT nAlig) = 0;
	virtual UINT			GetBytesWidth			(void) = 0;
	virtual PHOTOMETRIC		GetPhotometric			(void) = 0;

	virtual int				GetMaxPaletteColors		(void) = 0;
	virtual UINT			GetNumColors			(void) = 0;
	virtual BOOL			CountBWColors			(unsigned int* pColors,unsigned char nNumColors) = 0;
	virtual BOOL			CountRGBColors			(COLORREF* pColors,unsigned int* pCountColors,unsigned char nNumColors) = 0;

	virtual UINT			GetBPP					(void) = 0;
	virtual UINT			GetOriginalBPP			(void) = 0;
	virtual UINT			ConvertToBPP			(UINT nBitsPerPixel,UINT nFlags = 0,RGBQUAD* pPalette = NULL,UINT nColors = 0) = 0;
	
	virtual COLORREF		GetPixel				(int x,int y) = 0;
	virtual void			SetPixel				(int x,int y,COLORREF colorref) = 0;
	virtual void*			GetPixels				(void) = 0;
	virtual LPBITMAPINFO	GetBMI					(void) = 0;
	virtual UINT			GetMemUsed				(void) = 0;
	
	virtual BOOL			IsRGBOrder				(void) = 0;

	virtual int				GetDIBOrder				(void) = 0;
	virtual WORD			GetDIBNumColors			(LPSTR lpbi) = 0;
	virtual HPALETTE		CreateDIBPalette		(LPBITMAPINFO lpbmi) = 0;
	virtual BOOL			SetPalette				(UINT nIndex,UINT nColors,RGBQUAD* pPalette) = 0;
	virtual HDIB			GetDIB					(UINT* pSize = NULL) = 0;
	virtual BOOL			SetDIB					(HDIB hDib,int nOrientation = 1) = 0;

	virtual IMAGE_TYPE		GetType					(void) = 0;
	virtual DWORD			UpdateHeaderInfo		(void) = 0;

	virtual BOOL			Create					(BITMAPINFO* pBitmapInfo,void* pData = NULL) = 0;
	virtual BOOL			Load					(LPCSTR lpcszFileName,DWORD& dwError) = 0;
	virtual BOOL			LoadFromUrl				(LPCSTR lpcszUrl) = 0;
	virtual BOOL			Unload					(void) = 0;
	virtual BOOL			IsLoaded				(void) = 0;
	virtual BOOL			Save					(LPCSTR lpcszFileName,LPCSTR lpcszFormat,DWORD dwFlags = 0L) = 0;
	virtual BOOL			Draw					(HDC hDC, int x, int y) = 0;
	virtual HANDLE			Copy					(RECT& rect) = 0;
	virtual BOOL			Paste					(int x,int y,HANDLE hDib) = 0;
	virtual CImageObject*	Crop					(int x,int y,int width,int height) = 0;
	virtual UINT			Stretch					(RECT& rcSize,BOOL bAspectRatio = TRUE) = 0;
	virtual CImageObject*	Rescale					(int nWidth,int nHeight) = 0;

	virtual BOOL			LockData				(void) = 0;
	virtual BOOL			UnlockData				(BOOL bModified = FALSE) = 0;

	virtual void			SetLastErrorDescription	(LPCSTR lpcszError) {if(lpcszError && *lpcszError) strcpyn(m_szError,lpcszError,sizeof(m_szError));}
	virtual LPCSTR			GetLastErrorDescription	(void) {return(m_szError);}

	// filtri
	virtual UINT			Blur					(void) = 0;
	virtual UINT			Brightness				(void) = 0;
	virtual UINT			ColorShift				(void) = 0;
	virtual UINT			ColorSwap				(void) = 0;
	virtual UINT			Contrast				(void) = 0;
	virtual UINT			Echo					(void) = 0;
	virtual UINT			EdgeEnhance				(void) = 0;
	virtual UINT			Emboss					(void) = 0;
	virtual UINT			Equalize				(void) = 0;
	virtual UINT			FindEdge				(void) = 0;
	virtual UINT			GammaCorrection			(void) = 0;
	virtual UINT			GhostTrail				(void) = 0;
	virtual UINT			Grain					(void) = 0;
	virtual UINT			Grayscale				(void) = 0;
	virtual UINT			HalftoneBW				(void) = 0;
	virtual UINT			HalftoneColor			(void) = 0;
	virtual UINT			Hue						(void) = 0;
	virtual UINT			Intensity				(void) = 0;
	virtual UINT			JitterHorizontal		(void) = 0;
	virtual UINT			JitterSinusoidal		(void) = 0;
	virtual UINT			Median					(void) = 0;
	virtual UINT			MirrorHorizontal		(void) = 0;
	virtual UINT			MirrorVertical			(void) = 0;
	virtual UINT			Negate					(void) = 0;
	virtual UINT			Noise					(void) = 0;
	virtual UINT			Pixelate				(void) = 0;
	virtual UINT			PixelSort				(void) = 0;
	virtual UINT			Posterize				(void) = 0;
	virtual UINT			Rotate90Left			(void) = 0;
	virtual UINT			Rotate90Right			(void) = 0;
	virtual UINT			Rotate180				(void) = 0;
	virtual UINT			Saturation				(void) = 0;
	virtual UINT			Sharpen					(void) = 0;
	virtual UINT			Test					(void) = 0;

protected:
	char					m_szError[1024];			// descrizione errore
	char					m_szFileName[_MAX_PATH+1];	// nome file immagine completo di pathname
	char					m_szFormat[_MAX_EXT+1];		// formato immagine
	IMAGEHEADERINFO			m_InfoHeader;				// header immagine
	CImageTypeList			m_ImageTypeList;			// lista per i formati riconosciuti (deve essere caricata dalla derivata)
	FILTER_PARAMS			m_filterParams[34];			// array per con i valori relativi alle funzioni per i filtri
};

#endif // _CIMAGEOBJECT_H
