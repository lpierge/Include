/*$
	typeval.h
	Tipi e valori.
	Luca Piergentili, 27/07/93
	lpiergentili@yahoo.com
*/
#ifndef _TYPEVAL_H
#define _TYPEVAL_H 1

#include <stdbool.h>

#define	STR_MAX_VALUE	1024

/* lo specificatore LL per forzare il compilatore a usare aritmetica 64bit evitanto overflow */
/*#define	BYTE	1*/
/*
#define 32KB		32768
#define 64KB		65536
*/
#define	KILOBYTE	1024LL
#define	MEGABYTE	1048576LL
#define	GIGABYTE	1073741824LL
#define	TERABYTE	1099511627776LL

/*
	TYPE
	struttura per i tipi
*/
#ifdef __cplusplus
	enum TYPE {
#else
	typedef enum type_t {
#endif
		none = 0,
		bool_type,
		chr_type,
		string_type,
		string_pointer,
		void_pointer,
		word_type,
		word_pointer,
		doubleword_type,
		doubleword_pointer,
        quadword_type,
        quadword_pointer,
        float_type,
        float_pointer,
        double_type,
        double_pointer
#ifdef __cplusplus
};
#else
} TYPE;
#endif

/*
	VALUE
	struttura per i valori
	la dimensione del tipo 'stringa' viene definita di STR_MAX_VALUE caratteri

	La diferencia principal entre float y double en C es la precisión y el tamaño en memoria. 
	double ofrece precisión doble, usa 64 bits (8 bytes) y puede almacenar hasta unos 15 dígitos 
	decimales, mientras que float tiene precisión simple, usa 32 bits (4 bytes) y almacena hasta 
	7 dígitos decimales.
	Por lo tanto, double es más adecuado para cálculos científicos o financieros que requieren 
	mayor exactitud. 

	Tabla comparativa
	Caracteristica				float									double
	Precision					Simple (aprox. 7 dígitos decimales)		Doble (aprox. 15 dígitos decimales)
	Tamaño en memoria			32 bits (4 bytes)						64 bits (8 bytes)
	Rango de valores			Aprox. desde 1.4 x 10^{-45}				Aprox. desde 4.9 x 10^{-324}
								a 3.4 x 10^{38}							a 1.8 x 10^{308}
	Uso comun					Graficos, cuando la precisión no es		Calculos financieros y científicos,
								crítica									donde se requiere alta precisión
	Por defecto					Los literales de punto flotante se		Es el tipo por defecto para literales
								vuelven double a menos que se les		de punto flotante
								añada el sufijo f o F

*/
#ifdef __cplusplus
	union VALUE {
#else
	typedef union value_t {
#endif
		bool	bValue;
		char	cValue;
		char	szValue[STR_MAX_VALUE+1];
		char*	pValuePtr;
		void*	pVoidPtr;
		int		wValue;
		int*	wValuePtr;
		long	dwValue;
		long*	dwValuePtr;
        long long qwValue;
        long long* qwValuePtr;
		float   fValue;
        float*	fValuePtr;
        double	dValue;
        double*	dValuePtr;
#ifdef __cplusplus
	};
#else
	} VALUE;
#endif

#ifdef __cplusplus
enum VALUETYPE {
#else
typedef enum {
#endif

	WORD_TYPE = 0,
	DWORD_TYPE,
	QWORD_TYPE,
	FLOAT_TYPE,
	DOUBLE_TYPE	
#ifdef __cplusplus
};
#else
} VALUETYPE;
#endif

#ifdef __cplusplus
	struct TAGGEDVALUE {
#else
	typedef struct taggedvalue_t {
#endif
		VALUETYPE valuetype;
		VALUE value;
#ifdef __cplusplus
	};
#else
	} TAGGEDVALUE;
#endif

/*
	VALUE_EXT
	struttura per valori (con dimensione campo)
*/
#ifdef __cplusplus
	struct VALUE_EXT {
#else
	typedef struct value_ext_t {
#endif
		long	dwSize;
		TYPE	eType;
		VALUE	eValue;
#ifdef __cplusplus
	};
#else
	} VALUE_EXT;
#endif

#endif // _TYPEVAL_H
