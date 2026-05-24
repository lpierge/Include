/*$
	macro.h
	Macro di utilizzo generico.
	Luca Piergentili, 27/07/93
	lpiergentili@yahoo.com
*/
#ifndef _MACRO_H
#define _MACRO_H 1

#define nop
#define AND &&
#define OR ||

/*
	sizeofmacro()
	dimensione della macro
*/
#define sizeofmacro(m) (sizeof(m)-1)

/*
	IS_EVEN()
	true se il numero e' pari
*/
#define IS_EVEN(n) (!((n) & 0x01))

static inline int _IS_SPACE(char c) {return(c==' ' || c=='\t' || c=='\r' || c=='\n' || c=='\v' || c=='\f');}

#define IS_SPACE(c) _IS_SPACE(c)
/*
	ARRAY_SIZE
	dimensione dell'array
*/
#define ARRAY_SIZE(a)		(sizeof(a)/sizeof(a[0]))

/*
	ARRAY_[...]_SIZE
	per array a 2 dimensioni, come: char args[128][16];
*/
#define ARRAY_ROWS_SIZE(a)	(sizeof(a) / sizeof(a[0]))          // 128
#define ARRAY_COLS_SIZE(a)	(sizeof(a[0]) / sizeof(a[0][0]))    // 16

/*
	IIF()
	test in linea
*/
#define IIF(x,y,z) ((x)?(y):(z))

/*
	DENULLIFY
*/
#define DENULLIFY(ptr) (ptr ? ptr : "null")
//static inline const char* DENULLIFY(const char* ptr) {return ptr ? ptr : "null";}

/*
	DIV()
	evita la divisione per zero
*/
#define DIV(x,y) (((y)==0) ? ((x)/1) : ((x)/(y)))
#define FDIV(x,y) (((y)==0.0) ? ((x)/1) : ((x)/(y)))

/*
	SWAP()
	scambia i due valori
	il ciclo do { ... } while (0) rende la macro utilizzabile come una singola istruzione 
	e gestisce correttamente i punti e virgola, evitando problemi se la macro viene usata 
	in contesti come un if senza parentesi graffe
	le parentesi extra attorno ad a e b sono necessarie per evitare problemi con la precedenza 
	degli operatori quando gli argomenti passati alla macro sono espressioni complesse, es:
	SWAP(x + y, z * 2, int)
*/
#define SWAP(a, b, type) \
    do {                 \
        type temp = (a); \
        (a) = (b);       \
        (b) = temp;      \
    } while (0)

/*
	STR()
	converte in stringa il valore numerico
	il primo passaggio espande il valore numerico (sostituisce il nome di macro con il numero),
	mentre il secondo trasforma in stringa il valore (ad es. STR(666) viene espanso in "666")
*/
#define VAL(x) #x
#define STR(x) VAL(x)

/*
	CAST()
	permette il cast sulle macro se non si puo' accedere direttamente al valore della costante
	(ad es. se BYTEMASK vale 0xff, espande CAST(BYTEMASK,L) in 0xffL).
*/
#define EXPAND(x)	x
#define PASTE(x,y)	(x##y)
#define CAST(x,y)	PASTE(EXPAND(x),y)

/*
	NEARLYEQUAL()
*/
#define NEARLYEQUAL(a,b,epsilon) (abs((int)a - (int)b) < ((int)epsilon / 2))

/*
	PRAGMA_MESSAGE()
	indenta i messaggi inviati con la direttiva #pragma
*/
#ifdef _DEBUG
  #define PRAGMAMESSAGE(s) "   "s
  #define PRAGMA_MESSAGE(s) message(PRAGMAMESSAGE(s))
#else
  #define PRAGMAMESSAGE(s)
  #define PRAGMA_MESSAGE(s)
#endif

#endif /* _MACRO_H */
