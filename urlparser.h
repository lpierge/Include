/*$
	urlparser.h
	Analisi e spezzettamento URL (che si fa presto a dire, ma tra il dire e il fare c'e' di mezzo il mare).
	Luca Piergentili, 12/07/25
*/
#ifndef _URLPARSER_H
#define _URLPARSER_H 1

#include "url.h"

/* interfaccia C++ */
#ifdef __cplusplus
  extern "C" {
#endif

bool	url_parse						(URLDATA* pUrlData);
char*	url_combine						(URLDATA* pParent,URLDATA* pUrl,char* pCombinedUrl,size_t nUrlsize/* che sia minimo = MAX_URL_LENGTH */);
char*	url_refill						(char* url,size_t size,URLDATA* u);

void	url_remove_params				(const char* szSourceUrl,const char* szParamsToRemove,char* szDestBuffer,size_t nDestSize);
void	url_subst_param					(const char* szSourceUrl,const char* szParamToSubst,const char* szNewValue,char* szDestBuffer,size_t nDestSize);
void	url_get_param					(const char* szSourceUrl,const char* szParams,char* szValue,size_t nValue);
bool	url_add_param_string			(char* szDest,size_t nDestSize,const char* szParam,const char* szValue);
bool	url_add_param_number			(char* szDest,size_t nDestSize,const char* szParam,long long nValue);
bool	url_add_params					(const char* szSourceUrl,const char* pParams,char* szNewUrl,size_t nNewUrl);

void	url_translate_HTML_entity		(char* url);

char*	url_abbreviate					(const char* pUrl,int nSize,char* pOutput,size_t nOutput);

char*	decode_percent_encoded_string	(char* encoded_url);
char*	decode_percent_encoded_string_a	(const char* encoded_str);
char*	encode_string_to_percent_encoded(const char* original_str,const char* chars_to_encode);

void	decode_escape_sequences			(char* str);
char*	decode_unicode_escape_sequences	(char* str);

int		hex_to_int						(char c);

/* interfaccia C++ */
#ifdef __cplusplus
  }
#endif

#endif /* _URLPARSER_H */
