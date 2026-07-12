/*$
	traceexpr.h
	Luca Piergentili, 16/01/97
	lpiergentili@yahoo.com

	Usa le macro TCHAR, _T(), etc. per essere compatibile in modo trasparente con ANSI e con Wide (Unicode).
	Quello che cambia non e' quindi il codice qui, nell'implementazione, ma il modo in cui deve essere usato
	dal chiamante.

	In progetti di tipo ANSI, la macro TRACEEXPR dovre' essere usata con questa sintassi:

		TRACEEXPR((_TRACE_FLAG_INFO,__FILE__,__LINE__,"SomeFunction(): some text: %s\n",szParam));

	con szParam dichiarato come char szParam[...];, mentre in progetti Unicode, la sintassi dovra' essere:

		TRACEEXPR((_TRACE_FLAG_INFO,_L(__FILE__),__LINE__,L"SomeFunction(): some text: %s\n",wzParam));

	con wzParam dichiarato come wchar_t wzParam[...];. Se in C++, dichiarando std::wstring wzParam;, bisognera'
	usare:

		TRACEEXPR((_TRACE_FLAG_INFO,_L(__FILE__),__LINE__,L"SomeFunction(): some text: %s\n",wzParam.c_str()));

	La macro _L(), definita in modo similare alla _T(), viene usata per la conversione di tipo esplicito, dato
	che la macro __FILE__ e' definita come const char[] (ANSI/UTF-8 estandar), mentre la macro TRACEEXPR si
	aspetta un tipo TCHAR.
	Non si puo' usare direttamente L come per le stringhe costanti, perche' L non e' una macro che agisce su
	altre macro, ma un prefisso grammaticale del linguaggio che puo' essere applicato solo ad un letterale di
	tipo stringa gia' definito, non ad una macro che deve ancora essere espansa.

	A livello di progetto, le macro relative al set di caratteri sono UNICODE e _UNICODE (la prima per le API 
	Win32 e la seconda per funzioni C runtime) o _MBCS per ANSI/MBCS.

	Per garantire un minimo di thread-safety se in ambiente multithread, usa una sezione critica per tracciare
	il log.
	I mutex di Windows sono oggetti del kernel ed ogni volta che se ne usa uno la CPU deve passare da User-Mode
	a Kernel-Mode, operazione pesante che in un hook globale puo' causare un collo di bottiglia o un deadlock 
	inter-processo.
	Al contrario, la sezione critica gira in User-Mode, ed e' rientrante, ossia permette un lock ricorsivo da
	parte dello stesso thread.
	Quindi il chiamante DEVE usare le due macro TRACE_INIT e TRACE_TERM, dato che si occupano della gestione
	della sezione critica (anche se poi il codice si assicura di funzionare correttamente anche se le chiamate
	non avvengono...).
*/
#ifndef _TRACE_H
#define _TRACE_H 1

#include "macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <tchar.h> /* TCHAR, _T(), etc. */

/* interfaccia C++ */
#ifdef __cplusplus
  extern "C" {
#endif

#define	TRACE_LOG_FILE			_T("trace.log")		/* nome del file di log */
#define	TRACECALL_LOG_FILE		_T("tracecall.log")	/* nome del file di log per le chiamate di funzione */
#define	TRACE_BUF				8192				/* dimensione del buffer per l'output */

/* flag bitmask, dove ogni valore e' una potenza di due */
#define	_TRFLAG_NOTRACE			0x00000001			/* nessun output */
#define	_TRFLAG_TRACEFILE		0x00000002			/* output su file */
#define	_TRFLAG_TRACECONSOLE	0x00000004			/* output nella finestra */
#define	_TRFLAG_TRACEOUTPUT		0x00000008			/* output nella finestra del debugger */
#define	_TRFLAG_TRACEBREAKPOINT 0x00000010			/* breakpoint nel debugger */

#define __NOFILE__ ((const TCHAR*)NULL)
#define __NOLINE__ (0)

#ifdef _DEBUG
	#ifdef _WINDOWS
		#define DEBUGBREAKPOINT() __debugbreak()
	#else
		#define DEBUGBREAKPOINT()
	#endif
#else
		#define DEBUGBREAKPOINT()
#endif

/* abilita TRACEEXPR() solo se viene definito _DEBUG */
#ifdef _DEBUG
	#ifdef _WINDOWS
		/* per il file di log/console, necessarie anche per la sezione critica  */
		void trace_init(unsigned long flag);
		void trace_term(unsigned long flag);
		#define TRACE_INIT trace_init
		#define TRACE_TERM trace_term

		/* output */
		void trace(unsigned long,const TCHAR*,unsigned int,const TCHAR*,...);
		#define TRACEEXPR(a) {trace a;}

		/* chiamate di funzione */
		void trace_call(int,int,char *);
		#define TRACECALLS_BEGIN(b,a)	trace_call(b,0,a)
		#define TRACECALLS_END(b,a)		trace_call(b,1,a)
		#define TRACECALLS_ABEND(b,a)	trace_call(b,1,a)

		/* assert */
		void assert_expr(void *,void *,unsigned);
		#define ASSERTEXPR(e) (void)((e)||(assert_expr((void*)#e,(void*)_T(__FILE__),__LINE__),0))
		#define ASSERTCALL(e) (void)((e)||(assert_expr((void*)#e,(void*)_T(__FILE__),__LINE__),0))

		/* versione con messaggio */
		void assert_expr_with_msg(const TCHAR* expr,const TCHAR* file,unsigned line,const TCHAR* msg);
		#define ASSERTEXPRMSG(e,m) (void)((e)||(assert_expr_with_msg(#e,_T(__FILE__),__LINE__,m),0))
	#else
		#define TRACEEXPR(a) {;}
		
		#define TRACECALLS_BEGIN(b,a)
		#define TRACECALLS_END(b,a)
		#define TRACECALLS_ABEND(b,a)
		
		#define TRACE_INIT()
		#define TRACE_TERM()

		#define ASSERTEXPR(e)	assert(e)
		#define ASSERTCALL(e)	e
	#endif
#else
	#define TRACEEXPR(a) {;}
	
	#define TRACECALLS_BEGIN(b,a)
	#define TRACECALLS_END(b,a)
	#define TRACECALLS_ABEND(b,a)
	
	#define TRACE_INIT(a)
	#define TRACE_TERM(a)

	#define ASSERTEXPR(e)
	//#define ASSERTEXPR(e)	((void)0)
	#define ASSERTCALL(e)	e
	#define ASSERTEXPRMSG(e,m)
#endif

/* interfaccia C++ */
#ifdef __cplusplus
  }
#endif

#endif /* _TRACE_H */
