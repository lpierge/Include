/*$
	datetime.h
	Operazioni su data/ora.
	Luca Piergentili, '98
	lpiergentili@yahoo.com
*/
#ifndef _DATETIME_H
#define _DATETIME_H 1

/* interfaccia C++ */
#ifdef __cplusplus
  extern "C" {
#endif

#include <stddef.h>

long long		unixTimeStamp		(void);
int				unixTimeStampToDate	(unsigned long ulValue,char *buffer,size_t size);
unsigned long	dateToUnixTimeStamp	(char *date);

/* interfaccia C++ */
#ifdef __cplusplus
  }
#endif

#endif // _DATETIME_H

