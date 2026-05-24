/*$
	CzLib.h
	Classe base per interfaccia con la libreria zLib di Jean-loup Gailly e Mark Adler (http://www.gzip.org/zlib/) (CRT/SDK/MFC).
	Luca Piergentili, 20/12/98
	lpiergentili@yahoo.com

	A seconda della definizione o meno delle macro, viene usato per la DLL, la LIB o per il link diretto del codice.
*/
#ifndef _CZLIB_H
#define _CZLIB_H 1

#include <stdio.h>
#include <stdlib.h>
#ifdef _WINDOWS
  #include "window.h"
#endif
#include "typedef.h"
#include "macro.h"
#include "zlibapi.h"	// definizione interfaccia (ZLIBAPI)
#include "zlib.h"		// interfaccia zLib

// crea la referenza:
// _ZLIB_DLL per usare la dll
// _ZLIB_LIB per usare la libreria
// <nessuna definizione> per il link diretto con il codice
#if defined(_ZLIB_DLL)
	//
	//	_ZLIB_DLL
	//
#ifdef PRAGMA_MESSAGE_VERBOSE
	#pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): zLib support through dll")
#endif
	#ifdef _DEBUG
		#pragma comment(lib,"zLibdll.d.lib")
#ifdef PRAGMA_MESSAGE_VERBOSE
		#pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): automatically linking with zLibdll.d.dll")
#endif
	#else
		#pragma comment(lib,"zLibdll.lib")
#ifdef PRAGMA_MESSAGE_VERBOSE
		#pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): automatically linking with zLibdll.dll")
#endif
	#endif
#elif defined(_ZLIB_LIB)
	//
	//	_ZLIB_LIB
	//
#ifdef PRAGMA_MESSAGE_VERBOSE
	#pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): zLib support through lib")
#endif
	#ifdef _DEBUG
		#pragma comment(lib,"zLiblib.d.lib")
#ifdef PRAGMA_MESSAGE_VERBOSE
		#pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): automatically linking with zLibdll.d.lib")
#endif
	#else
		#pragma comment(lib,"zLiblib.lib")
#ifdef PRAGMA_MESSAGE_VERBOSE
		#pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): automatically linking with zLibdll.d.lib")
#endif
	#endif
#else
	//
	//	link con il codice
	//
	#pragma message(	"\t\t\t"\
						__FILE__\
						"("\
						STR(__LINE__)\
						"): warning: zLib support undefined (are you linking directly with the zLib code?)")
#endif

#define ZIP_EXTENSION	".zip"
#define ZIP_TYPEDESC	"compressed data"

/*
	CzLib
*/
class CzLib
{
public:
	CzLib() {}
	virtual ~CzLib() {}
	
	// se usata direttamente, esporta solo le due funzioni basiche
	inline BOOL Compress	(const char* filein,const char* fileout) {return(gzcompressfile(filein,fileout)==0);}
	inline BOOL UnCompress	(const char* filein,const char* fileout) {return(gzuncompressfile(filein,fileout)==0);}

protected:
	// se usata come classe derivata, esporta l'interfaccia della zLib
	inline int			gzsetcallback	(LPGZPROGRESSCALLBACK a,void* b,int c)	{return(::gzsetcallback(a,b,c));}
	inline int			gzcompressfile	(const char* a,const char* b)			{return(::gzcompressfile(a,b));}
	inline int			gzuncompressfile(const char* a,const char* b)			{return(::gzuncompressfile(a,b));}
	inline int			gzcompress		(FILE* a,gzFile b,QWORD c)				{return(::gzcompress(a,b,c));}
	inline int			gzuncompress	(gzFile a,FILE* b, QWORD c)				{return(::gzuncompress(a,b,c));}
	inline gzFile		gzopen			(const char* a,const char* b,GZWHDR* c)	{return(::gzopen(a,b,c));}
	inline gzFile		gzdopen			(int a,const char* b)					{return(::gzdopen(a,b));}
	inline int			gzread			(gzFile a,voidp b,unsigned int c)		{return(::gzread(a,b,c));}
	inline int			gzwrite			(gzFile a,const voidp b,unsigned int c)	{return(::gzwrite(a,b,c));}
	inline int			gzflush			(gzFile a,int b)						{return(::gzflush(a,b));}
	inline int			gzclose			(gzFile a)								{return(::gzclose(a));}
	inline const char*	gzerror			(gzFile a,int* b)						{return(::gzerror(a,b));}
	inline const char*	version			(void)									{return(::zlibVersion());}
};

/*
la classe (CzLib) usa la versione "custom" (con le modifiche per l'header GZW) della zLib originaria (versione 1.1.3), mentre
il codice d'esempio che segue (ripreso da Internet per formati zlib e gzip) necessita la ultima versione della libreria (1.3.1)
*/
#if 0

#define CHUNK 16384

void zLibCompressFile(FILE *source, FILE *dest)
{
    int ret, flush;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    // Initialize the zlib stream for compression
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
    if (ret != Z_OK) return;

    // Compress until end of file
    do {
        strm.avail_in = fread(in, 1, CHUNK, source);
        if (ferror(source)) {
            (void)deflateEnd(&strm);
            return;
        }
        flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;

        // Run deflate() on input until output buffer not full
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = deflate(&strm, flush);
            assert(ret != Z_STREAM_ERROR);
            have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                (void)deflateEnd(&strm);
                return;
            }
        } while (strm.avail_out == 0);
        assert(strm.avail_in == 0);
    } while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);

    // Clean up
    (void)deflateEnd(&strm);
}

void zLibUncompressFile(FILE *source, FILE *dest)
{
    int ret;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    // Initialize the zlib stream for decompression
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK) return;

    // Decompress until deflate stream ends or end of file
    do {
        strm.avail_in = fread(in, 1, CHUNK, source);
        if (ferror(source)) {
            (void)inflateEnd(&strm);
            return;
        }
        if (strm.avail_in == 0) break;
        strm.next_in = in;

        // Run inflate() on input until output buffer not full
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);
            switch (ret) {
                case Z_NEED_DICT:
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                    (void)inflateEnd(&strm);
                    return;
            }
            have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                (void)inflateEnd(&strm);
                return;
            }
        } while (strm.avail_out == 0);

        // Done when inflate() says it's done
    } while (ret != Z_STREAM_END);

    // Clean up
    (void)inflateEnd(&strm);
}
#endif

#endif // _CZLIB_H
