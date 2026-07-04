/*$
	wildcardspattern.h
	Gestione di pattern con wildcards per nomi files.
	Lasciare come codice sciolto, NON convertire in una classe.
	Luca Piergentili, 26/06/25
*/
#ifndef _WILDCARDSPATTERN_H
#define _WILDCARDSPATTERN_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

#define WILDCARDS_MATCH_STANDARD	1
#define WILDCARDS_MATCH_EXTENDED	2
#define WILDCARDS_MATCH_MSDOS		3

std::vector<std::string> SplitPattern		(const char* patternStr);

bool DoesStringMatchWithPattern				(const char* text,const char* pattern);

bool DoesFileNameMatchWithPattern			(const char* fullPath,const char* pattern);
bool DoesFileNameMatchWithPatternEx			(const char* fullPath,const char* pattern);
bool DoesFileNameMatchWithPatternMSDOS		(const char* fullPath,const char* pattern);

bool DoesFileNameMatchWithVectorOfPatterns	(const char* name,const std::vector<std::string>& exclusions,int matchtype);

#endif /* _WILDCARDSPATTERN_H */
