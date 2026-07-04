/*
	textdef.h
*/
#ifndef _TEXTDEF_H
#define _TEXTDEF_H 1

/* definizione dell'enumerazione e della struttura per la posizione del testo */
typedef enum _text_alignment {
    ALIGN_TOP_LEFT = 0,
    ALIGN_TOP_RIGHT = 1,
    ALIGN_BOTTOM_LEFT = 2,
    ALIGN_BOTTOM_RIGHT = 3,
    ALIGN_CENTER = 4,
    ALIGN_CUSTOM = 5,
    ALIGN_FIT = 6,
    ALIGN_RESERVED = 7
} TEXT_ALIGNMENT;

typedef struct _text_position {
    TEXT_ALIGNMENT alignment;
    TEXT_ALIGNMENT originalAlignment;
    long x; 
    long y; 
    long margin_x;
    long margin_y;
} TEXT_POSITION;

#endif /* _TEXTDEF_H */
