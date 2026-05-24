/*$
	traceexpr.h
	Luca Piergentili, 16/01/97
	lpiergentili@yahoo.com
*/
#ifndef _TRACE_H
#define _TRACE_H 1

/* interfaccia C++ */
#ifdef __cplusplus
  extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "macro.h"

#define	TRACE_LOG_FILE			"trace.log"		/* nome del file di log */
#define	TRACECALL_LOG_FILE		"tracecall.log"	/* nome del file di log per le chiamate di funzione */
#define	TRACE_BUF				8192			/* dimensione del buffer per l'output */

/* flag bitmask, dove ogni valore è una potenza di due */
#define	_TRFLAG_NOTRACE			0x00000001		/* nessun output */
#define	_TRFLAG_TRACEFILE		0x00000002		/* output su file */
#define	_TRFLAG_TRACECONSOLE	0x00000004		/* output nella finestra */
#define	_TRFLAG_TRACEOUTPUT		0x00000008		/* output nella finestra del debugger */
#define	_TRFLAG_TRACEBREAKPOINT 0x00000010		/* breakpoint nel debugger */

#define __NOFILE__ ((const char *)NULL)
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
		/* output */
		void trace(unsigned long,const char*,unsigned int,const char*,...);
		#define TRACEEXPR(a) {trace a;}

		/* chiamate di funzione */
		void trace_call(int,int,char *);
		#define TRACECALLS_BEGIN(b,a)	trace_call(b,0,a)
		#define TRACECALLS_END(b,a)		trace_call(b,1,a)
		#define TRACECALLS_ABEND(b,a)	trace_call(b,1,a)

		/* per il file di log */
		void trace_init(void);
		#define TRACE_INIT trace_init

		/* assert */
		void assert_expr(void *,void *,unsigned);
		#define ASSERTEXPR(e) (void)((e)||(assert_expr((void*)#e,(void*)__FILE__,__LINE__),0))
		#define ASSERTCALL(e) (void)((e)||(assert_expr((void*)#e,(void*)__FILE__,__LINE__),0))

		/* versione con messaggio */
		void assert_expr_with_msg(const char* expr, const char* file, unsigned line, const char* msg);
		#define ASSERTEXPRMSG(e,m) (void)((e)||(assert_expr_with_msg(#e,__FILE__,__LINE__,m),0))
	#else
		#define TRACEEXPR(a) {;}
		
		#define TRACECALLS_BEGIN(b,a)
		#define TRACECALLS_END(b,a)
		#define TRACECALLS_ABEND(b,a)
		
		#define TRACE_INIT()

		#define ASSERTEXPR(e)	assert(e)
		#define ASSERTCALL(e)	e
	#endif
#else
	#define TRACEEXPR(a) {;}
	
	#define TRACECALLS_BEGIN(b,a)
	#define TRACECALLS_END(b,a)
	#define TRACECALLS_ABEND(b,a)
	
	#define TRACE_INIT()

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
