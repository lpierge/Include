/*$
	CRand.h
	La classe CRand e' un wrapper per il codice originale di Takuji Nishimura e Makoto Matsumoto (vedi sotto).
	La classi CRandom (generazione con servizio crittografico) e CCardsDeck (generazione con distribuzione
	uniforme), entrambe originali, sono posteriori.
	Luca Piergentili, Agosto '03

	Vedi anche il codice in fastrand.h per randomizzazione Xorshift32.
*/

/* 
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Before using, initialize the state by using init_genrand(seed)  
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.keio.ac.jp/matumoto/emt.html
   email: matumoto@math.keio.ac.jp
*/
#ifndef _CRAND_H
#define _CRAND_H 1

#include <stdio.h>
#include <stdlib.h>
#include "window.h"
#include <wincrypt.h>
#include "fastrand.h"

#define N_SEED 624
#define M_SEED 397

/*
	CRand
*/
class CRand
{
public:
	CRand(unsigned long s = (unsigned long)-1L);
	virtual ~CRand() {}

	unsigned long Rand(void)
	{
		unsigned long n = labs(genrand_int32());
		if(n==m_nPrevious)
		{
			while(n==m_nPrevious)
				n = labs(genrand_int32());
		}
		else
			m_nPrevious = n;
		return(n);
	}
	
	unsigned long	RandMax(unsigned long nMax) {return(Rand() % nMax);}
	unsigned long	RandRange(unsigned long nMin,unsigned long nMax) {return(Rand() % (nMax - nMin + 1) + nMin);}
	
	void			Seed(unsigned long s) {init_genrand(s);}
	void			SeedArray(unsigned long init_key[],int key_length) {init_by_array(init_key,key_length);}

private:
	void			init_genrand(unsigned long s);
	void			init_by_array(unsigned long init_key[],int key_length);
	unsigned long	genrand_int32(void);
	long			genrand_int31(void);
	double			genrand_real1(void);
	double			genrand_real2(void);
	double			genrand_real3(void);
	double			genrand_res53(void);

	unsigned long	m_nPrevious;
	unsigned long	mt[N_SEED]; /* the array for the state vector  */
	int				mti; /* mti==N_SEED+1 means mt[N_SEED] is not initialized */
};

/*
	CRandom
*/
class CRandom
{
public:
	CRandom()
	{
		m_hCryptProv = 0;
		m_bInitialized = InitializeRandomGenerator();
	}

	virtual ~CRandom()
	{
		m_bInitialized = !CleanupRandomGenerator();
	}

	ULONG		GenerateRandomNumber(ULONG min,ULONG max);

private:
	BOOL		m_bInitialized;
	HCRYPTPROV	m_hCryptProv;

	BOOL		InitializeRandomGenerator(void);
	BOOL		CleanupRandomGenerator(void);
};

/*
	CARDSDECK

	Il mazzo di carte usato dall'algoritmo in CCardsDeck()
*/
typedef struct _cards_deck {
	int	*pool;
	int	range;
} CARDSDECK;

/*
	CCardsDeck

	Variante dell'algoritmo shuffle di Fisher-Yates:
	L'algoritmo Fisher-Yates (o Knuth shuffle) serve a mescolare in modo casuale gli elementi di un array, 
	garantendo che ogni possibile permutazione abbia esattamente la stessa probabilita' di verificarsi 
	(distribuzione uniforme).

	Concettualmente, un insieme e' aleatorio quando la sequenza dei suoi elementi e' completamente casuale,
	mentre secondo la nostra percezione, "aleatorio" significa piuttosto "disordinato", ossia in uno stato 
	in cui uno qualsiasi dei nostri schemi mentali viene rotto.

	Una sequenza 1,1,1,1 puo' essere perfettamente aleatoria come la sequenza 10,2450,0,445, pero' a causa
	della nostra forma mentis, la prima sequenza viene percepita come "ordinata" e solo la seconda come
	realmente "aleatoria", ossia random.

	Ci si scervella su come implementare il miglior algoritmo possibile ed i migliori test per verificare la
	bonta' di ogni algoritmo di randomizzazione, ma qualsiasi algoritmo sara' sempre ugualmente buono rispetto 
	al resto, perche' per definizione "aleatorio" significa appunto che sfugge a qualisasi regola predittiva.
	Il che dimostra che per noi umani "aleatorio" in realta' solo significa "disordinato".
	Il fatto che si stia cercando di usare la logica (l'algoritmo) per produrre l'illogico (il caso) risulta
	ingenuo e tenero allo stesso tempo e produce una contraddizione in termini.

	Come disse John von Neumann:
	"Chiunque consideri metodi aritmetici per produrre cifre casuali e', ovviamente, in uno stato di peccato."

	I tre tipi basici di randomizzazione possono riassumersi cosi':

	1)	si mescola il mazzo di carte ogni volta e si estrae una carta che si rimette poi nel mazzo
		questo garantisce un estrazione aleatoria ma NON garantisce che la carta non si ripresenti anche
		varie volte di seguito
	2)	si mescola il mazzo di carte solo una volta e si estrae poi una carta alla volta, eliminandola
		questo garantisce un estrazione aleatoria e garantisce che la carta NON si possa ripresentare, ma
		la randomizzazione viene prodotta solo una volta all'inizio e poi consumata tramite un estrazione
		sequenziale
	3)	si mescola il mazzo di carte ogni volta, si estrae una carta e si elimina la carta appena estratta
		questo garantisce un estrazione aleatoria e garantisce che la carta NON si possa ripresentare, la 
		randomizzazione viene prodotta ogni volta sulle carti rimanenti

	Qui usa il metodo 3), con il seguente procedimento:

	- genera un numero casuale (indice) compreso nel totale degli elementi
	- ricava e restituisce il valore a tale indice
	- sostituisce il valore a tale indice con quello del'ultimo elemento
	- riduce di 1 il numero di elementi totali entro cui effettuare la prossima generazione
	- se tale riduzione arriva ad esaurire il numero di elementi, reimposta il totale con quello originario,
	  reiniziando il ciclo
	- il totale degli elementi viene stabilito con il valore passato al costruttore, se dovesse cambiare tale
	  numero per esigenze del chiamante (si chiama la Shuffle() con un valore diverso da quello usato con il 
	  costruttore), reinizializza l'intero array usato internamente

	Occhio: i (signed) int su Win32/Win64 (a 4 bytes) possono arrivare fino a 2.147.483.647, ma qui il codice 
	usa la funzione rand(), che per motivi di compatibilita' storica (int a 2 byte) puo' generare numeri fino 
	ad un massimo di RAND_MAX (32767), quindi il massimo range ammesso qui per la sequenza da randomizzare e' 
	appunto RAND_MAX.

	Vedi anche:
	https://rosettacode.org/wiki/Knuth_shuffle
	https://www.geeksforgeeks.org/artificial-intelligence/what-is-monte-carlo-simulation/
*/
class CCardsDeck
{
public:
	CCardsDeck(unsigned long ulTot);
	virtual ~CCardsDeck();

	void			Initialize	(void);
	int				Shuffle		(int nTotal,int& nLeft);
	unsigned int	Shuffle		(unsigned int ulTotal,unsigned int& ulLeft) {return(fast_rand_range(ulTotal));}

private:
	CARDSDECK	m_cardsDeck;
	int			m_nTot;
};

#endif // _CRAND_H
