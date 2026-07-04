/*$
	urlextractor.h
	Estrae le url da un file, ricercando per protocollo/tag HTML/estensione file.
	Luca Piergentili, giugno 25

	La logica di base e' abbastanza semplice: invece di impazzire inplementando un parser che sia in grado di analizzare
	tutti i vari protocolli esistenti (HTML, Javascript, JSON, etc.), dato che quello che interessa e' estrarre la risorsa
	indipendentemente da dove e come viene referenziata, il codice cerca di andare a colpo sicuro cercando i due elementi
	che (quasi) sempre identificano una risorsa: il protocollo e l'estensione.

	Il protocollo puo' non essere presente nel caso di url relative, e l'estensione puo' essere omessa quando il server
	referenzia una risorsa tramite la Content-Type, per questo il chiamante deve effettuare un estrazione incrociata usando
	i tre metodi in sequenza.

	L'estrazione per tag HTML e' in realta' un ibrido che ricerca non per tag ma per attributo del tag, considerando HTML,
	CSS, Javascript e JSON, questi ultimi tre in forma minima.

	Delimitatori validi per la url:
	- spazi bianchi: (spazio), \t (tab), \n (nuova riga), \r (ritorno a capo)
	- virgolette/apici: " (doppio apice), ' (singolo apice) - perché spesso racchiudono l'URL in attributi HTML
	- (*)parentesi: (, ), [, ], {, } - se l'URL e' racchiuso tra parentesi in un testo
	- altri simboli di punteggiatura: spesso > (anche se pi'u raro), |, \ (backslash)
	(*) = { e } attulmente rimossi come delimitatori per url tipo quella sotto

	RFC 3986 (Uniform Resource Identifier (URI): lo standard che definisce la sintassi generale delle URI, di cui le URL 
	sono un sottotipo. Specifica quali caratteri sono "riservati" (hanno un significato speciale) e quali no.

	L'RFC 3986 classifica i seguenti come CARATTERI RISERVATI, dato che hanno un significato speciale come delimitatori o 
	indicatori di componenti all'interno di una URI. Se devono essere usati come dati all'interno di una URI (cioe', non 
	con il loro significato speciale), devono essere percent-encoded (es. uno spazio, che e' un "carattere non consentito" 
	deve essere %20).

	Delimitatori Generali (gen-delims): Usati per delimitare le componenti principali di una URI:
	: (colon)
	/ (slash)
	? (question mark)
	# (hash/fragment)
	[ (left square bracket)
	] (right square bracket)
	@ (at-sign)

	Delimitatori Secondari (sub-delims): Usati per delimitare i sottocomponenti o come parte di altre 
	grammatiche piu' specifiche:
	! (exclamation mark)
	$ (dollar sign)
	& (ampersand)
	' (single quote / apostrophe)
	( (left parenthesis)
	) (right parenthesis)
	* (asterisk)
	+ (plus sign)
	, (comma)
	; (semicolon)
	= (equals sign)

	In sintesi, l'insieme completo dei caratteri riservati e':
	! # $ & ' ( ) * + , / : ; = ? @ [ ]

	L'RFC 3986 classifica i seguenti come CARATTERI NON RISERVATI, ossia caratteri che non hanno alcun 
	significato speciale in un URI e possono essere sempre usati direttamente senza bisogno di essere 
	percent-encoded. L'insieme dei caratteri NON riservati include:
	lettere maiuscole: A a Z
	lettere minuscole: a a z
	cifre decimali: 0 a 9
	altri simboli specifici:
	- (hyphen-minus)
	. (period / full stop)
	_ (underscore)
	~ (tilde)
	In sintesi, l'insieme completo dei caratteri NON riservati e':
	A-Z a-z 0-9 - . _ ~

	CARATTERI RISERVATI MS-DOS:
	Nel contesto dei file system MS-DOS e Windows (NTFS e FAT), esistono caratteri specifici che sono riservati e non possono 
	essere utilizzati nei nomi di file o cartelle. Questi caratteri hanno un significato speciale per il sistema operativo o 
	sono usati come delimitatori:
	< (segno minore)
	> (segno maggiore)
	: (due punti)
	" (virgolette doppie)
	/ (barra, slash)
	\ (barra inversa, backslash)
	| (barra verticale, pipe)
	? (punto interrogativo)
	* (asterisco)
	Nomi riservati specifici (NON solo caratteri).
	Oltre ai singoli caratteri, ci sono anche alcuni nomi di file/cartelle specifici che sono riservati e non possono essere 
	utilizzati, indipendentemente dall'estensione. Questi erano importanti per MS-DOS e sono stati mantenuti per compatibilita' 
	in Windows:
	CON: console (input/output di sistema)
	PRN: dispositivo stampante
	AUX: dispositivo ausiliario (spesso seriale)
	NUL: dispositivo nullo (il "cestino" per l'output)
	COM1, COM2, COM3, COM4, COM5, COM6, COM7, COM8, COM9: porte seriali
	LPT1, LPT2, LPT3, LPT4, LPT5, LPT6, LPT7, LPT8, LPT9: porte parallele
 
	Percent-encoding: i caratteri "riservati" (come :, /, ?, #, [, ], @, !, $, &, ', (, ), *, +, ,, ;, =) ed i caratteri 
	"non consentiti" (come lo spazio o i caratteri non-ASCII) devono essere codificati usando il percent-encoding (o URL 
	encoding). Questo significa che vengono sostituiti da un simbolo di percentuale (%) seguito da due cifre esadecimali 
	che rappresentano il loro valore ASCII (o UTF-8).

	Esempio: uno spazio diventa %20, un asterisco * diventa %2A.
*/
#ifndef _URLEXTRACTOR_H
#define _URLEXTRACTOR_H 1

#include "url.h"

/* interfaccia C++ */
#ifdef __cplusplus
  extern "C" {
#endif

/*
	URRLIST
	lista (dinamica) per le url estratte
	chiamare SEMPRE url_list_initialize() SUBITO DOPO la dichiarazione, PRIMA di qualsiasi uso
*/
typedef struct _urllist {
    char**	urls;		/* array di puntatori a stringhe (url) */
    size_t	count;		/* numero di url attualmente memorizzate */
    size_t	capacity;	/* capacità massima attuale dell'array */
} URLLIST;

/* codici d'errore */
#define URLLIST_ERROR_SUCCESS		0
#define URLLIST_ERROR_MEMORYALLOC	1
#define URLLIST_ERROR_CREATEFILE	2
#define URLLIST_ERROR_FILESIZE		3
#define URLLIST_ERROR_CREATEFILEMAP	4
#define URLLIST_ERROR_MAPFILE		5
#define URLLIST_ERROR_ADDURL		6
#define URLLIST_ERROR_INVALID_PARAM 7

void	url_list_initialize	(URLLIST* list);
void	url_list_release	(URLLIST* list);
int		url_list_add		(URLLIST* list,const char* url);
void	url_list_sort		(URLLIST* list,int order);

int		url_extract_by_proto(URLLIST* list,const char* filename);
int		url_extract_by_html	(URLLIST* list,const char* filename);
int		url_extract_by_ext	(URLLIST* list,const char* filename,const char* exts);

/* interfaccia C++ */
#ifdef __cplusplus
  }
#endif

#endif /* _URLEXTRACTOR_H */
