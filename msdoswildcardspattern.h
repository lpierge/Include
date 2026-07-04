/*$
	msdoswildcardspattern.h
	Luca Piergentili, 26/06/25

	Implementa il match con un pattern (una specifica che puo' contenere wildcards)
	nello stesso modo in cui veniva implementato dall'MS-DOS nelle ricerche sui nomi
	dei files.

	Contiene le versioni ANSI e Wide (Unicode).

	Vedi le note in msdoswildcardspattern.c
*/
#ifndef _MSDOSWILDCARDSPATTERN_H
#define _MSDOSWILDCARDSPATTERN_H 1

#include <stdbool.h>
#include <wctype.h>

/* interfaccia C++ */
#ifdef __cplusplus
  extern "C" {
#endif

bool	msdos_wildcards_match	(const char* text,const char* pattern);
bool	msdos_wildcards_matchW	(const wchar_t* text,const wchar_t* pattern);

void	msdos_normalize_path	(char* path);
void	msdos_normalize_pathW	(wchar_t* path);

bool	msdos_pattern_match		(const char* fullpath,const char* pattern);
bool	msdos_pattern_matchW	(const wchar_t* fullpath,const wchar_t* pattern);

bool	msdos_pattern_excluded	(const char* fullpath,const char* exclusions_array[],int num_exclusions);
bool	msdos_pattern_excludedW	(const wchar_t* fullpath,const wchar_t* exclusions_array[],int num_exclusions);

/* interfaccia C++ */
#ifdef __cplusplus
  }
#endif

#endif // _MSDOSWILDCARDSPATTERN_H