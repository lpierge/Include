/*
	CBerkeleyDBTransactionsFallback.h
	Classi per sostituire il gestore originale della libreria BerkeleyDB per le transazioni (BDBTF).
	Luca Piergentili, Oct '25

	Nota: per disabilitare il warning del compilatore, andare su proprieta' del progetto ed impostare 
	l'opzione: C/C++ -> Code Generation -> Enable C++ Exceptions -> "Yes with SEH Exceptions (/EHa)"
*/
#ifndef _CBDBTF_H
#define _CBDBTF_H 1

#include "window.h"
#include <process.h>
#include <eh.h>
#include "CFindFile.h"

// timeout per apertura tabella da parte del thread, es. (1 * 60 * 1000) = 1 minuto, impostare a 5 min per debug
#define BDBTF_TIMEOUT (5 * 60 * 1000)

/*
	SehException

	Classe per gestire l'eccezione C++ personalizzata per le violazioni SEH.
*/
class SehException
{
public:
    unsigned int nCode;
    SehException(unsigned int code) : nCode(code) {}
};

/*
	SehTranslator()

	Traduce le eccezioni SEH in eccezioni C++.
*/
void SehTranslator(unsigned int u,EXCEPTION_POINTERS* pExp);

/*
	CDBServiceBase

	Classe factory astratta da cui far derivare tutti gli oggetti del livello 4 dell'interfaccia per la BerkeleyDB, ossia le classi 
	di tipo B che si occupano di definire le operazioni logiche sulla tabella, per poterle gestire con il meccanismo BDBTF:

	1) L'obiettivo e' rendere l'insieme delle funzioni BDBTF generico e riutilizzabile, in modo tale che non debba essere modificato 
	o ricompilato ogni volta che si aggiunge una nuova tabella nell'applicazione principale.

	2) L'architettura dell'interfaccia per la BerkeleyDB e' basata sulla composizione, cosa che impedisce al thread BDBTF (usato per 
	la creazione delle tabelle):
	- chiamare IsValid() sull'oggetto (tabella)
	- chiamare delete sull'oggetto (tabella), dato che il thread non conosce il tipo per la tabella (la classe B concreta che la 
	  implementa).

	3) La soluzione consiste nel 'fattorizzare' la classe di tipo B, ossia nell'introdurre il concetto di polimorfismo.

	3a) Polimorfismo -> CDBServiceBase
	Viene introdotta una nuova classe base astratta, CDBServiceBase, da cui derivare le classi di tipo B, e che definisce le condizioni
	minime che ogni oggetto che sia una classe di tipo B deve rispettare nel contesto BDBTF, ossia:
	a) un distruttore virtuale che garantisca che la delete sull'oggetto tabella al interno del codice generico del thread BDBTF 
	invochi il distruttore corretto della classe B concreta
	b) un membro virtual bool IsValid() = 0; che il thread BDBTF deve chiamare per verificare l'esito della creazione della tabella,
	nonostante non conosca l'implementazione concreta della classe B.

	3b) Factory(zzazione) -> funzione applicativo
	Invece di obbligare il thread BDBTF a conoscere l'oggetto concreto della classe B, si interviene al livello dell'applicazione
	principale, definendo una funzione factory che conosce il tipo concreto di tabella e che sa come crearla.
	main() passa quindi al thread BDBTF il puntatore a questa funzione factory in modo che tale che il thread possa chiamarla e
	creare quindi (indirettamente) l'oggetto per la tabella senza conoscerne il tipo concreto.
	In altre parole, al thread BDBTF non si passa piu' il puntatore a un oggetto concreto per la tabella, ma si passa il puntatore
	ad una funzione esterna che si conosce il tipo concreto dell'oggetto tabella perche' forma parte dello stesso livello del
	applicativo che la definisce.

	4) Il risultato di tutto cio e' che ora il codice del thread BDBTF opera solo sul puntatore generico CDBServiceBase*. Il thread 
	esegue il factory (la funzione dell'applicativo) e riceve l'oggetto corretto. Se l'apertura fallisce, il thread esegue la delete 
	sul puntatore base che, grazie al distruttore virtuale, chiama in cascata il distruttore della classe concreta, rilasciando la 
	correttamente la memoria.
*/
class CDBServiceBase
{
public:
	// definisce come virtuali i membri che devono essere chiamati dal thread per la verifica/eliminazione dell'oggetto tabella

    // abilita la delete polimorfica
    virtual ~CDBServiceBase() {}

    // per il controllo della validità dell'oggetto, usare il qualificatore 'override' nella derivata
    virtual bool IsValid() = 0; 
};

/*
	PfnCreateService

	tipo per il puntatore alla funzione factory da passare al thread BDBTF per creare la tabella specifica
*/
typedef CDBServiceBase* (*PfnCreateService)(const char* pTableName,const char* pPath);

/*
	PfnCheckService

	tipo per il puntatore alla funzione factory da passare al thread BDBTF per controllare la tabella specifica
*/
typedef BOOL (*PfnCheckService)(CDBServiceBase* pDB);

/*
	BDBTF_FACTORY_ITEM

	struttura per passare al thread BDBTF i dati relativi alla tabella
*/
typedef struct _bdbtf_factory_item {
	CDBServiceBase** ppDB;					// ptr alla classe factory
	PfnCreateService pfnCreate;				// ptr alla funzione factory
	PfnCheckService pfnCheck;
	char szTableName[64];					// nome della tabella
	char szProductionDir[_MAX_FILEPATH+1];	// directory di produzione per la tabella
	char szBackupDir[_MAX_FILEPATH+1];		// directory di backup per la tabella
} BDBTF_FACTORY_ITEM;

/*
	BDBTF_THREAD_PARAMS
	
	struttura per passare al thread BDBTF l'insieme dei parametri
*/
typedef struct _bdbtf_thread_params {
	DWORD dwTimeout;						// timeout per l'attesa del thread
	int nResult;							// codice di ritorno per la creazione/apertura della tabella
	BDBTF_FACTORY_ITEM* bdbtfArray;			// array per le coppie classe/funzione factory
	int nArrayCount;						// numero elementi dell'array
} BDBTF_THREAD_PARAMS;

/*
	CBerkeleyDBTransactionsFallback

	Classe per il meccanismo alternativo alle transazioni della libreria BerkeleyDB.
	Prova la apertura delle tabella tramite un thread controllato via timeout. Se il thread non ritorna
	nell'intervallo di tempo stabilito, assume che le funzioni di libreria della BerkeleyDB sono rimaste
	appese perche' le tabelle si sono corrotte. In tal caso ripristina in produzione la copia di backup
	e la applicazione deve reiniziare per accedere alla ultima versione corretta dei dati.
	La applicazione deve chiamare la Startup() all'inizio per la apertura delle tabella e la Backup()
	alla fine per effettuare il backup dei dati.
	La sequenza basica normale e':
	apertura tabelle -> modifica dati -> backup dati -> fine
	nel caso in cui i dati siano corrotti, la sequenza diventa:
	apertura tabelle -> fallimento -> ripristino backup -> reinizio applicazione per accesso a dati
	I dati per la Starup() vanno passati tramite le due strutture BDBTF_FACTORY_ITEM e BDBTF_THREAD_PARAMS
*/
class CBerkeleyDBTransactionsFallback
{
public:
	int		Startup	(BDBTF_THREAD_PARAMS* pParams);
	bool	Backup	(const char* pProductionDir,const char* pBackupDir);

private:
	bool CopyDatabaseFiles(const char* pSourceDir,const char* pDestDir);

	// per lanciare un thread con _beginthreadex(), la funzione per il thread deve essere di tipo statico, 
	// ma una funzione statica non puo' accedere ai membri della classe anche se viene dichiarata nella 
	// stessa classe
	static unsigned __stdcall OpenDatabaseFiles(LPVOID lpParam);

	// quindi la versione statica del codice del thread di cui sopra, solo viene usata come wrapper per il 
	// lancio di questa versione qui sotto che non puo' essere lanciata con _beginthreadex() ma che si puo' 
	// accedere ai membri della classe
	// il lancio di questa da parte della statica, che non puo' invocare i membri della classe, avveiene
	// con un trucco: passando alla statica il puntatore this della classe, presente nella struttura pParams,
	// per raggirare la restrizione
	unsigned int OpenDatabaseFiles(BDBTF_THREAD_PARAMS* pParams);
};

#endif // _CBDBTF_H
