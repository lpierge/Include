/*$
	CBerkeleyDB.h
	Classe base per l'interfaccia con la libreria Berkeley DB 2.7.7 (http://www.sleepycat.com).
	Luca Piergentili, 04/11/99
	lpiergentili@yahoo.com

	Qui i commenti vanno letti con la macchina del tempo.

	Gemini, 13/03/26:
	"Analizzando il tuo codice, la struttura e' molto chiara: stai implementando manualmente un sistema di indici 
	secondari (prima che Berkeley DB introducesse il supporto nativo per essi nelle versioni successive alla 3.x)."
	Dice Gemini "PRIMA", lol :) Il '99 fu senza dubbio un anno grandioso!

	Io, 5/6/25:
	Here we go again, twenty-six years after the first time! The funny thing is Oracle currently releases the Berkeley
	DB as "open source"... JAJAJAJAJAJA!!!
	As found on the Internet: "Oracle legally acquired the company and its intellectual property, and has continued to 
	develop and distribute Berkeley DB under its established licensing model. 
	Berkeley DB is a separate product from Oracle's flagship relational database (Oracle Database), serving different 
	use cases, primarily as an embedded, high-performance, key-value store."

	Basically, back in 2013, with the release of the version 6.0, Oracle changed the original SleepyCat license, and
	switched to the AGPLv3 license, a smart move to de-facto restrict and inhibit the use and distribution of the BDB
	source code. This is why I never updated this interface to the latest version of the BerkeleyDB (BDB) software,
	the 2.7.7 was a super compact version and ideal for "old school" pure C Win32 projects, where efficiency and no 
	frills matter more than anything else.

	Following, a piece of art of C/C++ code I wrote back in the days to wrap the original Berkeley DB code and give a 
	friendly (sic!) database interface for C/C++ programs, long life to DB3 and CLIPPER!
	And keep in mind, we are all going to die :)

	Note generali sul disegno:

	BerkeleyDB fornisce l'accesso ai dati nella forma chiave/valore, qui basicamente implementa il concetto di record
	(e quindi quello di tabella e di database) considerando il campo 'valore' come un record e suddividendolo in campi.

	L'interfaccia prevede 4 livelli + 1:

	1º livello: classe CBerkeleyDB per l'interfaccia diretta con la libreria BerkeleyDB
	La classe implementa un sistema a chiave primaria 'nascosta' (una chiave assoluta), che viene poi usata per definire
	gli eventuali indici secondari.

	In tal senso la classe si interfaccia al disegno originale della BerkeleyDB (coppia chiave+valore gestita con sistema
	B-Tree e cursori per stabilire viste sui dati) per introdurre ed implementare il concetto di chiave primaria e di indice
	secondario, permettendo quindi al livello successivo (classe CBase) l'astrazione dei record/campi (presente nei database 
	relazionali) ed una API tipizzata (GetField/PutField per i diversi tipi di dati).
	Tenere a mente che la BerkeleyDB non e' un database relazionale, ma un motore di archiviazione (storage engine) basato 
	sulle coppie chiave+valore, dove NON esistono i concetti di campo, indice, tabella.

	La BerkeleyDB implementa basicamente due tipi di accesso ai dati: 1) B-Tree (db_open con flag DB_BTREE) che e' un albero
	bilanciato ordinato per chiave (ricerche, range scan), normalmente usato per database con chiavi oridnate (alfanumeriche 
	o no), e 2) Hash (db_open con flag DB_HASH) che consiste in una tabella hash sulla chiave per una ricerca rapida su chiavi 
	"non ordinabili".

	L'accesso ai dati viene realizzato con il meccanismo di cursori, che sono dei veri e propri iteratori posizionali sul 
	albero B-Tree e che rappresentano l’equivalente di un record pointer:
	- c_get() permette di spostarsi nel B-Tree (prima, dopo, esatta, range, successivo, ecc.)
	- DB_SET posiziona il cursore esattamente sulla chiave
	- DB_SET_RANGE posiziona il cursore sulla prima chiave >= a quella cercata
	- DB_NEXT / DB_PREV navigano linearmente
	- DB_FIRST / DB_LAST portano agli estremi

	Non essendo quindi le chiavi secondarie native nella BerkeleyDB, la classe CBerkeleyDB le implementa mantenendo piu' files 
	B-Tree, uno per ogni indice secondario, ed inserendo coppie chiave+chiave primaria, per cui:
	- ogni indice secondario ha il proprio DB* e DBC*
	- la key dell’indice e' il valore dell’attributo indicizzato (qui padded)
	- il data dell'indice e' la chiave primaria del record principale
	quindi, una ricerca per indice e': indice -> chiave primaria -> tabella principale -> record completo.

	Le strutture DBT (Database Tuple) sono la colonna portante dell'API:
	typedef struct {
		void *data;   // puntatore al buffer
		u_int32_t size; // dimensione effettiva
		u_int32_t flags;
	} DBT;
	Il chiamante deve riempire data e size prima di un’operazione put o get. Se si usa DB_DBT_MALLOC, e' la BerkeleyDB che
	alloca data e l’applicazione deve poi liberarla con free(). I membri m_Key e m_Data della classe CBerkeleyDB incapsulano 
	esattamente questo concetto.

	Il flusso canonico per l’uso della BerkeleyDB e':
	- db_open() — apre o crea il file (Btree)
	- DB->cursor() — crea un cursore (DBC*)
	- DBcursor->c_get() — naviga/legge
	- DB->put() — inserisce/sostituisce record
	- DBcursor->c_del() — cancella record corrente
	- DBcursor->c_close() — chiude cursore
	- DB->close() — chiude file
	Che e' quanto realizza la classe, gestendo pero' le coppia chiave+valore in modo tale da permettere al livello superiore
	(CBase) di implementare una logica tabellare con gestione automatica degli indici e del buffer per il record.
	In sostanza la classe CBerkeleyDB fornisce al seguente livello dell'interfaccia (CBase) una vista record-oriented di una 
	struttura che, nativamente, e' solo key-value.

	2º livello: classe CBase per la implementazione del concetto di tabella
	La classe CBase si occupa di definire ed implementare la gestione della tabella dal punto di vista logico, ossia mettendo a 
	disposizione i metodi per creare e manipolare una tabella in astratto, non una tabella definita in concreto, e lo fa basandosi 
	su come la classe CBerkeleyDB manipola e gestisce la struttura dati originaria della libreria BerkeleyDB. In poche parole,
	CBase non sa che sotto ci sia un B-Tree dato che vede solo una tabella e CBerkeleyDB non sa cosa significhi record logico dato
	che gestisce solo chiavi e valori binari coerenti. Questa disaccoppiatura e' cio' che potrebbe consentire di evolvere entrambi 
	i lati (ad esempio, sostituiendo BerkeleyDB con LMDB o un backend custom) senza riscrivere la logica tabellare.

	3º livello: classe A) per la definizione della tabella
	La classe di tipo A) definisce in modo concreto la tabella, definendo quindi il record, i campi che lo compongono ed i metodi
	che permettono accedere ai dati sopra definiti (record/campi). Notare che tali metodi solo implementano lo scambio dei dati
	tra questo livello (dove vengono definiti in concreto) ed il sottostante (CBase), dove vengono gestiti in base al modello
	tabellare definito.

	4º livello: classe B) per la definizione delle operazioni logiche
	La classe di tipo B) definisce ed implementa le operazioni logiche sui dati definiti dalla classe del livello precedente, la
	classe A). A questo livello si realizzano le operazioni richieste dal programma utente, realizzando in altre parole, la logica
	business, dove i dati si manipolano a livello concettuale ("inserimento di un nuovo cliente per ogni contratto...", etc.).

	livello finale: il programma utente
	L'applicativo dove si definisce la logica business e si implementano le operazioni necessarie per realizzarla (definizione dei
	dati e del codice che opera su di essi).

	In questo schema non viene incluso il layer per ovviare al (buggato) modello transazionale originale della libreria, tale
	modello viene bypassato tramite un meccanismo specifico di apertura/rispristino/backup delle tabelle.
	Vedi il codice nei files CBdbtf.cpp/.h.

	+---------------------------------------------------------------+
	|                      LIVELLO APPLICATIVO                      |
	|			classe A), classe B), programma utente				|
	|---------------------------------------------------------------|
	|  Classi utente (es. CClienti, CArticoli, COrdini, ecc.)       |
	|   - Definiscono tabelle concrete                              |
	|   - Chiamano i metodi logici di CBase                         |
	|   - Non conoscono l’esistenza di BerkeleyDB                   |
	+-------------------------------|-------------------------------+
									|
									| usa
									|
	+-------------------------------v-------------------------------+
	|                            CBase                              |
	|---------------------------------------------------------------|
	|  - Gestione tabella logica (astratta)                         |
	|  - Mantiene:                                                  |
	|      * Definizione dei campi (nome, tipo, size, offset)       |
	|      * Definizione indici (secondari, univoci, ecc.)          |
	|      * Buffer record attuale (riga corrente)                  |
	|  - Metodi:                                                    |
	|      * CreateTable(), OpenTable(), CloseTable()               |
	|      * Read(), Write(), Update(), Delete(), Replace()         |
	|      * GetField(), PutField(), GetPrimaryKey(), ecc.          |
	|  - Implementa controllo logico e formattazione dati           |
	|  - Delega a CBerkeleyDB l’accesso fisico                      |
	+-------------------------------|-------------------------------+
									|
									| delega
									|
	+-------------------------------v-------------------------------+
	|                        CBerkeleyDB                            |
	|---------------------------------------------------------------|
	|  - Gestione accesso fisico via API BerkeleyDB 2.7.7           |
	|  - Mantiene:                                                  |
	|      * Handle DB* per tabella principale                      |
	|      * Handle DB* / DBC* per indici secondari                 |
	|      * Strutture DBT per key/data                             |
	|  - Metodi:                                                    |
	|      * Open(), Close()                                        |
	|      * Insert(), Replace(), Delete()                          |
	|      * GetByPrimary(), GetBySecondary(), Next(), Prev()       |
	|  - Realizza mapping diretto tra buffer record e coppie K/V    |
	|  - Cura la sincronizzazione indici ? dati                     |
	+-------------------------------|-------------------------------+
									|
									| chiama API nativa
									|
	+-------------------------------v-------------------------------+
	|                        Berkeley DB 2.7.7                      |
	|---------------------------------------------------------------|
	|  - DB_open() ? file .db (B-Tree)                              |
	|  - DB_put(), DB_get(), DB_del()                               |
	|  - DBcursor->c_get(), c_put(), c_del()                        |
	|  - Implementa B-Tree ordinato, gestione cursori, locking, I/O |
	|  - Nessuna nozione di schema o tipi                           |
	+---------------------------------------------------------------+
*/
#ifndef _CBERKELEYDB_H
#define _CBERKELEYDB_H 1

#include <string.h>
#include <stdbool.h>
#if defined(_WINDOWS)
  #include "window.h"
#endif
#include "typedef.h"
#include "db.h"

// crea la referenza alla DLL
#ifdef _DEBUG
  #pragma comment(lib,"BerkeleyDB.d.lib")
#else
  #pragma comment(lib,"BerkeleyDB.lib")
#endif
#ifdef PRAGMA_MESSAGE_VERBOSE
  #pragma message("\t\t\t" __FILE__ "(" STR(__LINE__) "): automatically linking with BerkeleyDB")
#endif

// generiche
#define MAX_PRIMARYKEY_SIZE	10					// dimensione massima della chiave primaria (interna) = unsigned long (4,294,967,295)
#define MAX_KEYSIZE			1024				// dimensione massima della chiave per gli indici secondari
#define MAX_FIELDSIZE		MAX_KEYSIZE			// dimensione massima del campo del record
#define MAX_FIELDCOUNT		128					// numero massimo di campi per la tabella
#define MAX_FIELDNAME		64					// dimensione massima per il nome del campo della tabella
#define MAX_ERRORCODE_STRING 128				// stringa per messaggio d'errore

/*
	ROW
	struttura per la definizione del campo del record
*/
struct ROW {
	int			num;							// numero progressivo del campo nel record (a base 0)
	int			ofs;							// offset (in bytes) del campo nel record (a base 0)
	char*		name;							// nome del campo
	char		type;							// tipo del campo
	int			size;							// dimensione del campo
	int			dec;							// decimali del campo
	char*		value;							// contenuto del campo (il buffer relativo e' l'istanza della struct per il record)
	unsigned long flags;						// flag per operazioni (filtri) sul campo
};

/*
	INDEX
	struttura per la definizione dell'indice
*/
struct INDEX {
	char		filename[_MAX_PATH+1];			// nome file indice
	char*		name;							// nome indice
	char*		fieldname;						// nome del campo della tabella utilizzato come chiave
	int			fieldnum;						// numero del campo della tabella utilizzato come chiave (a base 0)
};

/*
	TABLE_STAT
	struttura per lo status della tabella
*/
struct TABLE_STAT {
	int			bof;							// flag per inizio file
	int			eof;							// flag per fine file
	int			errnum;							// codice numerico ultimo errore
	char		errstr[MAX_ERRORCODE_STRING+1];	// descrizione ultimo errore
};

/*
	TABLE
	struttura per la definizione della tabella
*/
struct TABLE {
	char		filename[_MAX_PATH+1];			// nome tabella
	int			totfield;						// numero totale di campi
	ROW*		row;							// array per la definizione del record, da allocare a run-time
	int			totindex;						// numero totale indici
	INDEX*		index;							// array per la definizione degli indici, da allocare a run-time
	TABLE_STAT	stat;							// status
};

/*
	DATABASE
	struttura per la definizione del database
*/
struct DATABASE {
	TABLE		table;							// definizione della tabella
	unsigned long flags;						// flags per porcherie varie
};

/*
	DB_...
	codici di ritorno:
	1000		info
	2000		status
	>=3000	errori

	mantenere allineato con l'array nel .cpp
*/
#define DB_NO_ERROR						0

// 1000 (info)
#define DB_OK							1000

// 2000 (status)
#define DB_RETCODE_STATUS_BASE			2000 // base per test
#define DB_RETCODE_NOTFOUND				DB_RETCODE_STATUS_BASE
#define DB_RETCODE_BOF					DB_RETCODE_STATUS_BASE+1
#define DB_RETCODE_EOF					DB_RETCODE_STATUS_BASE+2

// 3000 (errori)
#define DB_RETCODE_ERROR_BASE			3000 // base per test

#define DB_RETCODE_ECREATETABLE			DB_RETCODE_ERROR_BASE	+ 1
#define DB_RETCODE_EOPENTABLE			DB_RETCODE_ERROR_BASE	+ 2
#define DB_RETCODE_EOPENCURSOR			DB_RETCODE_ERROR_BASE	+ 3
#define DB_RETCODE_EOPENINDEX			DB_RETCODE_ERROR_BASE	+ 4
#define DB_RETCODE_EOPENIDXCURSOR		DB_RETCODE_ERROR_BASE	+ 5
#define DB_RETCODE_EOPENDEFINITION		DB_RETCODE_ERROR_BASE	+ 6

#define DB_RETCODE_EALREADYOPEN			DB_RETCODE_ERROR_BASE	+ 7
#define DB_RETCODE_ETABLENOTOPEN		DB_RETCODE_ERROR_BASE	+ 8
#define DB_RETCODE_EINDEXREQUIRED		DB_RETCODE_ERROR_BASE	+ 9

#define DB_RETCODE_ECLOSETABLE			DB_RETCODE_ERROR_BASE	+ 10
#define DB_RETCODE_ECLOSECURSOR			DB_RETCODE_ERROR_BASE	+ 11
#define DB_RETCODE_ECLOSEINDEX			DB_RETCODE_ERROR_BASE	+ 12
#define DB_RETCODE_ECLOSEIDXCURSOR		DB_RETCODE_ERROR_BASE	+ 13

#define DB_RETCODE_EINVALIDOBJECT		DB_RETCODE_ERROR_BASE	+ 14
#define DB_RETCODE_EINVALIDHANDLE		DB_RETCODE_ERROR_BASE	+ 15
#define DB_RETCODE_EINVALIDPARAM		DB_RETCODE_ERROR_BASE	+ 16
#define DB_RETCODE_EINVALIDRESOURCE		DB_RETCODE_ERROR_BASE	+ 17
#define DB_RETCODE_EINVALIDCURSOR		DB_RETCODE_ERROR_BASE	+ 18
#define DB_RETCODE_EINVALIDINDEX		DB_RETCODE_ERROR_BASE	+ 19

#define DB_RETCODE_EINVALIDFIELDNUMBER	DB_RETCODE_ERROR_BASE	+ 20
#define DB_RETCODE_EINVALIDFIELDSIZE	DB_RETCODE_ERROR_BASE	+ 21
#define DB_RETCODE_EINVALIDFIELDTYPE	DB_RETCODE_ERROR_BASE	+ 22

#define DB_RETCODE_EINVALIDPRIMARYKEYSIZE DB_RETCODE_ERROR_BASE	+ 23

#define DB_RETCODE_EOUTOFMEMORY			DB_RETCODE_ERROR_BASE	+ 24

#define DB_RETCODE_ELOCKFAILURE			DB_RETCODE_ERROR_BASE	+ 25
#define DB_RETCODE_EALLOCFAILURE		DB_RETCODE_ERROR_BASE	+ 26
#define DB_RETCODE_ESYNCFAILURE			DB_RETCODE_ERROR_BASE	+ 27

#define DB_RETCODE_ESECONDARYKEYNOTFOUND DB_RETCODE_ERROR_BASE	+ 28

// >3000 (errori)
#define DB_RETCODE_ERROR				4000
#define DB_RETCODE_UNKNOWNERROR			6666

// da usare per il posizionamento (se specificato al posto di DB_FIRST|DB_NEXT|DB_PREV|DB_LAST, usa DB_SET)
#define DB_SEARCH 0

/*
	CBerkeleyDB
*/
class CBerkeleyDB
{
public:
	CBerkeleyDB();
	virtual ~CBerkeleyDB();

	int			Open				(u_int32_t = DB_CREATE);
	inline int	IsOpen				(void) const			{return(m_pDbHandle!=NULL);}
	int			Create				(void);
	int			Close				(void);

	int			Insert				(void);
	int			Delete				(void);
	int			Replace				(int nFieldNum,const char* pCurrentValue,const char* pReplaceValue);

	int			Reindex				(int nIndex = -1);
	int			CheckIndex			(int nIndex = -1);

	inline void	ResetCursor			(void)					{m_pCurrentCursor = m_pDbCursor; m_nCurrentCursorNumber = -1;}
	int			SetCursor			(int);
	inline DBC*	GetCursor			(void) const			{return(m_pCurrentCursor);}
	inline int	GetCursorNumber		(void) const			{return(m_nCurrentCursorNumber);}

	char*		GetPrimaryKeyData	(void) const;
	inline int	GetPrimaryKeySize	(void) const			{return(m_Database.table.row[0].size);}
	char*		GetSecondaryKeyData	(int nFieldNum) const;
	int			GetSecondaryKeySize	(int nFieldNum) const;
	int			GetSecondaryKeyField(int nIndex) const;
	
	int			Get					(u_int32_t db_goto_flag/*DB_SEARCH(usa DB_SET)|DB_FIRST|DB_NEXT|DB_PREV|DB_LAST*/,u_int32_t db_set_flag = DB_SET/*DB_SET|DB_SET_RANGE*/);
	inline int	GetFirst			(void)					{return(Get(DB_FIRST,DB_SET));}
	inline int	GetNext				(void)					{return(Get(DB_NEXT,DB_SEARCH));}
	inline int	GetPrev				(void)					{return(Get(DB_PREV,DB_SEARCH));}
	inline int	GetLast				(void)					{return(Get(DB_LAST,DB_SET));}

	int			GetRecordNumber		(db_recno_t& ulRecNo);
	int			SetRecordNumber		(db_recno_t ulRecNo);

    unsigned long GetRecordCount	(void);
    unsigned long GetRecordCountByCursor(void);

	int			GetPrimaryKey		(const char* pPrimaryKey);
	int			GetSecondaryKey		(int nIndex,u_int32_t db_set_flag = DB_SET/*DB_SET|DB_SET_RANGE*/);
	void		PutKey				(char* pValue,int nIndex);

	bool		GetField			(int nFieldNum,char* pBuffer,int nSize);
	void		PutField			(int nFieldNum,const char* pValue);

	inline DATABASE* GetDatabase	(void)					{return(&m_Database);}

	int			SetLastError		(int);
	void		ResetLastError		(void);
	int			GetLastError		(void);
	const char*	GetLastErrorString	(void);

#if defined(_WINDOWS)
	inline void	ShowErrors			(bool bFlag)			{m_bShowErrors = bFlag;}
#endif

	inline void	ClearRecord			(void)					{ResetRecord();}
	int			SyncDatabase		(bool bFlush = false);
	int			SyncIndex			(int nIndex,bool bFlush = false);
	int			SyncIndexAll		(bool bFlush = false);

private:
	int			SyncDb				(bool bFlush = false);
	int			SyncIdx				(int nIndex,bool bFlush = false);
	inline int	FlushDb				(void)					{return(SyncDb(true));}
	inline int	FlushIdx			(int nIndex)			{return(SyncIdx(nIndex,true));}

	inline void	ResetKeyPair		(void)					{memset(&m_Key,'\0',sizeof(m_Key));}
	inline void	ResetDataPair		(void)					{memset(&m_Data,'\0',sizeof(m_Data));}

	void		ResetPair			(void);
	void		ResetRecord			(void);
	void		BlankRecord			(void);
	void		MemoryToRecord		(void);
	void		RecordToMemory		(void);

	DATABASE	m_Database;									// struct per la tabella
	char*		m_pDataBuffer;								// buffer per il campo data.data per i dati (campi del record)
	int			m_nDataBufferSize;							// dimensione
	int			m_nRecordSize;								// dimensione del record (somma dei campi)
	DB_INFO		m_dbinfo;									// (db)
	DBT			m_Key,m_Data;								// coppia chiave/valore (db)
	DB*			m_pDbHandle;								// handle della tabella (db)
	DBC*		m_pDbCursor;								// cursore per la vista sulla tabella (db)
	DBC*		m_pCurrentCursor;							// cursore corrente
	int			m_nCurrentCursorNumber;						// indice del cursore corrente
	DB**		m_pIdxHandleArray;							// array per gli handles degli indici
	DBC**		m_pIdxCursorArray;							// array per i cursori degli indici
	char		m_szLastPrimaryKey[MAX_PRIMARYKEY_SIZE+1];	// buffer per la generazione della chiave primaria
#if defined(_WINDOWS)
	bool		m_bShowErrors;								// flag per visualizzazione errori
#endif
	char*		m_szPutKeyBuffer;							// per eliminare i vecchi buffer statici
    char*		m_szGetFieldBuffer;							// per eliminare i vecchi buffer statici
};

#endif // _CBERKELEYDB_H
