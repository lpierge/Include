/*$
	CHostCache.h
	Classe per la cache dei nomi host.
	Luca Piergentili, Settembre '25
*/
#ifndef _CHOSTCACHE_H
#define _CHOSTCACHE_H 1

#include "pragma.h"
#include <stdio.h>
#include <string.h>
#include "strings.h"
#include "window.h"
#include "ipaddress.h"
#include "CWinsock.h"

// dimensione dell'array per la cache
#define CACHE_SIZE 64

// elemento della cache: 0=vuoto, n=disponibile
#define CACHE_ENTRY_FREE 0 

/*
	HOSTCACHEENTRY
*/
typedef struct _hostcacheentry {
    char			hostname[HOSTNAME_LEN+1];
    char			ipaddress[IP_ADDRESS_LEN+1];
    unsigned int	counter;
	int				wsaerror;
} HOSTCACHEENTRY;

/*
	CHostCache
*/
class CHostCache
{
public:
	CHostCache();
	virtual ~CHostCache();

	int ResolveHost(LPCSTR lpcszHostname,LPSTR lpszIPAddress,UINT nIPAddress);

private:
	int FindCacheEntry(LPCSTR lpcszHostname);
	int FindLFUIndex(void);

	CWinsock m_Socket;
	HOSTCACHEENTRY m_chacheArray[CACHE_SIZE];
};

#endif // _CHOSTCACHE_H
