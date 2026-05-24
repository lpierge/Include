/*$
	CSync.h
	Classi per la sincronizzazione tra processi/threads.
	Luca Piergentili, 18/11/02
	lpiergentili@yahoo.com

	L'idea di base e' quella di esporre le stesse funzionalita' usando classi differenti, (al momento CSyncThreads
	e CSyncProcesses), ossia classi diverse ma con la stessa interfaccia anche se internamente usano oggetti diversi 
	(sezione critica e mutex).

	Lo scopo delle due classi CSyncThreads e CSyncProcesses e' di permettere a piu' threads serializzare l'accesso	
	esclusivo ad una risorsa.
	La differenza sta' nel fatto che CSyncThreads puo' essere usata solo da threads appartenenti allo stesso processo, 
	mentre CSyncProcesses puo' essere usata anche da processi differenti.

	Esempio d'uso:

	// prototipo
	unsigned __stdcall ThreadProc(void* p);	// run-time C
	// DWORD WINAPI ThreadProc(void* p);	// Win32

	class CThreadManager
	{
	public:
		BOOL Start(void)
		{
			//m_sync = new CSyncThreads("THREADS");
			m_sync = new CSyncProcesses("PROCESSES");

			for(int i = 0; i < 5; ++i)
				m_threads[i] = (HANDLE)_beginthreadex(NULL,0,ThreadProc,this,0,&m_id[i]);
				//m_threads[i] = CreateThread(NULL,0,ThreadProc,this,0,&m_id[i]);

			return(TRUE);
		}

		void Stop(void)
		{
			WaitForMultipleObjects(5,m_threads,TRUE,INFINITE);

			for(int i = 0; i < 5; ++i)
				CloseHandle(m_threads[i]);

			delete m_sync;
			m_sync = NULL;
		}

		//CSyncThreads* GetSync() { return m_sync; }
		CSyncProcesses* GetSync() { return m_sync; }

	private:
		HANDLE m_threads[5];
		DWORD m_id[5];
		//CSyncThreads* m_sync;
		CSyncProcesses* m_sync;
	};

	unsigned __stdcall ThreadProc(void* p)
	//DWORD WINAPI ThreadProc(void* p)
	{
		CRandom rand;
		CThreadManager* mgr = (CThreadManager*)p;
		//CSyncThreads* sync = mgr->GetSync();
		CSyncProcesses* sync = mgr->GetSync();

		long nID = rand.GenerateRandomNumber(100,200);
		long nSecs = rand.GenerateRandomNumber(1,10);

		printf("[%ld] started, now waiting %ld seconds before locking...\n",nID,nSecs);
		Sleep(nSecs * 1000);
		if(sync->Lock(SYNC_INFINITE_TIMEOUT))
		{
			printf("[%ld] locked, now waiting %ld seconds before unlocking - internal sync counter = %ld\n",nID,nSecs,sync->GetCounter());
			Sleep(nSecs * 1000);
			sync->Unlock();
			printf("[%ld] unlocked\n",nID);
		}

		return(0L);
	}

	main()
	{
		CThreadManager m;
		m.Start();
		m.Stop();
		return 0;
	}

	Note:
	- se il thread usa funzioni di libreria del runtime C (CRT), allora bisogna crearlo con _beginthreadex() e 
	  non con CreateThread()
	- per terminare il thread si puo' usare il return normale (in entrambi i casi) o _endthreadex nel primo caso 
	  ed ExitThread nel secondo, pero' se si usa la chiamata esplicita, invece di return, usare sempre la funzione 
	  complementaria a quella con cui si e' creato il thread
*/
#ifndef _CSYNC_H
#define _CSYNC_H 1

#include <stdlib.h>
#include "window.h"
#include "win32api.h"
#include "CNodeList.h"
#include "CWindowsVersion.h"

#ifdef _DEBUG
  #define _CSYNC_VERBOSE 1
#endif

#define SYNC_MS_PER_SEC				1000					// ms contenuti in un secondo
#define SYNC_MS_TIMEOUT_INCREMENT	10						// ms per incremento sul calcolo del timeout
#define SYNC_DEFAULT_TIMEOUT		(3 * SYNC_MS_PER_SEC)	// unita' per ms per sec. (3 * 1000 = 3 sec.)

#define SYNC_1_SECS_TIMEOUT			1000					// timeout predefiniti
#define SYNC_2_SECS_TIMEOUT			2000
#define SYNC_3_SECS_TIMEOUT			3000
#define SYNC_4_SECS_TIMEOUT			4000
#define SYNC_5_SECS_TIMEOUT			5000
#define SYNC_10_SECS_TIMEOUT		10000
#define SYNC_60_SECS_TIMEOUT		60000
#define SYNC_INFINITE_TIMEOUT		-1

/*
	CMasterLock()

	Definisce un oggetto astratto da cui derivare le classi CSyncThreads e CSyncProcesses e, piu' in generale, 
	qualsiasi classe che contempli i due metodi previsti (Lock e Unlock).
	Definisce poi la classe CLockGuard (per lock con meccanismo RAII) che richiede come parametro il puntatore 
	a tale oggetto (CMasterLock).
	In tal modo, alla classe CLockGuard potranno essere passati oggetti diversi per il lock, a condizione che 
	siano stati derivati della classe CMasterLock.
	Cio' permette implementare il codice interno di Lock e Unlock in modo specifico per ogni tipo di classe, in 
	modo tale che ogni classe possa realizzare la sua propria logica operativa.

	Esempio:
	- la classe CBase deriva da CMasterLock, definendo Lock e Unlock in un certo modo [...]:

		class CBase : public CMasterLock [...]
		[...]
		BOOL Lock(int nTimeout = 0);
		BOOL Unlock(void);

	- CBase, per accedere alla tabella in modo esclusivo, dentro ogni sua funzione che lo necessiti, dichiara
	  quindi un oggetto CLockGuard, passandogli il proprio puntatore <this>:

		// blocca l'accesso alla tabella (RAII Lock Guard)
		CLockGuard guard(this);
		if(!guard.IsLocked())
		{
			m_nLastError = m_pBerkeleyDB->SetLastError(DB_RETCODE_ELOCKFAILURE);
			Invalidate();
			return(FALSE);
		}

	  tale dichiarazione crea un oggetto RAII (di tipo CLockGuard) per il lock sulla tabella implementando le 
	  specificita' richieste da CBase, dato che CLockGuard, nel suo costruttore, acquisisce il lock chiamando
	  il membro Lock() del puntatore <this> ricevuto, ossia il Lock() di CBase.
*/
class CMasterLock
{
public:
	CMasterLock() {}
	virtual ~CMasterLock() {}

	virtual BOOL Lock(int nTimeout = 0) = 0;
	virtual BOOL Unlock(void) = 0;
};

/*
	CSyncThreads()

	Classe per la sincronizzazione tra threads appartenenti allo stesso processo.

	La sincronizzazione avviene tramite l'utilizzo di una sezione critica e funziona solo con threads che fanno 
	parte dello stesso processo.
	Notare che per default l'entrata a una sezione critica e' bloccante, ma qui usa un meccanismo di timeout per
	non far rimanere appeso in una attesa eterna il thread che richiede il blocco.

	La logica e' la seguente: il processo principale crea un oggetto di tipo CSyncThreads e lo condivide con i suoi 
	threads, affinche' questi si sincronizzino tra loro per accedere in modo mutuamente esclusivo ad una certa risorsa, 
	quindi NON si tratta di un oggetto che viene creato da ogni thread per sincronizzarsi con il processo principale.

	Tenere a mente che, tra threads diversi, un thread NON puo' bloccare la sezione critica se questa e' gia stata
	bloccata da un altro thread, ma uno stesso thread SI puo' bloccare piu' volte la stessa sezione critica (la API
	di Win32 lo permette per evitare problemi di deadlock).

	In conclusione, CSyncThreads e' un oggetto di mutua esclusione condiviso tra piu' thread, usato per serializzare 
	l’accesso a risorse comuni, ed il contatore che usa internamente ha un mero valore informativo.
	La responsabilita' di garantire che nessun thread sia attivo al momento del rilascio dell’oggetto CSyncThreads,
	ricade esclusivamente sul creatore dell'oggetto (il processo principale), che deve sincronizzarsi con i thread 
	tramite meccanismi diversi prima di eliminare l’oggetto.

	Processo (owner)
	|
	|--> Oggetto di sincronizzazione (CSyncThreads)
	|
	|--> Thread A --|
	|--> Thread B --|-- usano l'oggetto
	|--> Thread C --|
	|
	|--> Processo attende la fine dei thread
		 |
		 Distrugge l'oggetto

	Tutti i valori per i timeout vanno espressi in ms (1000 ms = 1 sec).
*/
class CSyncThreads : public CMasterLock
{
public:
	// mantiene la stessa interfaccia di CSyncProcesses
	CSyncThreads(LPCSTR lpcszName = NULL,int nTimeout = 0);
	virtual ~CSyncThreads();

	virtual BOOL	Lock		(int nTimeout = 0);
	virtual BOOL	Unlock		(void);

	void			SetTimeout	(int nTimeout);
	BOOL			SetName		(LPCSTR lpcszName);
	LPCSTR			GetName		(void) const {return(*m_szName ? m_szName : NULL);}

	const HANDLE	GetHandle	(void) const {return(NULL);}

	LONG			GetCounter	(void) {return(m_nCounter);}

private:
	volatile LONG	m_nCounter;
	int				m_nTimeout;
	char			m_szName[_MAX_FILEPATH+1];
	CRITICAL_SECTION m_CriticalSection;
	OSVERSIONTYPE	m_osVersion;
};

/*
	CSyncProcesses()

	Classe per la sincronizzazione tra processi differenti o tra threads appartenenti a processi differenti.

	Puo' essere usata anche per la sincronizzazione tra threads appartenenti allo stesso processo.
	La sincronizzazione viene effettuata tramite l'uso di un mutex, che deve essere nominato obbligatoriamente.
*/
class CSyncProcesses : public CMasterLock
{
public:
	// mantiene la stessa interfaccia di CSyncThreads
	CSyncProcesses(LPCSTR lpcszName = NULL,int nTimeout = 0);
	virtual ~CSyncProcesses();

	virtual BOOL	Lock		(int nTimeout = 0);
	virtual BOOL	Unlock		(void);

	void			SetTimeout	(int nTimeout);
	BOOL			SetName		(LPCSTR lpcszName);
	LPCSTR			GetName		(void) const {return(*m_szName ? m_szName : NULL);}

	const HANDLE	GetHandle	(void) const {return(m_hHandle);}

	LONG			GetCounter	(void) {return(m_nLockCount);}

private:
	HANDLE			m_hHandle;
	volatile LONG	m_nLockCount;
	int				m_nTimeout;
	char			m_szName[_MAX_FILEPATH+1];
	CRITICAL_SECTION m_csLockCount;
};

/*
	CLockGuard
	Implementa il lock tramite un oggetto RAII.

	La filosofia e' bloccare automaticamente l'oggetto nel momento in cui viene dichiarato (ossia
	nel costruttore), e rilasciarlo automaticamente (nel distruttore) quando l'oggetto cessa di 
	esistere.

	Il metodo Unlock() NON entra nella logica RAII e quindi NON deve essere usato, a meno che non 
	si debba sbloccare anticipatamente per evitare deadlocks.
*/
class CLockGuard
{
public:
	// il costruttore acquisisce immediatamente il lock
	CLockGuard(CMasterLock* pObject,int nTimeout = 0) : m_pObject(pObject),m_bLocked(FALSE)
	{
		if(m_pObject)
			m_bLocked = m_pObject->Lock(nTimeout);
	}

	// il distruttore rilascia SEMPRE il lock
	~CLockGuard()
	{
		if(m_bLocked && m_pObject)
			m_pObject->Unlock();
	}

	// NON entra nella filosofia RAII, viene fornito solo per sbloccare anticipatamente
	bool Unlock(void)
	{
		if(m_bLocked && m_pObject)
			m_bLocked = !m_pObject->Unlock();

		return(!m_bLocked);
	}

	// controlla se ha acquisito il lock
	bool IsLocked(void) {return(m_bLocked);}

	/*
	Qui la spiegazione estesa una volta per tutte:

	Il codice dichiara privati due metodi fondamentali: il costruttore di copia e l'operatore di assegnazione in
	modo tale da prevenire copia, assegnazione, passaggio, etc.:

		copia esplicita:
		CLockGuard guard1;
		CLockGuard guard2 = guard1;  // ERRORE: costruttore di copia privato
		CLockGuard guard3(guard1);   // ERRORE: costruttore di copia privato

		assegnazione:
		CLockGuard guard1;
		CLockGuard guard2;
		guard2 = guard1;  // ERRORE: operatore di assegnazione privato

		passaggio per valore a funzioni:
		void funzione(CLockGuard guard)  // ERRORE: richiede costruttore di copia
		{
			// ...
		}
		CLockGuard guard;
		funzione(guard);  // ERRORE

	in modo tale da garantire l'integrita' dell'oggetto. CLockGuard possiede la risorsa (il lock) e la rilascia nel 
	distruttore (RAII).
	Copiare l’oggetto significherebbe duplicare la ownership, causando un doppio unlock o un unlock su una risorsa 
	gia' liberata.
	Spostare (move) l'oggetto nemmeno ha senso perche' il guard deve vivere solo nello scope dove e' stato dichiarato.
	In sintesi, mettere private e senza body il copy-constructor e l’operatore =, elimina la copia a compile-time, 
	evitando che due oggetti condividano la stessa risorsa e garantendo che il RAII funzioni correttamente.
	*/
private:
    CLockGuard(const CLockGuard&);				// costruttore di copia
    CLockGuard& operator=(const CLockGuard&);	// operatore di assegnazione
												// tecnica classica pre-C++11, dichiara come privato senza fornire l'implementazione
private:
    CMasterLock* m_pObject;
    bool m_bLocked;
};

#endif // _CSYNC_H
