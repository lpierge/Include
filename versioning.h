/*$
	versioning.h
	Definizione delle macro per il versionamento.
	Luca Piergentili, '24
*/

/*
						SISTEMA VERSIONAMENTO A TRE NUMERI CON PUNTEGGIATURA

Il sistema e' chiamato versionamento semantico (SemVer) e nasce dalla pratica diffusa in Unix/Linux, 
poi codificata nella specifica https://semver.org.

Formato:
	MAJOR.MINOR.PATCH (es. 2.14.7)

Significato di ciascun numero:
	MAJOR
	incompatibilita' introdotta: API cambiate, formati di file non retro-compatibili, dipendenze rotte
	quando sale, MINOR e PATCH si azzerano
	MINOR
	funzionalita'' nuove che non rompono la retro-compatibilita'
	quando sale, PATCH si azzera
	PATCH
	bug-fix o piccoli cambiamenti interni che non aggiungono funzionalita' e non rompono nulla

Perche' si usa?
	dipendenze automatiche (package manager, build system) possono aggiornare sicuri fino alla major successiva
	es. 2.14.7 -> ""accetta qualsiasi 2.x.y >= 2.14.7""
	comunicazione chiara al downstream: so cosa posso aspettarmi senza leggere il changelog
	versioni ordinabili lexicalmente: 2.9.0 < 2.10.0 < 3.0.0

Esempio pratico:
	Cambiamento						Nuova versione
	fix typo						2.14.8
	nuova API retro-compatibile		2.15.0
	rimosso parametro obbligatorio	3.0.0

Estensioni comuni:
	pre-release: 2.0.0-alpha.1
	build metadata: 2.0.0+win64

In sintesi:
	MAJOR rompe, MINOR aggiunge, PATCH ripara; il resto e' zero

Spesso i tre numeri vengono impacchettati in un unico dword (32 bit) usando maschere di bit.

Il layout piu' comune e':
	31            23            15             7            0
	|   MAJOR   |   MINOR   |   PATCH   |
		 8 bit       8 bit       8 bit      (8 bit liberi o build)

Formula:
	#define VERSION_PACK(maj,min,pat)	((((maj) & 0xFFU) << 16) | (((min) & 0xFFU) << 8) | ((pat) & 0xFFU))
	#define VERSION_UNPACK_MAJOR(v)		(((v) >> 16) & 0xFF)
	#define VERSION_UNPACK_MINOR(v)		(((v) >>  8) & 0xFF)
	#define VERSION_UNPACK_PATCH(v)		( (v)        & 0xFF)

Esempio pratico:
	unsigned int v = VERSION_PACK(2, 14, 7);	// v = 0x020E07
	unsigned int maj = VERSION_UNPACK_MAJOR(v);	// 2
	unsigned int min = VERSION_UNPACK_MINOR(v);	// 14
	unsigned int pat = VERSION_UNPACK_PATCH(v);	// 7

Vantaggi:
	un solo numero da confrontare (if (ver >= 0x020E00))
	ordinamento lessicale naturale: 0x020E07 < 0x020F00 < 0x030000
	salva spazio in struct, registry, resource

Svantaggi:
	massimo 255 per ogni componente (8 bit), ma sufficiente per la maggior parte dei progetti
*/
#ifndef _VERSIONING_H
#define _VERSIONING_H 1

#define VERSION_PACK(maj,min,pat)	((((maj) & 0xFFU) << 16) | (((min) & 0xFFU) << 8) | ((pat) & 0xFFU))
#define VERSION_UNPACK_MAJOR(v)		(((v) >> 16) & 0xFF)
#define VERSION_UNPACK_MINOR(v)		(((v) >>  8) & 0xFF)
#define VERSION_UNPACK_PATCH(v)		((v) & 0xFF)

#endif /* _VERSIONING_H */
