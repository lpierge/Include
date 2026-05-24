/*$
	pragma.h
	Elimina i warning scassacazzi del VC++ 4.2.
	Come norma generale, includerlo nei .c/.cpp, no nei .h (nei .h se includono codice inline).
	Da inserire come prima linea del codice, prima di qualsiasi #include.
	Luca Piergentili, 13/08/96
	lpiergentili@yahoo.com
*/
#ifndef _PRAGMA_H
#define _PRAGMA_H 1

/* definire comunque (Debug/Release) per Visual Studio 2022 */
#if _MSC_VER >= 1930
#define _CRT_OBSOLETE_NO_WARNINGS 1
#define _CRT_SECURE_NO_WARNINGS 1
/*#define _CRT_NONSTDC_NO_DEPRECATE 1*/
#endif

#if defined(_MSC_VER)
	#ifdef _DEBUG
		#ifdef PRAGMA_MESSAGE_VERBOSE
			#pragma message("\t\t\t"\
							__FILE__\
							" included: some warning disabled")
		#endif
//		#pragma warning(disable:4100) /* unreferenced formal parameter */
//		#pragma warning(disable:4102) /* unreferenced label */
//		#pragma warning(disable:4115) /* named type definition in parentheses */
//		#pragma warning(disable:4127) /* conditional expression is constant */
//		#pragma warning(disable:4131) /* uses old-style declarator */
//		#pragma warning(disable:4146) /* (headers VC) unary minus operator applied to unsigned type, result still unsigned */
//		#pragma warning(disable:4206) /* nonstandard extension used : translation unit is empty */
//		#pragma warning(disable:4214) /* nonstandard extension used : bit field types other than int */
//		#pragma warning(disable:4244) /* conversion from 'int' to 'unsigned short', possible loss of data */
//		#pragma warning(disable:4305) /* abstract declarator : truncation from '...' to '...' */
//		#pragma warning(disable:4505) /* unreferenced local function has been removed */
//		#pragma warning(disable:4514) /* unreferenced inline function has been removed */
//		#pragma warning(disable:4663) /* (headers VC) C++ language change: to explicitly specialize class template '...' use the following syntax:... */
	#endif

//	#pragma warning(disable:4201) /* nonstandard extension used : nameless struct/union */
//	#pragma warning(disable:4206) /* translation unit is empty */
//	#pragma warning(disable:4482) /* nonstandard extension used: enum ... used in qualified name */
	#pragma warning(disable:4996) /* MSVC2008: this function or variable may be unsafe (deprecated) */
//	#pragma warning(disable:4996) /* code uses a function, class member, variable, or typedef that's marked deprecated  */
//	#pragma warning(disable:4189) /* local variable is initialized but not referenced  */
	#pragma warning(disable:4456) /* VS2022: declaration of 'identifier' hides previous local declaration */
	#pragma warning(disable:4100) /* VS2022: 'identifier' : unreferenced formal parameter */

	#pragma warning(default:4189) /* local variable is initialized but not referenced  */
	#pragma warning(default:4101) /* local variable is initialized but not referenced  */
	#pragma warning(default:4100) /* unreferenced formal parameter */

//	#pragma warning(default:4101)
#endif

#endif /* _PRAGMA_H */
