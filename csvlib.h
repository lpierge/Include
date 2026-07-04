/*$
	csvlib.h
	Operazioni su files in formato CSV.
	Luca Piergentili, Ottobre '25
*/
#ifndef _CSVLIB_H
#define _CSVLIB_H 1

/* interfaccia C++ */
#ifdef __cplusplus
  extern "C" {
#endif

/*
	CSV_FIELD_MAP
	struttura per mappare i valori (campi) del .csv nelle variabili di memoria relative
	usarla dichiarandone un array dove ogni elemento corrisponde ad un campo del .csv
	specificare il puntatore al buffer e la dimensione (il sizeof)
*/
typedef struct _csv_field_map {
    char*	dest_ptr;
    size_t	max_len;
} CSV_FIELD_MAP;

/* restituiscono 0 in caso di successo, un valore negativo in caso di errore */
int	csv_parse_line	(const char* csv_line,CSV_FIELD_MAP* field_map,size_t num_fields);
int	csv_format_line	(char* out_buffer,size_t buf_size,CSV_FIELD_MAP* field_map,size_t num_fields);

/* codici di ritorno/errore */
#define CSV_SUCCESS					0
#define CSV_ERR_MALFORMED			-1
#define CSV_ERR_BUFFER_FULL			-2
#define CSV_ERR_TOO_MANY_FIELDS		-3
#define CSV_ERR_BUFFER_TOO_SMALL	-4
#define CSV_ERR_READ_FAIL			-5
#define CSV_ERR_WRITE_FAIL			-6

/* interfaccia C++ */
#ifdef __cplusplus
  }
#endif

#endif /* _CSVLIB_H */
