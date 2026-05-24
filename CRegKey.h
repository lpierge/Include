/*$
	CRegKey.h
	Classe base per l'accesso al registro (SDK/MFC).
	Riadattata dal codice originale ATL (M$) (vedi http://codeguru.earthweb.com/system/CRegKey.shtml).
	La chiave standard per registrare i dati del programma e': HKEY_CURRENT_USER\Software\[Fabbricante]\[App]
	Luca Piergentili, 14/07/99
	lpiergentili@yahoo.com
*/
#ifndef _CREGKEY_H
#define _CREGKEY_H 1

#include <stdio.h>
#include <stdlib.h>
#include "window.h"
#include <winreg.h>

#define	HKEY_CURRENT_ACCESS_LEVEL	HKEY_CURRENT_USER		// HKEY_CURRENT_USER se con HKEY_LOCAL_MACHINE da errore per mancanza di permessi (da admin)

#define REGKEY_MAX_SECTION_NAME		_MAX_PATH				// dim. max. nome sezione
#define REGKEY_MAX_KEY_NAME			REGKEY_MAX_SECTION_NAME	// dim. max. nome chiave
#define REGKEY_MAX_KEY_VALUE		REGKEY_MAX_SECTION_NAME	// dim. max. chiave

/*
	CRegistryKey
	(anteriormente CRegKey, rinominata perche' con VS2022 genera errore: simbolo ambiguo)
*/
class CRegistryKey
{
public:
					CRegistryKey()	{m_hKey = NULL;}
	virtual			~CRegistryKey() {CRegistryKey::Close();}

	LONG			Create			(	HKEY	hKeyParent,
										LPCSTR	lpcszKeyName,
										LPSTR = REG_NONE,
										DWORD = REG_OPTION_NON_VOLATILE,
#if defined( _M_IX86)
										// se non crea con KEY_ALL_ACCESS falliscono le Set...() etc., assicurarsi quindi di
										// usare HKEY_CURRENT_USER o di tenere i permessi per poter usare HKEY_LOCAL_MACHINE
										REGSAM = /*KEY_CREATE_SUB_KEY*/KEY_ALL_ACCESS | KEY_WOW64_64KEY,
#elif defined(_M_X64)
										REGSAM = KEY_CREATE_SUB_KEY,
#else
	#error "unsupported platform"
#endif
										LPSECURITY_ATTRIBUTES = NULL,
										LPDWORD = NULL
										);

	LONG			Open			(	HKEY	hKeyParent,
										LPCSTR lpcszKeyName,
#if defined( _M_IX86)
										// se non apre con KEY_ALL_ACCESS falliscono le Set...() etc., assicurarsi quindi di
										// usare HKEY_CURRENT_USER o di tenere i permessi per poter usare HKEY_LOCAL_MACHINE
										REGSAM = /*KEY_QUERY_VALUE*/ KEY_ALL_ACCESS | KEY_WOW64_64KEY
#elif defined(_M_X64)
										REGSAM = KEY_QUERY_VALUE
#else
	#error "unsupported platform"
#endif
										);
	
	LONG			Close			(void);

	LONG			QueryValue		(LPBYTE	lpszValue,LPCSTR lpcszValueName,DWORD* pdwCount);
	LONG			QueryValue		(DWORD&	dwValue,LPCSTR lpcszValueName);

	LONG			SetValue		(LPCSTR lpcszValue,LPCSTR lpcszValueName = NULL);
	LONG			SetValue		(DWORD dwValue,LPCSTR lpcszValueName);
	LONG			SetBinaryValue	(DWORD dwValue,LPCSTR lpcszValueName);
	LONG WINAPI		SetValue		(HKEY hKeyParent,LPCSTR lpcszKeyName,LPCSTR lpcszValue,LPCSTR lpcszValueName = NULL);
	LONG			SetKeyValue		(LPCSTR lpcszKeyName,LPCSTR lpcszValue,LPCSTR lpcszValueName = NULL);

	BOOL			DeleteValue		(LPCSTR lpcszValue);
	BOOL			DeleteKey		(LPCSTR lpcszKey);
	BOOL			DeleteSubKey	(LPCSTR lpcszSubKey);
	
	operator		HKEY() const	{return(m_hKey);}

private:
	HKEY m_hKey;
};

BOOL	CreateRegistryKey	(LPCSTR lpcszKey,LPCSTR lpcszName,LPCSTR lpcszValue);
BOOL	GetRegistryKey		(LPCSTR lpcszKey,LPCSTR lpcszName,LPSTR lpszValue,int nValueSize);
BOOL	DeleteRegistryKey	(LPCSTR lpcszKey,LPCSTR lpcszName);

#endif // _CREGKEY_H
