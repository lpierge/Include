/*$
	win32inet.h
	Generiche per Internet.
	Luca Piergentili, 02/07/25

	Vedi le note in win32inet.c
*/
#ifndef _WIN32INET_H
#define _WIN32INET_H 1

#include "window.h"
#include "inet.h"

/* interfaccia C++ */
#ifdef __cplusplus
  extern "C" {
#endif

/*
	HTTP_PROTOCOL
*/
typedef enum _http_protocol {
	HTTP_PROTOCOL_UNKNOWN,
	HTTP_PROTOCOL_NON_SECURE,
	HTTP_PROTOCOL_SECURE,
} HTTP_PROTOCOL;

HTTP_PROTOCOL	WhichHttpProtocol			(LPCSTR lpcszUrl);

LPSTR			GetHostFromUrl				(LPCSTR pszUrl);
LPSTR			GetProtoFromUrl				(LPCSTR pszUrl);
LPSTR			GetBaseDomainFromHost		(LPCSTR pszHost);
LPSTR			GetPathFromUrl				(LPCSTR pszUrl);

void			SplitContentType			(LPCSTR lpcszContent,LPSTR lpszContentType,UINT nContentType,LPSTR lpszContentSubType,UINT nContentSubType);

MIMEGENRE		GetMimeGenreFromExtension	(LPCSTR lpcszFilename);
MIMEGENRE		GetMimeGenreFromContentType	(LPCSTR lpcszContentType);
MIMEFAMILY		GetMimeFamilyFromExtension	(LPCSTR lpcszString);
MIMEFAMILY		GetMimeFamilyFromContentType(LPCSTR lpcszContentType);

MIMETYPE*		GetMimeTypeFromMimeFamily	(MIMEFAMILY mimefamily,int* nIterator);

LPCSTR			GetContentTypeFromMimeFamily(MIMEFAMILY mimefamily,int* nIterator);
LPCSTR			GetContentTypeFromExtension	(LPCSTR lpcszFilename);
LPCSTR			GetExtensionFromContentType	(LPCSTR pszContentType);

LPCSTR			WSAGetLastErrorString		(int nError);

BOOL			GetLastInetResponse			(LPSTR pBuffer,UINT nSize,DWORD* pdwError);
BOOL			GetInetErrorString			(DWORD dwError,LPSTR lpBuffer,DWORD dwSize);

/*
	HTTPSTATUS
*/
typedef struct _http_status {
	int			code;
	const char*	reason;
	const char*	description;
} HTTPSTATUS;

const HTTPSTATUS* GetHttpErrorStatus		(DWORD dwError);

const char*		GetHttpErrorText			(int code);
const char*		GetHttpErrorDescription		(int code);

LPSTR			MSDosPathToHttpUrl			(LPCSTR lpcszMSDosPath);
LPSTR			MSDosPercentEncodeUnallowed	(LPCSTR lpcszFilename,LPSTR lpBuffer,int nSize);

/* interfaccia C++ */
#ifdef __cplusplus
  }
#endif

#endif /* _WIN32INET_H */
