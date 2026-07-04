/*$
	CContentDecoder.h
	Classe per la decodifica dei files compressi ricevuti dal server HTTP in seguito 
	all'invio dell'header Accept-encoding.
	Supporta tre formati base: gzip, deflate e br; gzip e deflate vengono gestiti qui, 
	br con il codice della DLL relativa: https://github.com/lpierge/BrotliDLL
	Luca Piergentili, Settembre '25

	Vedi le note in CContentDecoder.cpp.
*/
#ifndef _CCONTENTDECODER_H
#define _CCONTENTDECODER_H 1

#include "window.h"

/*
	ENCODEDFORMAT
*/
typedef enum _encodedformat {
	DEFLATE_ENCODED_FORMAT,
	GZIP_ENCODED_FORMAT,
	BROTLI_ENCODED_FORMAT,
	UNKNOWN_ENCODED_FORMAT
} ENCODEDFORMAT;

/*
	CContentDecoder
*/
class CContentDecoder
{
public:
	CContentDecoder() {};
	virtual ~CContentDecoder() {};

	BOOL DecodeFile(LPCSTR lpcszFilename,LPCSTR lpcszContentType,ENCODEDFORMAT& fmt);
};

#endif // _CCONTENTDECODER_H
