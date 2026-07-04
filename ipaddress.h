/*$
	ipaddress.h
	Gestione stringhe IP.
	Luca Piergentili, Settembre '25
*/
#ifndef _IP_H
#define _IP_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* interfaccia C++ */
#ifdef __cplusplus
  extern "C" {
#endif

#define IP_ADDRESS_LEN	16
#define HOSTNAME_LEN	256

void	generate_ip_address	(char* buffer,size_t size,int min_octet/* -1 */,int max_octet/* -1 */);
int		replace_ip_address	(const char* str,char* buffer,size_t size,int min_octet/* -1 */,int max_octet/* -1 */,const char* hook/* NULL */);
int		extract_ip_address	(const char* str,char* buffer,size_t size);

/* interfaccia C++ */
#ifdef __cplusplus
  }
#endif

#endif /* _IP_H */
