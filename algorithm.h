/*$
	algorithm.h
	Algoritmi vari.
	Luca Piergentili, Luglio '25
*/
#ifndef _ALGORITHM_H
#define _ALGORITHM_H 1

#include <stdbool.h>
#include <stdint.h>
#include <limits.h>

/* interfaccia C++ */
#ifdef __cplusplus
  extern "C" {
#endif

bool			swing							(int *value,int min,int max,int dec,int inc,bool *status);
bool			get_weighted_boolean			(int nThreshold);
bool			multiset_equality_check			(const char* array_of_sequences[],int array_size,const char* target_sequence);
int				linear_map						(int inputVal,int outMin,int outMax);
uint64_t		hash_string_FNV1a				(const char* str);
uint64_t		hash_normalized_string_FNV1a	(const char* str);

/* interfaccia C++ */
#ifdef __cplusplus
  }
#endif

#endif // _ALGORITHM_H
