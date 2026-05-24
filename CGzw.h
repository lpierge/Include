/*$
	CGzw.h
	Classe base per il vecchio codice di interfaccia con la zLib.
	Luca Piergentili, 31/08/96 (stesura originale)
	lpiergentili@yahoo.com

	Disegno (e contesto):
	Questa classe venne scritta nel '96 per 'modernizzare' il vecchio codice C scritto per usare la zLib da linea di
	comando, all'interno di un sistema client/server di banca dati con intercambio ma senza un server reale, dove lo
	scambio dei dati veniva gestito via email con attachment binari ed elaborazione distribuita (una specie di P2P a 
	livello empresariale :)

	Lo scopo del progetto originario era quello di usare la zLib come libreria base per comprimere vari files di input 
	in un unico file di output e poter poi gestire l'estrazione in modo controllato.

	L'idea di base fu quella di aggiungere i file(s) di input ad un unico file .gz, quindi non un formato proprietario
	contenente vari files .gz sciolti, ma un unico file .gz a cui aggiungere le compressioni relative ai files di input,
	una sorta di 'append', cosi' come previsto dalle specifiche per il formato .gz (RFC 1952).

	Per poter gestire il file .gz di output come un contenitore, si disegno' quindi un header propietario (GZW) da
	inserire tra un file compresso ed il seguente, per poter cosi' scorrere e leggere all'interno del file, ed allo 
	scopo si modificarono chirurgicamente le funzioni della zLib, facendole iniettare l'header GZW nel punto esatto 
	tra la scrittura dell'header GZ originale e l'inizio della scrittura dei dati compressi.

	In sintesi, se si usa un file .gzw (in realta' un .gz gonfiato) come contenitore di oggetti (.gz), bisogna usare 
	un meccanismo (header GZW iniettato) per poter scorrere nel file, saltando da un oggetto all'altro.

	Il risultato finale fu un file di output .gzw che in realta era un file .gz con uno o piu' file(s) compressi al
	suo  interno, tutti  aggiunti con un append dopo aver creato il primo, e tutti forniti di un header aggiuntivo
	(l'header GZW), usato poi dall'interfaccia per scorrerne e leggerne il contenuto.

	Insomma, un lavoro di 'restaurazione' di un software storico.
	
	Luca P., Nov'25
*/
#ifndef _CGZW_H
#define _CGZW_H 1

#include <limits.h>
#include "typedef.h"
#include "window.h"
#include "win32api.h"
#include "gzwhdr.h"
#include "CzLib.h"
#include "CNodeList.h"
#include "CWildCards.h"

/*
	FPGZWCALLBACK
	tipo per la callback tra classe e chiamante (programma che usa la classe)
	chiamata durante le operazioni di compressione, decompressione e lista
	NON e' la callback tra classe e zLib, che viene definita in gzwhdr.h
*/
typedef LONG (*FPGZWCALLBACK)(WPARAM,LPARAM,LPARAM,LPARAM);

/*
	GzwCallback()

	Callback standard per output su stdout.
*/
LONG GzwCallback(WPARAM wParam,LPARAM lParam1,LPARAM lParam2,LPARAM lParam3);

/*
	GZW_OPERATION
	enum per le operazioni supportate
*/
enum GZW_OPERATION {
	GZW_COMPRESS = 0,
	GZW_MOVE,
	GZW_UNCOMPRESS,
	GZW_EXTRACT,
	GZW_LIST,
	GZW_UNKNOWN
};

/*
	GZW_PATHSCHEME
	enum per tipo path
*/
enum GZW_PATHSCHEME {
	GZW_PATHSCHEME_NONE = 0,
	GZW_PATHSCHEME_RELATIVE,
	GZW_PATHSCHEME_ABSOLUTE
};

/*
	GZW
	struttura per l'interfaccia con l'API GZW (la classe), da non confondere 
	con l'header GZW, definito in gzwhdr.h e condiviso con i files della zLib
*/
struct GZW {
	HWND			hWnd;							// finestra del chiamante (obsoleto, non piu' usato)
	FPGZWCALLBACK	fpCallBack;						// callback, impostare a NULL se non si utilizza
	char			szInputFile[_MAX_PATH+1];		// -i file di input
	int				nInputFileLen;					// lunghezza nome file di input
	char			szOutputFile[_MAX_PATH+1];		// -o file di output
	int				nOutputFileLen;					// lunghezza nome file di output
	char			szWildcardsIncl[_MAX_PATH+1];	// -w pattern per inclusioni
	int				nWildcardsInclLen;				// lunghezza del pattern
	char			szWildcardsExcl[_MAX_PATH+1];	// -W pattern per esclusioni
	int				nWildcardsExclLen;				// lunghezza del pattern
	size_t			nCompressRatio;					// -R<n> 0=store, 9=max
	QWORD			nMinsize;						// -s<n> bytes
	QWORD			nMaxsize;						// -S<n> bytes
	char			szPsw[GZW_PSW_MAX+1];			// -d password
	int				nPswLen;						// lunghezza password
	BOOL			bRecursiveSearch;				// -r
	BOOL			bOverwriteExisting;				// -O
	BOOL			bWholeMatch;					// -f/-F
	GZW_PATHSCHEME	nPathScheme;					// none/-P/-p
	GZW_OPERATION	nGzwOperation;					// -c/-m/-u/-e/-l
	int				nRotate;						// -t<n>
	QWORD			qwTotFilesSize;					// totali per ogni fine operazione (compress/uncompress/list)
	QWORD			qwTotCompressedFilesSize;
	DWORD			dwTotFiles;
};

// enum per i codici di ritorno
// mantenere a base 0 ed allineati con l'array in CGzw.cpp
enum GZWE_ERROR_CODE {
	GZWE_UNKNOWN_ERROR = 0,
	GZWE_UNKNOWN_OPTION,
	GZWE_WRONG_PARAMETERS,
	GZWE_COMPRESS_ERROR,
	GZWE_UNCOMPRESS_ERROR,
	GZWE_LIST_ERROR,
	GZWE_VIEW_ERROR,
	GZWE_INVALID_FILENAME,
	GZWE_NOSUCHFILE,
	GZWE_INVALID_FORMAT,
	GZWE_WRONG_PASSWORD,
	GZWE_MALLOC_ERROR,
	GZWE_SEARCH_ERROR,
	GZWE_SCRIPT_ERROR,
	GZWE_CHECK_ERROR,
	GZWE_OPEN_ERROR,
	GZWE_CREATE_ERROR,
	GZWE_CLOSE_ERROR,
	GZWE_SEEK_ERROR,
	GZWE_READ_ERROR,
	GZWE_WRITE_ERROR,
	GZWE_MKDIR_ERROR,
	GZWE_UPDATE_ERROR,
	GZWE_UNDERSIZE,
	GZWE_FILE_EXISTS,
	GZW_HALTED,
	GZW_DONE,
	GZW_SUCCESS
};

// enum per gli id della callback
enum GZW_CALLBACK_ID {
	GZW_CALLBACK_COMPRESS_BEGIN = 0,
	GZW_CALLBACK_COMPRESS,
	GZW_CALLBACK_COMPRESS_END,
	GZW_CALLBACK_COMPRESS_TOTAL,

	GZW_CALLBACK_UNCOMPRESS_BEGIN,
	GZW_CALLBACK_UNCOMPRESS,
	GZW_CALLBACK_UNCOMPRESS_TMP,
	GZW_CALLBACK_UNCOMPRESS_END,
	GZW_CALLBACK_UNCOMPRESS_TOTAL,

	GZW_CALLBACK_LIST_BEGIN,
	GZW_CALLBACK_LIST,
	GZW_CALLBACK_LIST_END,
	GZW_CALLBACK_LIST_TOTAL,

	GZW_CALLBACK_SEARCH_FILE_BEGIN,
	GZW_CALLBACK_SEARCH_FILE,
	GZW_CALLBACK_SEARCH_FILE_END,

	GZW_CALLBACK_BEGIN,
	GZW_CALLBACK_END,

	GZW_CALLBACK_HALT
};

/*
	COMPRESSION_LIBRARY_ID
	identificativi librerie (comunque sia, solo zLib)
*/
 typedef enum _compression_library_id {
	ZLIB_LIBRARY = 0
} COMPRESSION_LIBRARY_ID;

/*
	CGzw
*/
class CGzw : public CzLib
{
public:
	CGzw(HWND hWnd = NULL);
	virtual ~CGzw();

	inline void				SetOperation		(GZW_OPERATION op)	{m_Gzw.nGzwOperation = op;}
	inline GZW_OPERATION	GetOperation		(void)				{return(m_Gzw.nGzwOperation);}

	inline void				SetInput			(LPCSTR lpcszFile)	{strcpyn(m_Gzw.szInputFile,lpcszFile,sizeof(m_Gzw.szInputFile));m_Gzw.nInputFileLen = strlen(lpcszFile);}
	inline LPCSTR			GetInput			(void)				{return(m_Gzw.szInputFile);}

	inline void				SetOutput			(LPCSTR lpcszFile)	{strcpyn(m_Gzw.szOutputFile,lpcszFile,sizeof(m_Gzw.szOutputFile)); m_Gzw.nOutputFileLen = strlen(lpcszFile);}
	inline LPCSTR			GetOutput			(void)				{return(m_Gzw.szOutputFile);}

	/*
	permettono definire i valori da usare per l'inclusione o esclusione di patterns nei percorsi/nomi di files, 
	da non confondere con il pattern usato per specificare l'input per compressione, decompressione e lista
	possono essere specificati piu' valori separandoli con ';', ad es.: -w.txt;text;... etc.
	i flag -f e -F controllano su cosa viene applicato il confronto: -f per considerare solo il nome del file e 
	-F per considerare l'intero pathname (se presente), vedi sotto le funzioni CheckInclusions() e CheckExclusions()
	*/
	void					SetIncludePattern	(LPCSTR lpcszWildcards = NULL,BOOL bIgnoreCase = FALSE);
	inline LPCSTR			GetIncludePattern	(void)				{return(m_Gzw.szWildcardsIncl);}

	void					SetExcludePattern	(LPCSTR lpcszWildcards = NULL,BOOL bIgnoreCase = FALSE);
	inline LPCSTR			GetExcludePattern	(void)				{return(m_Gzw.szWildcardsExcl);}

	void					SetPatternMatchWhole(BOOL bFlag)		{m_Gzw.bWholeMatch = bFlag;}

	/*
	compressione:
		-p registra il pathname assoluto (ossia il pathname completo relativo al file)
		-P registra il pathname relativo (ossia parziale, a partire dalla directory dove si trova il file)
		none (default) registra solo il nome file, ignorando qualsiasi pathname
	decompressione:
		-p ricrea l'intero pathname, esattamente come salvato (drive incluso), ignorando la directory di output
		-P ricrea l'intero pathname a partire dalla directory corrente o da quella specificata (ignora il drive)
		none (default) estrae i file nella directory specificata, ignorando qualsiasi pathname salvato in compressione
	*/
	inline void				SetPathScheme		(GZW_PATHSCHEME nPathScheme) {m_Gzw.nPathScheme = nPathScheme;}
	inline GZW_PATHSCHEME	GetPathScheme		(void)				{return(m_Gzw.nPathScheme);}
	
	inline void				SetRecursive		(BOOL bFlag)		{m_Gzw.bRecursiveSearch = bFlag;}
	inline BOOL				GetRecursive		(void)				{return(m_Gzw.bRecursiveSearch);}

	inline void				SetRatio			(int n)				{m_Gzw.nCompressRatio = (n >= 0 && n < 9) ? n : 9;}
	inline int				GetRatio			(void)				{return(m_Gzw.nCompressRatio);}

	inline void				SetOverwrite		(BOOL bFlag)		{m_Gzw.bOverwriteExisting = bFlag;}
	inline BOOL				GetOverwrite		(void)				{return(m_Gzw.bOverwriteExisting);}

	inline void				SetMinSize			(QWORD n)			{m_Gzw.nMinsize = (n > 0LL && n < LLONG_MAX) ? n : 0LL;}
	inline QWORD			GetMinSize			(void)				{return(m_Gzw.nMinsize);}

	inline void				SetMaxSize			(QWORD n)			{m_Gzw.nMaxsize = (n > 0LL && n < LLONG_MAX) ? n : LLONG_MAX;}
	inline QWORD			GetMaxSize			(void)				{return(m_Gzw.nMaxsize);}

	UINT					SetPassword			(LPCSTR lpcszPsw);
	inline LPCSTR			GetPassword			(void)				{return(m_Gzw.szPsw);}

	inline void				SetRotate			(int n)				{m_Gzw.nRotate = (n > 0 && n < 1965) ? n : 3;}
	inline int				GetRotate			(void)				{return(m_Gzw.nRotate);}
	
	/*
	il codice della classe NON visualizza nessun progresso, quindi per avere un feedback sull'avanzamento, il chiamante 
	deve fornire il puntatore ad una callback, che verra' chiamata per ogni file elaborato dalle funzioni di compressione, 
	decompressione e lista
	a differenza del resto, il codice per il listing, OLTRE a chiamare la callback, permette salvare l'output in un file 
	da specificare tramite SetOutput()
	un callback predefinita - GzwCallback() - con output su stdout viene fornita alla fine del file CGzw.cpp
	*/
	void					SetCallback			(FPGZWCALLBACK lpFn) {m_Gzw.fpCallBack = lpFn;}

	UINT					Gzw					(void);
	
	LPCSTR					GetErrorDesc		(UINT nRet);

	// come regola generale, le callbacks NON devono essere dichiarate nella sezione private per evitare problemi di 
	// visibilita' in caso di chiamate da parte di codice esterno, come avviene con la zLib

	// dichiara le callbacks per la CFindFile della classe: la wrapper (static) e la reale, chiamata via puntatore 
	// this, e usata per filtrare la inclusione/esclusione dei files durante la compressione, mentre per la lista e 
	// la decompressione non viene usata callback perche' i filtri si applicano al contenuto dei files di input
	//
	static LONG __stdcall	FindFileCallBackWrapper	(WPARAM,LPARAM,LPARAM,LPARAM);
	LONG					FindFileCallBack		(WPARAM,LPARAM,LPARAM,LPARAM);
	//
	// qui la convenzione di chiamata e' il defaut di Windows (__stdcall), perche' la convenzione WinAPI dice che tutte 
	// le funzioni che vengono chiamate dal sistema operativo, o da librerie che lo emulano, devono aderire allo standard 
	// del chiamante, che su Windows e' quasi sempre __stdcall
	// __stdcall = la funzione chiamata deve ripulire lo stack prima di tornare

	// callbacks per la zLib: la wrapper (static) e la reale, chiamata via puntatore this
	// le callbacks vengono usate per visualizzare il % di compressione/decompressione
	// 
    static void				ProgressCallbackWrapper	(void* pContext, int nPercent, int nAction);
	void					ProgressCallback		(int nPercent,int nAction);
	//
	// qui la convenzione di chiamata e' il defaut (__cdecl) perche' il codice che la chiama e' codice C (la DLL della zLib) 
	// che per default usa __cdecl, quindi qui deve conformarsi
	// __cdecl = il chiamante (DLL) deve ripulire lo stack prima di tornare

protected:
	BOOL					IsCompressedFile	(LPCSTR lpcszFilename);

private:
	UINT					Compress			(void);
	UINT					CompressFile		(LPCSTR,LPCSTR,LPCSTR,GZWHDR*);

	UINT					Uncompress			(void);
	UINT					UncompressFile		(void);
	LPCSTR					ExtractFile			(LPGZWHEADERDATA);

	UINT					List				(void);
	UINT					ListFile			(void);
	LPCSTR					ExtractHeader		(LPGZWHEADERDATA);
	
	UINT					CheckGzwHeader		(LPCSTR);
	UINT					CheckLibraryHeader	(FILE*,size_t*,BOOL,COMPRESSION_LIBRARY_ID);
	BOOL					CheckGzHeader		(const unsigned char*);

	BOOL					CheckInputOutput	(GZW_OPERATION);

	LPCSTR					ParseScript			(LPCSTR,UINT*);
	UINT					StripPath			(LPSTR,UINT);
	UINT					AddAbsolutePath		(LPSTR,UINT);
	UINT					AddRelativePath		(LPSTR,UINT);
	UINT					Diff				(LPSTR,LPSTR);
	LPSTR					TmpName				(LPSTR,size_t);
	void					TmpClean			(void);
	BOOL					EnsureOutDirExists	(LPCSTR,BOOL);

	/*
	verificano se il nome file soddisfa le condizioni di inclusione/esclusione specificate con le opzioni -w e -W
	vengono usate, durante la compressione, sui nomi dei files del pattern di input, espanso con la Search() della 
	CFindfile, e durante la decompressione/lista per discriminare sui nomi dei file contenuti nel .gzw di input
	i flag -f e -F determinano se il match tra nome file e pattern viene verificato SOLO sul nome del file o sul
	pathname COMPLETO (se presente)
	*/
	LONG					CheckInclusions		(LPCSTR);
	LONG					CheckExclusions		(LPCSTR);

	GZW						m_Gzw;
	GZWHDR					m_GzwHeader;
	CWildCards				m_includePattern;
	CWildCards				m_excludePattern;
	CItemList*				m_pitemListIncludePattern;
	CItemList*				m_pitemListExcludePattern;

	// variabili specifiche per il buffer allocato dinamicamente dalla ExtractFile()
	// l'estrazione di un .gzw contenente <n> files, comporta <n>x chiamate a malloc() e free(),
	// mentre con la dichiarazione delle sentinelle, qui, nella classe, le chiamate a malloc() e
	// free() si riducono a 1x
	// l'allocazione avviene solo alla prima chiamata di ExtractFile(), mentre il rilascio solo 
	// avviene nel distruttore della classe
	unsigned char* buffer_ptr;
	int malloed;
};

#endif // _CGZW_H
