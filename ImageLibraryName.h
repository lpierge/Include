/*$
	ImageLibraryName.h
	Definizione dei nomi delle librerie supportate.
	Luca Piergentili, 01/09/00
	lpiergentili@yahoo.com

	Ad memoriam - Nemo me impune lacessit.
*/
#ifndef _IMAGELIBRARYNAME_H
#define _IMAGELIBRARYNAME_H 1

#include "ImageConfig.h"

// per ogni nuova libreria, aggiungere la macro con il nome
#ifdef HAVE_PAINTLIB_LIBRARY
  #define PAINTLIB_LIB_NAME    "paintLib"
#endif
#ifdef HAVE_NEXGENIPL_LIBRARY
  #define NEXGENIPL_LIB_NAME   "NexgenIPL"
#endif
#ifdef HAVE_FREEIMAGE_LIBRARY
  #define FREEIMAGE_LIB_NAME   "FreeImage"
#endif
#ifdef HAVE_IMAGEMAGICK_LIBRARY
  #define IMAGEMAGICK_LIB_NAME "ImageMagick"
#endif

// imposta la libreria di default su NexgenIPL
#define IMAGE_DEFAULT_LIBRARY	FREEIMAGE_LIB_NAME

#endif // _IMAGELIBRARYNAME_H
