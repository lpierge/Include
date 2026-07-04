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

long long		unix_timestamp			(void);
int				unix_timestamp_to_date	(unsigned long ulValue,char *buffer,size_t size);
unsigned long	date_to_unix_timestamp	(char *date);

/* interfaccia C++ */
#ifdef __cplusplus
  }
#endif

#endif // _DATETIME_H

