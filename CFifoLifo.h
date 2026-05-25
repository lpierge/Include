/*$
	CFifoLifo.h
	Classi per liste FIFO/LIFO.
	Implementa la logica FIFO/LIFO usando una lista di tipo CNodeList.
	Luca Piergentili, Dicembre '18

	Esempio:

		const char* p;

		cLIFO lifo;
		printf("LIFO:\n");
		p = (const char*)lifo.Pop();
		printf("pop: %s\n",p ? p : "NULL");
		p = "pippo";
		lifo.Push((void*)p);
		printf("push: %s\n",p);
		p = (const char*)lifo.Pop();
		printf("pop: %s\n",p ? p : "NULL");
		p = (const char*)lifo.Pop();
		printf("pop: %s\n",p ? p : "NULL");
		p = (const char*)lifo.Pop();
		printf("pop: %s\n",p ? p : "NULL");
		p = "pluto";
		lifo.Push((void*)p);
		printf("push: %s\n",p);
		p = "paperino";
		lifo.Push((void*)p);
		printf("push: %s\n",p);
		p = "paperone";
		lifo.Push((void*)p);
		printf("push: %s\n",p);
		while((p = (const char*)lifo.Pop())!=NULL)
			printf("pop: %s\n",p);

		printf("\n");

		cFIFO fifo;
		printf("FIFO:\n");
		p = (const char*)fifo.Pop();
		printf("pop: %s\n",p ? p : "NULL");
		p = "pippo";
		fifo.Push((void*)p);
		printf("push: %s\n",p);
		p = "pluto";
		fifo.Push((void*)p);
		printf("push: %s\n",p);
		p = (const char*)fifo.Pop();
		printf("pop: %s\n",p ? p : "NULL");
		p = "paperino";
		fifo.Push((void*)p);
		printf("push: %s\n",p);
		p = (const char*)fifo.Pop();
		printf("pop: %s\n",p ? p : "NULL");
		p = (const char*)fifo.Pop();
		printf("pop: %s\n",p ? p : "NULL");
		p = "paperone";
		fifo.Push((void*)p);
		printf("push: %s\n",p);
		while((p = (const char*)fifo.Pop())!=NULL)
			printf("pop: %s\n",p);
*/
#ifndef _CFIFOLIFO_H
#define _CFIFOLIFO_H 1

#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"
#include "CNodeList.h"

#define MAX_FIFOLIFO_ITEMS_AT_TIME 256

/*
	LIFO

	elemento per l'oggetto pushato/poppato nella/dalla lista
*/
struct LIFO {
	int counter;	// progressivo assegnato all'elemento nella lista, per la ricerca
	void* ptr;		// puntatore per agganciare i dati (l'oggetto) forniti da chi usa la lista
};

/*
	CFIFOLIFO

	lista (ad uso interno) per gli oggetti da pushare/poppare
*/
class CFIFOLIFO : public CNodeList
{
public:
	CFIFOLIFO() : CNodeList() {}
	virtual ~CFIFOLIFO() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new LIFO);
	}	
	void* Initialize(void* pVoid)
	{
		LIFO* pData = (LIFO*)pVoid;
		if(!pData)
			pData = (LIFO*)Create();
		if(pData)
		{
			pData->counter = 0L;
			pData->ptr = NULL;
		}
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((LIFO*)iter->data) delete ((LIFO*)iter->data),iter->data = (LIFO*)NULL;
		return(TRUE);
	}
	int Size(void) {return(sizeof(LIFO));}
#ifdef _DEBUG
	const char* Signature(void) {return("CFIFOLIFO");}
#endif
};

/*
	cLIFO

	classe per la lista LIFO (Last In, First Out)
*/
class cLIFO
{
public:
	cLIFO ();
	virtual ~cLIFO ();

	BOOL	Push	(void* ptr);
	void*	Pop		(void);
	void	Reset	(void);	

private:
	int m_nCounter;			// progressivo numerico per identificare l'elemento della lista
	int m_nTotalSize;		// dimensione della lista
	CFIFOLIFO m_LIFOList;	// lista per gli elementi
};

/*
	cFIFO

	classe per la lista FIFO (First In, First Out)
*/
class cFIFO
{
public:
	cFIFO ();
	virtual ~cFIFO ();

	BOOL	Push	(void* ptr);
	void*	Pop		(void);
	void	Reset	(void);	

private:
	int m_nCounter;			// progressivo numerico per identificare l'elemento della lista
	int m_nLastPop;			// progressivo numerico per l'indice dell'ultimo elemento poppato
	int m_nTotalSize;		// dimensione della lista
	CFIFOLIFO m_FIFOList;	// lista per gli elementi
};

#endif // _CFIFOLIFO_H
