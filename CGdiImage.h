/*
	CGdiImage.h
	Classe basica per gestione files grafici via GDI+.
	Luca Piergentili, 16/07/2026

	Vedi le note in CGdiImage.cpp.
*/
#ifndef _GDI_IMAGE_H
#define _GDI_IMAGE_H 1

#include <windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

/*
	GdiImageHandler()
*/
class GdiImageHandler
{
public:
	static ULONG_PTR	Startup				(void);
	static void			Shutdown			(ULONG_PTR token);

public:
	GdiImageHandler();
	~GdiImageHandler();

    // proibisce copia ed assegnazione per evitare memory corruption
    GdiImageHandler(const GdiImageHandler&) = delete;
    GdiImageHandler& operator=(const GdiImageHandler&) = delete;

	void				Free				(void);
	bool				Load				(const wchar_t* pwszFilePath);

	UINT				GetWidth			(void) const {return(m_nWidth);}
	UINT				GetHeight			(void) const {return(m_nHeight);}

	bool				Resize				(UINT newWidth,UINT newHeight);
	bool				Save				(void);
	bool				SaveAs				(const wchar_t* pwszFilePath,const wchar_t* pwszMimeType);

private:
	int					GetEncoderClsid		(const wchar_t* format, CLSID* pClsid);
	const wchar_t*		GetMimeTypeFromPath	(const wchar_t* pwszPath);

	Gdiplus::Bitmap*	m_pBitmap;
	wchar_t*			m_pwszCurrentPath;
	UINT				m_nWidth;
	UINT				m_nHeight;
};

#endif // _GDI_IMAGE_H
