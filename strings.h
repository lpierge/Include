/*$
	strings.h
	Operazioni sulle stringhe.
	Luca Piergentili, 13/09/98
	lpiergentili@yahoo.com
*/
#ifndef _STRINGS_H
#define _STRINGS_H 1

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include "typedef.h"
#include "typeval.h"

/* interfaccia C++ */
#ifdef __cplusplus
  extern "C" {
#endif

/*
riguardo le funzioni per eliminare gli spazi, tenere sempre presente che la funzione 
isspace() non considera come spazio solo il carattere ' ', ma tutti i seguenti:
spazio ' '
tabulazione orizzontale '\t'
newline '\n'
ritorno a capo '\r'
form feed '\f'
tabulazione verticale '\v'
macro.h contiene la versione inline ddella macro ISSPACE() -> IS_SPACE()
*/
#define			ISSPACE(c)			((c==' ') || (c=='\t') || (c=='\v') || (c=='\f') || (c=='\r') || (c=='\n'))
#define			ISALPHA(c)			(c >= 0 && c <= 255)
#define			ISSEP(c)			((c=='.') || (c==',') || (c==';') || (c==':') || (c=='-'))

void			memnxor				(char* buffer,char* psw,size_t len);
void			memnxor_salt		(char* buffer,const char* psw,size_t len,const char* salt,size_t salt_len);

inline void*	bzero				(void* buf,size_t size) {return(memset(buf,'\0',size));}
inline void		strzero				(char *str,size_t size) {memset((void*)str,'\0',size);}

char			strfirst			(const char* str,const char* chars);
char*			strshift			(char* str,const char* substring);

char*			strsep				(char **str,const char *delim);

int				strchkc				(const char* str,const char* chrs);
int				strchks				(const char* str,const char** ptrarray,size_t arraysize);

char*			strichr				(const char* str,int c);
char*			stristr				(const char* str,const char* substring);
const char*		strrstr				(const char* str,const char* substring);
char*			strextac			(const char* str, const char* exact);

char*			strsetn				(char* str,const char c,size_t size);

char*			strcpyn				(char* dst,const char *src,size_t size);
char*			strcpync			(char* dst,const char* src,size_t size,char delimiter);

/* come strcpyn(), pero' copia solo se dest e' vuoto -> {0} */
#define strcpyne(dest,src,size) \
do { \
    if((dest)[0]=='\0') { \
        strcpyn((dest),(src),(size)); \
    } \
} while(0)

char*			strcpyleft			(char* dst,const char* src,size_t size);
char*			strcpyright			(char* dst,const char* src,size_t size);

char*			strcatn				(char* dst,const char *src,size_t size);

int				strcmpni			(const char* s1,const char* s2,size_t n);
	
const char*		strchrn				(const char* str, int c, size_t n);
const char*		strstrn				(const char* haystack, const char* needle, size_t n);

int				strchgc				(char* str,char c,char chr);
int				strchgs				(char* str,const char* s,char chr);

char*			subst				(const char* str,const char* substring,const char* replace);
int				substr				(const char* str,const char* substring,const char* replace,char* buffer,size_t size);
char*			substrn				(const char* string,size_t pos,size_t len,const char* replace);

char*			strtokargs			(char* str,const char separator,const char inclusor);

void			strstrpc			(char *str,char chr);

char*			strshuffle			(char *string);
char*			strwords			(char *buffer,size_t size);

bool			strinbtw			(const char* string,const char* substring,const char* from,const char* to);

void			strrcspn			(char* str, const char* chars_to_remove);

inline bool		strnull				(const char* str) {return(str ? (str[0]=='\0' ? true : (bool)(strcmp(str,"")==0)) : true);}
int				strempty			(const char* str);
size_t			strrtrim			(char* str);
size_t			strltrim			(char* str);

size_t			stralltrim			(char* str);
char* /*size_t*/stroutrim			(char* str);

size_t			strstrim			(char* str);

char*			strrot				(char* str,int n,int t);

int				strcount			(const char *str, const char *substr);

char*			strlrw				(char* str);
char*			strurp				(char* str);

int				strright			(const char* str,const char* substr);
int				striright			(const char* str,const char* substr);
int				strleft				(const char* str,const char* substr);
int				strileft			(const char* str,const char* substr);

long long		strkmgt				(const char* size);
char*			strsizefmt			(char* str,int size,double bytes);
char*			strnumfmt			(const TAGGEDVALUE *tv, char *buf, size_t bufSize);

char*			strdupl				(const char* str);

int				wtfsnprintf			(char *buf,size_t n,const char *fmt,...);

char*			ltos				(long,char*,size_t);
char*			ultos				(unsigned long,char*,size_t);
char*			dtos				(double,char*,size_t);
char*			ftos				(float,char*,size_t);
char*			qwtos				(QWORD,char*,size_t);

bool			is_all_ascii		(const char* str);
bool			has_ansi_ext		(const char* str);
void			replace_non_ansi	(char* str,char chr);
void			replace_non_ascii	(char* str,char chr);
void			delete_non_ansi		(char* str);
void			delete_non_ascii	(char* str);

/* interfaccia C++ */
#ifdef __cplusplus
  }
#endif

#endif /* _STRINGS_H */
