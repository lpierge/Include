/*$
	inet.h
	Internet et varia.
	Luca Piergentili, Luglio '25
*/
#ifndef _INET_H
#define _INET_H 1

#include <stdbool.h>

/* interfaccia C++ */
#ifdef __cplusplus
  extern "C" {
#endif

#define ENUM_GRAPHICS_FORMAT	0
#define ENUM_SCRIPT_TYPE		1
#define ENUM_STYLE_SHEET		2
#define ENUM_AUDIO_FORMAT		3
#define ENUM_DATA_FORMAT		4
#define ENUM_DOCUMENTS			5
#define ENUM_COMPRESS_FORMAT	6

/*
	MIMEGENRE

	genere (testo/binario) per sapere se e' un file da cui poter estrarre links
*/
typedef enum _mimegenre {
	GENRE_TEXT = 0,
	GENRE_BINARY,
	GENRE_UNKNOWN = 9,
	GENRE_RESERVED
} MIMEGENRE;

/*
	MIMEFAMILY

	famiglia, per raggruppare per tipi (basicamente, la prima parte della content type)
*/
typedef enum _mimefamily {
    FAMILY_TEXT = 0,
    FAMILY_IMAGE,
    FAMILY_AUDIO,
    FAMILY_VIDEO,
    FAMILY_APPLICATION,
    FAMILY_FONT,
    FAMILY_HTML = 6,
    FAMILY_UNKNOWN = 9,
    FAMILY_RESERVED
} MIMEFAMILY;

#define CONTENT_TYPE_LEN	128		/* il valore della Content-Type: ("text/html", "image/jpeg", etc.) */
#define CONTENT_TYPE_EXT	32		/* l'estensione relativa alla Content-Type: (".html", ".jpeg", etc.) */
#define CONTENT_TYPE_DESC	256		/* descrizione estesa */

/*
	MIMETYPE

	struttura per l'array per le Content-Type:
*/
typedef struct _mimetype {
	MIMEGENRE	genre;								/* tipo: binario/testo */
	MIMEFAMILY	family;								/* famiglia: testo, immagine, applicazione, etc. */
	int			age;								/* eta' della content type (0 x browsers preistorici e antichi, 1 x moderni) */
	char		type[CONTENT_TYPE_LEN+1];			/* tipo mime (content type) */
	char		ext[CONTENT_TYPE_EXT+1];			/* estensione file relativo */
	char		description[CONTENT_TYPE_DESC+1];	/* descrizione in umano... */
} MIMETYPE;

/*
	WEBCONTENTFAMILYTYPE
*/
typedef struct _webcontentfamilytype {
	const char* family;								/* famiglia: formati grafici, scripts, formati audio, documenti, etc. */
	char* type;										/* elemento/i della famiglia: tipo di file relativo (es. .jpg[...]) */
} WEBCONTENTFAMILYTYPE;

const char*		inet_enum_internet_protocols	(int* iterator);
const MIMETYPE*	inet_enum_mime_types			(int* iterator);
void			inet_load_mime_types			(const MIMETYPE* mt);
const char*		inet_enum_graphics_formats		(int* iterator);
const char*		inet_enum_web_content_formats	(int* iterator,int type);

bool			inet_is_graphics_format			(const char* filename);
bool			inet_is_script_format			(const char* filename);
bool			inet_is_style_format			(const char* filename);
bool			inet_is_font_format				(const char* filename);
bool			inet_is_audio_format			(const char* filename);
bool			inet_is_data_format				(const char* filename);
bool			inet_is_doc_format				(const char* filename);
bool			inet_is_compress_format			(const char* filename);

/* interfaccia C++ */
#ifdef __cplusplus
  }
#endif

#endif /* _INET_H */
