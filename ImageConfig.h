/*$
	ImageConfig.h
	Definizione delle librerie supportate.
	Luca Piergentili, 01/09/00
	lpiergentili@yahoo.com

	Ad memoriam - Nemo me impune lacessit.
*/
#ifndef _IMAGECONFIG_H
#define _IMAGECONFIG_H 1

// per ogni libreria supportata, definire la HAVE_[...] relativa per includere/escludere le librerie a compile-time
#define HAVE_PAINTLIB_LIBRARY		1
#define HAVE_NEXGENIPL_LIBRARY		1
#define HAVE_FREEIMAGE_LIBRARY		1
#define HAVE_IMAGEMAGICK_LIBRARY	1

#endif // _IMAGECONFIG_H
