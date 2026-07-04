/*$
	CBase.h
	Classe derivata per l'interfaccia con il database.
	Definisce l'oggetto per il database (CBase) usando la classe che si interfaccia con la libreria (CBerkeleyDB).
	Luca Piergentili, 04/11/99
	lpiergentili@yahoo.com

	Vedi le note in CBerkleyDB.h

	"Perfection is reached at the point of collapse." (Shawn Lane)
*/
#ifndef _CBASE_H
#define _CBASE_H 1

#include <stdbool.h>
#include "typedef.h"
#if defined(_WINDOWS)
  #include "window.h"
#endif
#include "CSync.h"
#include "CDateTime.h"
#include "CBerkeleyDB.h"

/*
	CBASE_FLAG_...
	flags per filtri sui campi
*/
#ifdef _USE_FIELD_PICTURES
  #define CBASE_FLAG_NONE			0x0000	// nessun azione
  #define CBASE_FLAG_TOUPPER		0x0002	// a -> A
  #define CBASE_FLAG_TOLOWER		0x0004	// A -> a
  #define CBASE_FLAG_NUMBER			0x0008	// 0-9
  #define CBASE_FLAG_CHAR			0x0010	// a-z
  #define CBASE_FLAG_PUNCT			0x0020	// .,;:?¿!¡&%$#@(){}[]-+*/
  #define CBASE_FLAG_USERDEFINED	0x0040	// definito dall'utente
  #define CBASE_MAX_NUMBER_PICTURE	32		// dimensioni dei buffer per la picture
  #define CBASE_MAX_CHAR_PICTURE	128
  #define CBASE_MAX_PUNCT_PICTURE	128
  #define CBASE_MAX_USER_PICTURE	128
#endif

/*
	CBASE_TABLE
	struttura per la definizione della tabella
*/
struct CBASE_TABLE {
	char*	field;		// nome campo
	char	type;		// tipo
	int		size;		// dimensione
	int		dec;		// decimali
};

/*
	CBASE_INDEX
	struttura per la definizione dell'indice
*/
struct CBASE_INDEX {
	char*	file;		// nome file
	char*	name;		// nome indice
	char*	field;		// nome campo associato
};

/*
	FIELD_DELIM
	carattere usato per delimitare i campi all'interno del record
	(quando il record viene restituito come stringa)
*/
#define FIELD_DELIM '|'

/*
	FIELD_TYPE
	union per il valore del campo
*/
union FIELD_TYPE {
	bool		boolean;
	short int	shortint;
	int			integer;
	long		longint;
	unsigned long ulongint;
	double		realnum;
	char		chr;
	char		buffer[MAX_FIELDSIZE+1];
};

/*
	FIELD
	struttura per la definizione del campo
*/
struct FIELD {
	char		type;
	int			size;
	int			dec;
	FIELD_TYPE	field;
};

/*
	FIELD_...SIZE
	stabiliscono le dimensioni dei campi a seconda del tipo

	Note:
	- MAX_FIELDSIZE, definito in CBerkeleyDB.h, stabilisce la dimensione massima di una stringa
	- le date vengono immagazzinate nel formato ANSI_SHORT, precedute da '~' o '+' a seconda se A.C. o B.C.
	- i boolean vengono immagazzinati come T/F
*/
#define FIELD_CHAR_MINSIZE			1
#define FIELD_CHAR_MAXSIZE			MAX_FIELDSIZE

#define FIELD_TIME_MINSIZE			6
#define FIELD_TIME_MAXSIZE			FIELD_TIME_MINSIZE
#define FIELD_DATE_MINSIZE			1+8
#define FIELD_DATE_MAXSIZE			FIELD_DATE_MINSIZE

#define FIELD_BOOLEAN_MINSIZE		1
#define FIELD_BOOLEAN_MAXSIZE		FIELD_BOOLEAN_MINSIZE

#define FIELD_SHORTINT_MINSIZE		5
#define FIELD_SHORTINT_MAXSIZE		FIELD_SHORTINT_MINSIZE
#define FIELD_INT_MINSIZE			10
#define FIELD_INT_MAXSIZE			FIELD_INT_MINSIZE
#define FIELD_LONG_MINSIZE			10
#define FIELD_LONG_MAXSIZE			FIELD_LONG_MINSIZE
#define FIELD_UNSIGNEDLONG_MINSIZE	10
#define FIELD_UNSIGNEDLONG_MAXSIZE	FIELD_UNSIGNEDLONG_MINSIZE
#define FIELD_REAL_MINDECS			0
#define FIELD_REAL_MAXDECS			6
#define FIELD_REAL_MINSIZE			10
#define FIELD_REAL_MAXSIZE			(FIELD_REAL_MINSIZE + 1 + FIELD_REAL_MAXDECS)

/*
	CBase
*/
class CBase : public CMasterLock
{
public:
	// tipi numerici
	union NUMBER {
		short int		shortint;	// intero corto
		int				integer;	// intero
		long			longint;	// lungo
		unsigned long	ulongint;	// esageratamente lungo
		double			realnum;	// virgola mobile
	};
	enum NUMTYPE {
		shortint_type,
		integer_type,
		longint_type,
		ulongint_type,
		realnum_type
	};

	CBase();
	virtual ~CBase();

	// verificano la validita' dell'oggetto interno usato per la tabella (CBerkeleyDB)
	// la validita' viene impostata dal costruttore e deve essere verificata dai vari membri durante l'esecuzione
	// la invalidazione e' una misura drastica per prevenire l'accesso all'oggetto: vedi sotto Invalidate() nella
	// sezione protected
	inline bool		IsValid				(void) const {return(m_bIsValid);}

	// i codice d'errore restituiti sono quelli definiti in CBerkeleyDB.h
	// l'errore lo restituisce il membro relativo della classe CBerkeleyDB o un wrapper (+ ...) se l'oggetto non e' valido (vedi sopra)
	inline int		GetLastError		(void)  {return(IsValid() ? m_pBerkeleyDB->GetLastError()/*m_nLastError?*/	: DB_RETCODE_EINVALIDOBJECT);}
	inline const char* GetLastErrorString(void) {return(IsValid() ? m_pBerkeleyDB->GetLastErrorString()				: "DB_RETCODE_EINVALIDOBJECT");}

	// lock
	BOOL			Lock				(int nTimeout = 0);
	BOOL			Unlock				(void);

	// database
	bool			Create				(const char* pTableName,CBASE_TABLE* pTable = NULL,CBASE_INDEX* pIdx = NULL);
	bool			Open				(void);
	bool			Zap					(bool bLock = true);
	bool			Close				(void);

	// import/export
	bool			Dump				(const char* pFileName,bool bDumpPrimaryKey = FALSE,char cSeparator = 0,bool bTrimSpaces = FALSE);
	bool			Load				(const char* pFileName);

	// operazioni
	bool			Insert				(bool bLock = true);
	bool			Delete				(bool bLock = true);

	// vanno per numero campo e nome campo
	bool			Replace				(int nFieldNum,const char* pOldValue,const char* pNewValue,bool bLock = true);
	bool			Replace				(const char* pFieldName,const char* pOldValue,const char* pNewValue,bool bLock = true);
	bool			Replace				(int nFieldNum,bool bOldValue,bool bNewValue,bool bLock = true);
	bool			Replace				(const char* pFieldName,bool bOldValue,bool bNewValue,bool bLock = true);
	bool			Replace				(int nFieldNum,CDateTime& pOldValue,CDateTime& pNewValue,bool bLock = true);
	bool			Replace				(const char* pFieldName,CDateTime& pOldValue,CDateTime& pNewValue,bool bLock = true);
	bool			Replace				(int nFieldNum,short int nOldValue,short int nNewValue,bool bLock = true);
	bool			Replace				(const char* pFieldName,short int nOldValue,short int nNewValue,bool bLock = true);
	bool			Replace				(int nFieldNum,int nOldValue,int nNewValue,bool bLock = true);
	bool			Replace				(const char* pFieldName,int nOldValue,int nNewValue,bool bLock = true);
	bool			Replace				(int nFieldNum,long nOldValue,long nNewValue,bool bLock = true);
	bool			Replace				(const char* pFieldName,long nOldValue,long nNewValue,bool bLock = true);
	bool			Replace				(int nFieldNum,unsigned long nOldValue,unsigned long nNewValue,bool bLock = true);
	bool			Replace				(const char* pFieldName,unsigned long nOldValue,unsigned long nNewValue,bool bLock = true);
	bool			Replace				(int nFieldNum,double nOldValue,double nNewValue,bool bLock = true);
	bool			Replace				(const char* pFieldName,double nOldValue,double nNewValue,bool bLock = true);

	// posizionamento
	bool			Bof					(void) const;
	bool			Eof					(void) const;

	bool			GoTop				(int nIndex = -1);
	bool			GoBottom			(int nIndex = -1);

	bool			GetCurrent			(int nIndex = -1);
	bool			GetFirst			(int nIndex = -1);
	bool			GetNext				(int nIndex = -1);
	bool			GetPrev				(int nIndex = -1);
	bool			GetLast				(int nIndex = -1);

	ULONG			GetRecordCount		(void);
	ULONG			GetRecordCountByCursor(void);
	
	// ricerca
	inline bool		SetSoftSeek			(bool bFlag) {bool bPrevious = m_bSoftseek; m_bSoftseek = bFlag; return(bPrevious);}
	inline bool		GetSoftSeek			(void) const {return(m_bSoftseek);}

	bool			Seek				(const char* pValue,int nIndex = -1);
	bool			Seek				(short int nValue,int nIndex = -1);
	bool			Seek				(int nValue,int nIndex = -1);
	bool			Seek				(long nValue,int nIndex = -1);
	bool			Seek				(unsigned long nValue,int nIndex = -1);
	bool			Seek				(double nValue,int nIndex = -1);

	bool			SeekPrimaryKey		(const char* pPrimaryKey);
	
	bool			Find				(const char* pValue);
	
	// indici
	int				GetIndex			(void);
	int				SetIndex			(int nIndex);
	void			ResetIndex			(void);

	bool			Reindex				(int nIndex = -1);
	bool			CheckIndex			(int nIndex = -1);

	int				GetIndexCount		(void) const;
	const char*		GetIndexNames		(int& nIterator);
	int				GetIndexNumberByName(const char* pIndexName);
	const char*		GetIndexNameByNumber(int nIndex);
	
	int				GetFieldNumberOfIndex(int nIndex);
	const char*		GetFieldNameOfIndex	(int nIndex);

	// campi
	int				GetFieldCount		(void);
	const char*		GetFieldNames		(int& nIterator);
	int				GetFieldNumberByName(const char* pFieldName);
	const char*		GetFieldNameByNumber(int nFieldNum);

	bool			GetFieldInfoByNumber(int nFieldNum,char* pFieldName = NULL,int nSize = 0,int* cFieldType = NULL,int* nFieldSize = NULL,int* nFieldDec = NULL,int* nFieldNumber = NULL,int* nIndexNum = NULL);
	bool			GetFieldInfoByName	(const char* pFieldName,int* cFieldType = NULL,int* nFieldSize = NULL,int* nFieldDec = NULL,int* nFieldNumber = NULL,int* nIndexNum = NULL);

	const char*		GetPrimaryKeyValue	(void) const {return(m_szPrimaryKeyValue);}

	// per ricavare il valore del campo interno (relativo al campo corrente della tabella) in base al progressivo numerico
	const char*		GetFieldRaw			(int			nFieldNum,	char* pBuffer,int nSize);
	const char*		GetFieldRaw			(const char*	pFieldName,	char* pBuffer,int nSize);

	FIELD*			GetFieldUnion		(int			nFieldNum,	FIELD* f);
	FIELD*			GetFieldUnion		(const char*	pFieldName,	FIELD* f);
	
	char*			GetField			(int			nFieldNum,	char* pBuffer,int nSize,bool bTrim = FALSE);
	char*			GetField			(const char*	pFieldName,	char* pBuffer,int nSize,bool bTrim = FALSE);
	bool			GetField			(int			nFieldNum,	bool& bValue);
	bool			GetField			(const char*	pFieldName,	bool& bValue);
	CDateTime&		GetField			(int			nFieldNum,	CDateTime& pDateTime);
	CDateTime&		GetField			(const char*	pFieldName,	CDateTime& pDateTime);
	short int		GetField			(int			nFieldNum,	short int& nValue);
	short int		GetField			(const char*	pFieldName,	short int& nValue);
	int				GetField			(int			nFieldNum,	int& nValue);
	int				GetField			(const char*	pFieldName,	int& nValue);
	long			GetField			(int			nFieldNum,	long& nValue);
	long			GetField			(const char*	pFieldName,	long& nValue);
	unsigned long	GetField			(int			nFieldNum,	unsigned long& nValue);
	unsigned long	GetField			(const char*	pFieldName,	unsigned long& nValue);
	double			GetField			(int			nFieldNum,	double& nValue);
	double			GetField			(const char*	pFieldName,	double& nValue);

	void			PutField			(int			nFieldNum,	const char* pValue);
	void			PutField			(const char*	pFieldName,	const char* pValue);
	void			PutField			(int			nFieldNum,	bool bValue);
	void			PutField			(const char*	pFieldName,	bool bValue);
	void			PutField			(int			nFieldNum,	CDateTime& pDateTime);
	void			PutField			(const char*	pFieldName,	CDateTime& pDateTime);
	void			PutField			(int			nFieldNum,	short int nValue);
	void			PutField			(const char*	pFieldName,	short int nValue);
	void			PutField			(int			nFieldNum,	int nValue);
	void			PutField			(const char*	pFieldName,	int nValue);
	void			PutField			(int			nFieldNum,	long nValue);
	void			PutField			(const char*	pFieldName,	long nValue);
	void			PutField			(int			nFieldNum,	unsigned long nValue);
	void			PutField			(const char*	pFieldName,	unsigned long nValue);
	void			PutField			(int			nFieldNum,	double nValue);
	void			PutField			(const char*	pFieldName,	double nValue);

#ifdef _USE_FIELD_PICTURES
	// picture
	bool			SetNumberPicture	(const char* pPicture);
	bool			SetCharPicture		(const char* pPicture);
	bool			SetPunctPicture		(const char* pPicture);
	bool			SetUserPicture		(const char* pPicture);
	void			SetFieldFlags		(int nFieldNum,unsigned long nFlags);
#endif

	// data
	inline void		SetDateFormat		(DATEFORMAT format)	{m_DateTime.SetDateFormat(format);}
	inline void		SetTimeFormat		(TIMEFORMAT format)	{m_DateTime.SetTimeFormat(format);}
	inline void		SetCentury			(bool bCentury)		{m_bCentury = bCentury;}
	inline DATEFORMAT GetDateFormat		(void)				{return(m_DateTime.GetDateFormat());}
	inline TIMEFORMAT GetTimeFormat		(void)				{return(m_DateTime.GetTimeFormat());}

	bool			DateFormatForTable	(const char* pInputDate,char* pOutputDate,size_t nDateSize,const DATEFORMAT dateFormat,bool bBC = FALSE);
	bool			DateFormatFromTable	(const char* pInputDate,char* pOutputDate,size_t nDateSize,const DATEFORMAT dateFormat,bool& bBC) const;
	void			DateComplement		(char* szDateYYYYMMDD,size_t size) const;

	// strutture della tabella e degli indici
	virtual const CBASE_TABLE* GetTableStruct(void)			{return(m_pTableStruct);}
	virtual const CBASE_INDEX* GetIndexStruct(void)			{return(m_pIdxStruct);}

	// virtuali pure, la derivata dovra' definirle con i dati relativi all'implementazione della tabella
	virtual const char*	GetTableName		(void) = 0;
	virtual const char*	GetTablePath		(void) = 0;
	virtual int			GetRecordLength		(void) = 0;
	virtual const char*	GetRecordAsString	(bool bTrim = false,char cSep = 0) = 0; // FIELD_DELIM (o uno definito dal chiamante) come delimitatore di campo

	// usata per azzerare i campi prima dell'inserimento
	virtual void		ResetMemvars		(void) = 0;

	// usata per passare i campi al record interno prima dell'inserimento
	virtual void		GatherMemvars		(void) = 0;

	// usata per caricare i campi con quanto presente nel record interno dopo la ricerca
	virtual void		ScatterMemvars		(bool = TRUE) = 0;

	/*
		esempio inserimento:

		if(!Seek())
			ResetMemvars()
			PutField_...(<valore da inserire in tabella>)
			GatherMemvars()
			Insert()

		esempio reperimento:

		if(Seek())
			ScatterMemvars()
			GetField_...()
	*/

protected:
	inline void		Invalidate			(void) {m_bIsValid = FALSE; if(m_pBerkeleyDB) m_nLastError = m_pBerkeleyDB->SetLastError(DB_RETCODE_EINVALIDOBJECT);}

private:
	// membri	
	void			GetNumericField		(int,NUMBER&,NUMTYPE);
	void			PutNumericField		(int,NUMBER,NUMTYPE);

	char*			FormatField			(char* pBuffer,int nSize,bool bValue);
	char*			FormatField			(char* pBuffer,int nSize,CDateTime& pDateTime,char cType);
	char*			FormatField			(char* pBuffer,int nSize,NUMBER value,NUMTYPE type,int nFieldNum);

#ifdef _USE_FIELD_PICTURES
	void			SetDefaultNumberPicture	(void);
	void			SetDefaultCharPicture	(void);
	void			SetDefaultPunctPicture	(void);
	void			SetDefaultUserPicture	(void);
	void			SetFieldFormat			(char* pField,int nSize,unsigned long nFlags);
	void			SetFieldFormatByPicture	(char* pField,int nSize,unsigned long nFlags);
#endif

	// variabili

	// flag per validita' oggetto (la tabella CBerkeleyDB)
	bool			m_bIsValid;

	// per il lock della tabella, deprecated, vedi CLockGuard
	CSyncProcesses	m_mutexTable;
	
	// oggetto per l'interfaccia con il database
	CBerkeleyDB*	m_pBerkeleyDB;

	// copia di backup per l'ultimo codice d'errore
	int				m_nLastError;
	
	// puntatore alla definizione del database
	DATABASE*		m_pDatabase;

	// per la creazione dinamica delle strutture
	CBASE_TABLE*	m_pTableStruct;
	int				m_nTotRows;
	CBASE_INDEX*	m_pIdxStruct;
	int				m_nTotIdx;

	// puntatore al buffer per il record
	char*			m_pRecordBuffer;
	
	// per le Get...()/Put...() che usano i nomi dei campi
	FIELD			m_Field;

	// per la chiave primaria corrente
	char			m_szPrimaryKeyValue[MAX_PRIMARYKEY_SIZE+1];
	
	// tipo ricerca
	bool			m_bSoftseek;

	// data
	CDateTime		m_DateTime;
	bool			m_bCentury;

#ifdef _USE_FIELD_PICTURES
	char*			m_pPictureNumber;
	char*			m_pPictureChar;
	char*			m_pPicturePunct;
	char*			m_pPictureUserDefined;
#endif
};

#endif // _CBASE_H
