/*$
	typedef.h
	Definizione dei tipi.
	Luca Piergentili, 27/07/93
	lpiergentili@yahoo.com
*/
#ifndef _TYPEDEF_H
#define _TYPEDEF_H 1

#define VIRTUAL
#define IN
#define OUT

/*
	TERN
*/
#ifndef _TERN_TYPE
typedef enum _tern {
	False = 0,
	True = 1,
	Undef = 2
} TERN;
#endif

/*
	BOOL
*/
#ifndef _BOOL_TYPE
  typedef int BOOL;
  #define TRUE 1
  #define FALSE 0
  #define _BOOL_TYPE 1
#endif

/*
	QWORD
*/
#ifndef _QWORD_TYPE
  #if defined(_WINDOWS) && defined(_MSC_VER)
    typedef unsigned __int64 QWORD;
	// usare LONGLONG se si ha bisogno di una QWORD signed
  #else
    typedef unsigned long long QWORD;
  #endif
  typedef QWORD *LPQWORD;
  #define _QWORD_TYPE 1
#endif

/*
	FPCALLBACK
	tipo per il puntatore a funzione per le callback
*/
#ifndef _FPCALLBACK_TYPE
  #define _FPCALLBACK_TYPE 1
  /*typedef UINT (*FPCALLBACK)(LPVOID,LPVOID);*/
  typedef unsigned int (*FPCALLBACK)(void*,void*);
#endif /* _FPCALLBACK_TYPE */

#endif /* _TYPEDEF_H */
