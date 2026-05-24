/*$
	algorithm.h
	Algoritmi.
	Luca Piergentili, Luglio '25

	Vedi anche il codice in CRand.h
*/
#ifndef _ALGORITHM_H
#define _ALGORITHM_H 1

/* interfaccia C++ */
#ifdef __cplusplus
  extern "C" {
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>

bool			swing							(int *value,int min,int max,int dec,int inc,bool *status);
bool			get_weighted_boolean			(int nThreshold);
bool			multiset_equality_check			(const char* array_of_sequences[],int array_size,const char* target_sequence);
int				linear_map						(int inputVal,int outMin,int outMax);

uint64_t		hash_string_FNV1a				(const char* str);
uint64_t		hash_normalized_string_FNV1a	(const char* str);


/*
	Xorshift

	Xorshift32 e' un PRNG (pseudo random numer generator) leggendario per velocita'.
	Questa implementazione specifica (13, 17, 5) e' quella classica di George Marsaglia.
	Ha un periodo 2[32]-1 (non 2[32], perche' lo stato zero e' proibito dato che blocherebbe il generatore.

	rand(): ~20-40 cicli CPU
	fast_rand(): ~4-6 cicli CPU (5-8x piu' veloce)

	Non e' PCG, Un vero PCG (es. M.E. O'Neill) ha permutazione e passa batterie di test come BigCrush, ma 
	va perfettamente se la velocita' e' la priorita assoluta e se lo scopo e' proprio quello di mostrare la 
	differenza tra la percezione di qualcosa di aleatorio e l'aleatorio "matematico".
	Altrimenti passare a xoroshiro128+ (periodo 2[128-1]), o PCG, o xorshift64* che ha migliori proprieta'
	statistiche con stesso costo. Per crittografia andrebbe usato RAND_bytes da OpenSSL.
*/

void			init_xorshift32					(void);
//unsigned int	fast_rand						(void);
//unsigned int	fast_rand_range_between			(unsigned int start,unsigned int end);
//unsigned int	fast_rand_range					(unsigned int range);
//int			fast_rand_symmetric				(int amplitude);
//bool			fast_rand_chance				(int probability_per_mille);


extern unsigned int xorshift32_state;


/*
	rand_m()

	La rand() standard, a parte il problemi con la globalita' del seme via srand(),
	NON restituisce un numero realmente aleatorio, ma un progressivo a partire dalla
	sequenza di un generatore interno.
	Qui bypassa il problema di srand() e cerca di restituire un numero nel modo piu'
	disordinato ("m" per "messy") possibile.
	Controlla il valore di RAND_MAX perche' su sistemi moderni puo' arrivare a valere
	2147483647, ma qui emulare la rand() originale (ANSI C tipico).
*/
#if RAND_MAX > 32767
  #error RAND_MAX beyond tipical ANSI C value
#endif
#define rand_m() ((int)fast_rand_range_between(0,RAND_MAX))


/*
	rand_w()

	La rand() standard non accetta parametri, qui implementa invece una rand che restituisce un
	valore compreso nell'intervallo ("w" per "within").
	Controlla il valore di RAND_MAX perche' su sistemi moderni puo' arrivare fino a 2147483647,
	mentre qui emula la rand() originale (ANSI C tipico) che restituisce un int a 16bit (32768).
*/
int rand_w(int,int);

/*
	fast_rand()

	Restituisce un valore tra 0 e UINT_MAX, le altre funzioni sono dei wrapper che trasformano questo valore in range specifici:

	fast_rand()				-> 0 a 4.294.967.295 (tutto il range UINT)
	fast_rand_range(N)		-> 0 a N-1 (scalato uniformemente) - OCCHIO: passare il TOTALE dei valori possibili, ossia N, a base 0 e N-1 e' interno per il calcolo
	fast_rand_symmetric(A)	-> -A a +A (centrato su zero)
	fast_rand_chance(P)		-> true/false con probabilita' P/1000

	fast_rand() potrebbe restituire    : 3.827.492.115
	fast_rand_range(100) restituira'   : (3.827.492.115 * 100) >> 32 ~ 89
	fast_rand_symmetric(50) restituira': (valore % 101) - 50 ~ tra -50 e +50
	fast_rand_chance(300) restituira'  : (valore % 1000) < 300 -> 30% probabilita' true
*/
__forceinline unsigned int fast_rand(void)
{
	unsigned int x = xorshift32_state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	xorshift32_state = x;
	return(x);
}


/*
	fast_rand_range_between()

	Restituisce un valore compreso tra start ed end (inclusi).
*/
__forceinline unsigned int fast_rand_range_between(unsigned int start,unsigned int end)
{
	// calcola quanti numeri ci sono nell'intervallo, es. tra 1 e 10 ci sono (10 - 1 + 1) = 10 numeri
	unsigned int range = (end - start) + 1;

	// usa la logica fast_rand_range sull'ampiezza e poi trasla il risultato
	return(((unsigned long long)fast_rand() * range >> 32) + start);
}


/*
	fast_rand_range()

	Restituisce un valore tra 0 e (range-1).
*/
__forceinline unsigned int fast_rand_range(unsigned int range)
{
	/* usa i bit alti per disordine migliore */
	return(((unsigned long long)fast_rand() * range) >> 32);
}


/*
	fast_rand_symmetric()

	Restituisce un valore tra -amplitude e +amplitude.
*/
__forceinline int fast_rand_symmetric(int amplitude)
{
#if 0
	// Bias enorme, usa % su un numero massimo 0x7FFFFFFF che non e' multiplo di (amplitude*2+1) nella maggior parte dei casi
	// lo stesso problema del rand() % <n> classico
	int val = fast_rand() & 0x7FFFFFFF;
	return((val % (amplitude * 2 + 1)) - amplitude);
#else
	int r = fast_rand_range(amplitude*2+1);
	return(r - amplitude);
#endif
}


/*
	fast_rand_chance()

	Per Salt & Pepper.
*/
__forceinline bool fast_rand_chance(int probability_per_mille)
{
#if 0
	// stesso problema di fast_rand_symmetric(): fast_rand() produce valori da 0 a UINT_MAX
	// UINT_MAX+1 = 4294967296 non e' multiplo di 1000, quindi i numeri 0..(4294967296 % 1000)-1 sono sovrarappresentati
	// soluzione corretta -> senza bias, ramo dell' #else
	/* probability_per_mille = nFactor (0-1000) */
	return((fast_rand() % 1000) < (unsigned int)probability_per_mille);
#else
	return(fast_rand_range(1000) < (unsigned int)probability_per_mille);
#endif
}

/* interfaccia C++ */
#ifdef __cplusplus
  }
#endif

#endif // _ALGORITHM_H
