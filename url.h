/*$
	url.h
	Tutto quanto relativo alle url.
	Luca Piergentili, 16/07/25
*/
#ifndef _URL_H
#define _URL_H 1

#include <stdlib.h>
#ifndef _MAX_FNAME
  #define _MAX_FNAME 256
#endif
#ifndef _MAX_PATH
  #define _MAX_PATH _MAX_FNAME
#endif
#ifndef _MAX_FILEPATH
  #define _MAX_FILEPATH (_MAX_DIR+_MAX_FNAME)
#endif
#include "ipaddress.h"

/* interfaccia C++ */
#ifdef __cplusplus
  extern "C" {
#endif

#ifndef MAX_PATH_SEGMENTS
  #define MAX_PATH_SEGMENTS 512
#endif

#define MAX_URL_AUTHORITY_LEN	512
#define MAX_URL_FRAGMENT_LEN	256
#define	MAX_URL_USERNAME		64
#define	MAX_URL_PASSWORD		64
#define MAX_URL_SCHEME			32				/* protocollo (include ://) */
#define MAX_URL_HOST			HOSTNAME_LEN	/* host */
#define MAX_URL_PORT_VALUE		65535
#define MAX_URL_PORT_LENGTH		5				/* porta */
#define MAX_URL_PATH			_MAX_FILEPATH	/* path (include lunghezza file) */
#define MAX_URL_FILE			_MAX_FNAME		/* file */
#define MAX_URL_EXT				_MAX_EXT		/* estensione */
#define MAX_URL_QUERY			1024
#define MAX_URL_LENGTH          (MAX_URL_SCHEME + 3 \
                                + MAX_URL_HOST \
                                + MAX_URL_PORT_LENGTH + 1 \
                                + MAX_URL_PATH + 1 \
                                + MAX_URL_QUERY + 1 \
                                + MAX_URL_FRAGMENT_LEN + 1 \
                                + 1)

/*
	URLDATA
	
	inizializzare a zero prima di usare
	la url da scomporre va copiata nel campo <url> prima di chiamare 
	la funzione url_parse(), che la legge da questo campo
*/
typedef struct _urldata {
	char url[MAX_URL_LENGTH+1];
	char proto[MAX_URL_SCHEME+1];
	char host[MAX_URL_HOST+1];
	char port[MAX_URL_PORT_LENGTH+1];
	char path[MAX_URL_PATH+1];
	char file[MAX_URL_FILE+1];				/* nome file completo di estensione */
	char name[MAX_URL_FILE+1];				/* solo nome file (senza estensione) */
	char ext[MAX_URL_EXT+1];				/* solo estensione */
	char query[MAX_URL_QUERY+1];
    char fragment[MAX_URL_FRAGMENT_LEN+1];
	char username[MAX_URL_USERNAME+1];
    char password[MAX_URL_PASSWORD+1];
} URLDATA;

/*
	URLPROBABILITY
*/
typedef struct _urlprobability {
    int probability;
} URLPROBABILITY;

/*
	FILEURLPROBABILITY
*/
typedef struct _fileurlprobability {
    int file;
    int url;
} FILEURLPROBABILITY;

char*				normalize_unix_path		(char* path,int len);

URLPROBABILITY		get_url_probability		(const char* str);
FILEURLPROBABILITY	get_file_url_probability(const char* str);

/* interfaccia C++ */
#ifdef __cplusplus
  }
#endif

#endif /* _URL_H */
