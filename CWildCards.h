/*$
	CWildCards.h
	Classe base per gestione skeleton/pattern.
	Luca Piergentili, 26/06/00
	lpiergentili@yahoo.com

	Notare che con 'skeleton' si intende un pattern 'esatto', che NON include wildcards, mentre con 'pattern' si 
	intende una stringa che si puo' contenere wildcards (?*).

	Il nome della classe e di alcuni suoi elementi ('wildcard') e' dovuto al fatto che inizialmente il codice solo
	gestiva wildcards, la differenziazione in skeleton (pattern esatto) e pattern (una specifica che puo' contenere 
	wildcard) e' venuta a posteriori.

	Incapsulato il codice originale di Florian Schintke:

		"Implementation of the UN*X wildcards in C. So they are available in a portable way and can be used
		whereever	needed.
		Author(s):
		Florian Schintke (schintke@cs.tu-berlin.de)
		Dates:
		First editing: unknown, but before 04/02/1997
		Last Change  : 11/10/1998"

	e aggiunto il codice per gestire piu' pattern in un unica stringa (separati da spazio). Aggiunto anche 
	il codice per cercare sottostringhe esatte, dato che l'originale di Schintke non e' progettato per cercare 
	come farebbe una funzione di ricerca stringa standard, tipo strstr().
	Vedi le note in Match().

	Il membro SplitPattern() separa e serializzare in una lista di tipo CItemList i pattern passati come una 
	unica stringa e separati dal carattere ';'. 
	Il chiamante dovra' poi agganciare la lista tramite il membro GetItemList() e scorrerla chiamando Match() e/o 
	MatchSubString() su ogni elemento, per verificare se la stringa fa match con lo skeleton/pattern della lista.
	Al contrario, con il meccanismo originario della classe, la SetIgnoreSpaces(), che permette di considerare 
	gli spazi nella stringa per le wildcards come un separatore, si genera una lista interna di pattern che viene 
	verificata internamente.
	
	Note:

	- Wildcard stile MS-DOS:
	Semplici e limitate, tipicamente supportano solo:
	*: qualsiasi sequenza di caratteri (inclusa vuota)
	?: un singolo carattere qualsiasi
	Non includono set di caratteri come [a-z], negazioni (!), o altre feature avanzate. Sono pensate per matching 
	di file nel prompt dei comandi, e derivano da CP/M e dai primi sistemi DOS. Ad esempio, *.txt matcha tutti i 
	file TXT, ma non si puo' specificare un range come [a-c]*.txt.

	- Wildcard stile UNIX:
	Questo e' quanto implementa il codice di Florian Schintke. E' piu' espressivo di MS-DOS, ma non arriva alla 
	potenza delle espressioni regolari complete. Supporta:
	*: qualsiasi sequenza di caratteri
	?: un singolo carattere qualsiasi
	Set di caratteri: [a-z] (range), [abc] (specifici), e negazione con ! o ^ come [!a-z] (tutto tranne lettere 
	minuscole).
	Non supporta quantificatori come + (uno o piu'), {n,m} (ripetizioni), ancore come ^ o $ (inizio/fine stringa), 
	o alternanze complesse come (a|b).
	E' ispirato al matching di file nelle shell UNIX (come bash o sh), simile alla funzione fnmatch() della libreria 
	standard C (POSIX). Negli esempi del codice: '[a-g]l*i?n' matcha "florian" perche' combina set, *, ? e sequenze.
*/
#ifndef _CWILDCARDS_H
#define _CWILDCARDS_H 1

#include "typedef.h"
#include "CNodeList.h"

/*
	CWildCardItem
	classe per il singolo elemento della lista delle wildcards
*/
class CWildCardItem {
public:
	CWildCardItem() {m_pString = NULL;}
	CWildCardItem(char* pString);
	~CWildCardItem();

	inline const char* GetString(void) const {return(m_pString);}
	inline void Reset(void) {if(m_pString) delete [] m_pString; m_pString = NULL;}

private:
	char* m_pString;
};

/*
	CWildCardList
	classe per la lista delle wildcards
*/
class CWildCardList : public CNodeList
{
public:
	CWildCardList() : CNodeList() {}
	virtual ~CWildCardList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new CWildCardItem());
	}	
	void* Initialize(void* pVoid)
	{
		CWildCardItem* pData = (CWildCardItem*)pVoid;
		if(!pData)
			pData = (CWildCardItem*)Create();
		if(pData)
			pData->Reset();
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((CWildCardItem*)iter->data)
			delete ((CWildCardItem*)iter->data),iter->data = (CWildCardItem*)NULL;
		return(TRUE);
	}
	int Size(void) {return(-1);}
#ifdef _DEBUG
	const char* Signature(void) {return("CWildCardList");}
#endif
};

/*
	CWildCards
	classe base per gestione wildcards (?*)
	distingue tra maiuscole e minuscole
	a seconda del flag (m_bIgnoreSpaces) considera la stringa contenente i pattern 
	come un solo valore o come piu' valori (separati da uno spazio)
*/
class CWildCards
{
public:
	CWildCards(BOOL bIgnoreCase = FALSE,BOOL bIgnoreSpaces = TRUE)
	{
		m_bIgnoreCase = bIgnoreCase;
		m_bIgnoreSpaces = bIgnoreSpaces;
		m_pInternalItemList = NULL;
	}

	virtual ~CWildCards()
	{
		if(m_pInternalItemList)
			delete m_pInternalItemList;
	}

	// accetta wildcards (pattern)
	BOOL			Match			(const char* skeleton,const char* string);
	
	// solo sottostringhe, no wildcards (skeleton)
	BOOL			MatchSubString	(const char* substring,const char* string);

	inline void		SetIgnoreCase	(BOOL bFlag) {m_bIgnoreCase = bFlag;}
	inline BOOL		GetIgnoreCase	(void) const {return(m_bIgnoreCase);}
	
	// forma originaria per considerare piu pattern in una stringa, separandoli con uno spazio
	inline void		SetIgnoreSpaces	(BOOL bFlag) {m_bIgnoreSpaces = bFlag;}
	inline BOOL		GetIgnoreSpaces	(void) const {return(m_bIgnoreSpaces);}

	// forma aggiuntiva per considerare piu pattern in una stringa, separati da ;
	// forma aggiuntiva per considerare piu pattern in una stringa, separati da ;
	CItemList*		SplitPattern	(const char* patternStr);
	CItemList*		GetItemList		(void) {return(m_pInternalItemList);}

	inline void		Reset			(void) {m_listWildcards.EraseAll(); if(m_pInternalItemList) m_pInternalItemList->EraseAll();}

private:

	BOOL			match			(int searchtype,const char* wildcard,const char* string);
	int				match_floriansk	(const char* wildcards,const char* string);
	BOOL			match_substring	(const char* substring,const char* string);

	int				set				(const char** wildcard,const char** string);
	int				asterisk		(const char** wildcard,const char** string);

	BOOL			m_bIgnoreCase;
	BOOL			m_bIgnoreSpaces;
	CWildCardList	m_listWildcards;

	// la lista degli item estratti dalla lista di inclusione/esclusione tramite la 
	// SplitPattern()
	// notare che tale funzione, oltre ad impostare i dati dell'item con la stringa
	// estratta, imposta il flag per sapere se la stringa contiene o no wilcards (?/*)
	// tale flag verra' poi usato per decidere quale membro usare per verificare il
	// match, Match() o MatchSubString()
	CItemList*		m_pInternalItemList;
};

#endif // _CWILDCARDS_H
