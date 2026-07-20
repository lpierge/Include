/*$
	getopt.h
	Gestione opzioni/parametri ricevuti da linea di comando.
	Luca Piergentili, 31/08/98
	lpiergentili@yahoo.com

	Vedi le note in getopt.c.
*/
#ifndef _GETOPT_H
#define _GETOPT_H 1

#include <stdbool.h>
#include "typeval.h"

/* interfaccia C++ */
#ifdef __cplusplus
  extern "C" {
#endif

#define MAX_CMDLINE 4096 // in teoria 8192 via cmd.exe in Windows (moderno)

char*	GetSafeCommandLine		(char* pCmdLine);
char*	GetCommandLineAsString	(int argc,char* argv[],char* szBuffer,size_t nBuffer);
void	BuildCustomArgs			(const char* cmd,int* custom_argc,char*** custom_argv);

/*
	GETOPT
	struttura per specificare/ricevere i dati dell'opzione
*/
typedef struct _getopt {
	char	cOpt;	/* specifica:	carattere per l'opzione */
	bool	bFound;	/* riceve:		boolean per opzione presente */
	bool	bArgs;	/* specifica:	se l'opzione prevede argomenti, riceve: se l'argomento era presente */
	TYPE	eType;	/* specifica:	tipo dell'argomento (enum) */
	VALUE	uValue;	/* riceve:		valore argomento (union) */
} GETOPT;

// ABCDEFGHIJKLMNOPQRSTUVWXYZ

int getopt(	char		cFlag,				/* carattere da usare per indicare l'opzione ('/' o '-') */
			GETOPT*		opts,				/* puntatore all'array GETOPT per le opzioni */
			int			optsSize,			/* dimensione dell'array */
			int			argc,				/* numero di argomenti della funzione main() */
			char		*argv[],			/* array di puntatori agli argomenti della funzione main */
			char		*wrongOptsBuffer,	/* buffer per ricevere l'opzione erronea */
			int			wrongOptsSize,		/* dimensione del buffer */
			const char*	cmdLine				/* linea di comando completa */
			);

/* interfaccia C++ */
#ifdef __cplusplus
  }
#endif

#endif // _GETOPT_H
