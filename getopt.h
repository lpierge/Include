/*$
	getopt.h
	Gestione opzioni/parametri ricevuti da linea di comando.
	Luca Piergentili, 31/08/98
	lpiergentili@yahoo.com
*/
#ifndef _GETOPT_H
#define _GETOPT_H 1

/* interfaccia C++ */
#ifdef __cplusplus
  extern "C" {
#endif

#include <stdbool.h>
#include "typeval.h"

#define MAX_CMDLINE 4096 // in teoria 8192 via CMD.EXE in Windows (moderno)

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
