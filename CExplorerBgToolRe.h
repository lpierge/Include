/*
	CExplorerBgToolRe.h
	Classe per l'interfaccia con la DLL ExploreBgToolRe.dll.
	Unicode esplicito.
	Luca Piergentili, 18/06/2026

	Note:
	Presupposto fondamentale per il funzionamento del codice e' l'uso della versione "custom" della DLL
	ExplorerBgTool (ExploreBgToolRe.dll), dato che la versione originale NON permette la registrazione a 
	meno che il programma chiamante sia regsvr32.exe.
	Per un elenco completo dei bugs risolti e delle modifiche apportare all'originale, vedi le note nel
	file dllmain.cpp del progetto ExploreBgToolRe.
*/
#ifndef _CEXPLORERBGTOOLRE_H
#define _CEXPLORERBGTOOLRE_H 1

#include "typedef.h"
#include "window.h"

/*
	CExplorerBgToolRe()
*/
class CExplorerBgToolRe
{
public:
	CExplorerBgToolRe();
	~CExplorerBgToolRe();

	HRESULT	GetVersion			(const wchar_t* pcwzDllPath,int& major,int& minor,int& patch);

	HRESULT	Register			(const wchar_t* pcwzDllPath);
	HRESULT	Unregister			(const wchar_t* pcwzDllPath);
	HRESULT	Unregister			(void);

	TERN	IsRegistered		(const wchar_t* pcwzDllPath);
	TERN	IsRegistered		(wchar_t* pwzDllPath,size_t nSize);

	BOOL	IsAdmin				(void);

	BOOL	RestartExplorer		(void);

private:
    FARPROC	LoadFunction		(const wchar_t* pcwzDllPath,const char* pProcName);
    void	UnloadDll			(void);

    static const wchar_t* CLSID_EXPLORERBGTOOL;
    HMODULE m_hDll;
	DWORD	m_dwError;
};

#endif // _CEXPLORERBGTOOLRE_H
