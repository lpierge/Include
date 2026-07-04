/*$
	CTextFont.h
	Classe per la gestione del file words (wchg).
	Luca Piergentili, 13/12/25

	Il codice e' specifico per riempire la lista con il contenuto del file words.txt:

	;
	;	words.txt
	;
	;	PAY CLOSE ATTENTION: THIS FILE MUST BE IN ANSI FORMAT AND MUST CONTAIN ONLY ANSI CHARACTERS.
	;	ANY TEXT FILE YOU USE FOR 'WORDS' FILE MUST BE ANSI AND CONTAIN ONLY ANSI CHARACTERS.
	;
	;	This is the configuration file used to write text over images (used with the -w option).
	;	Any line starting with ';' is treated as a comment and ignored.
	;	Keep in mind that all labels and values are case-sensitive.
	;	IF YOU MESS IT UP, just delete the file and the program will recreate it with the default values the next time you run it.
	;	Changes here require restarting the program.
	;
	;	Each record (=line) must contain the following fields:
	;		<text>,<font name>,<font size>,<r (RED component of RGB)>,<g (GREEN component of RGB)>,<b (BLUE component of RGB)>,<alignment>
	;
	;	OR just the string text, in which case the values from the [Default] section will be applied,
	;
	;	OR just one of the below macros ($AMERICANDATE, etc.).
	;
	;		allowed values for <font size> are: 5-500
	;		allowed value for <alignment> are: ALIGN_TOP_LEFT, ALIGN_TOP_RIGHT, ALIGN_BOTTOM_LEFT, ALIGN_BOTTOM_RIGHT
	;
	;	The [Default] section is mandatory and must define all the required values, to be used for records that only contain the text string.
	; 
	;	The [Fonts] section is optional and will be used as follows:
	;
	;		If the [Fonts] section exists and contains one or more font names:
	;			load those fonts and randomly choose one to apply
	;			(overrides font name of the record)
	;
	;		If the [Fonts] section exists but is empty:
	;			load ALL available system fonts and randomly pick one to use
	;			(overrides font name of the record)
	;
	;		If the [Fonts] section is not present:
	;			use the font value(s) specified in the current record, 
	;			or fall back to the value from [Default] if none are specified
	;
	;	Instead of a plain text string, you can use one of the following macros:
	;	$AMERICANDATE, $BRITISHDATE, $GMTDATE, $SHORTTIME, $TIME, $AMPMTIME, $DRIVEC:
	;	Note that the $DRIVE macro requires the drive letter followed by ':', for example $DRIVEC:, $DRIVED:, etc.
	;
	[Default] "CaliMerO","Verdana","28","255","0","255","ALIGN_BOTTOM_RIGHT"
	[Fonts]
	$AMPMTIME
	Calimero, pulcino piccolo e nero,Impact,36,255,0,255,ALIGN_BOTTOM_RIGHT
*/
#ifndef _CTEXTFONT_H
#define _CTEXTFONT_H 1

#include <stdio.h>
#include <stdlib.h>
#include "window.h"
#include "CNodeList.h"

#define MAX_FD_NAME_LEN		_MAX_FILEPATH

/*
	FONTDATA

	aggruppa le informazioni relative al font, al momento solo include il nome
	da ampliare
*/
typedef struct _fontdata {
	char name[MAX_FD_NAME_LEN+1];
} FONTDATA;

/*
	CFontDataList

	classe per la lista dei fonts via FONTDATA
	conterra' i nomi dei fonts registrati nel sistema
*/
class CFontDataList : public CNodeList
{
public:
	CFontDataList() : CNodeList() {}
	virtual ~CFontDataList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new FONTDATA);
	}	
	void* Initialize(void* pVoid)
	{
		FONTDATA* pData = (FONTDATA*)pVoid;
		if(!pData)
			pData = (FONTDATA*)Create();
		if(pData)
			memset(pData,'\0',sizeof(FONTDATA));
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((FONTDATA*)iter->data)
			delete ((FONTDATA*)iter->data),iter->data = (FONTDATA*)NULL;
		return(TRUE);
	}
	int Size(void) {return(sizeof(FONTDATA));}
#ifdef _DEBUG
	const char* Signature(void) {return("CFontDataList");}
#endif
};

#define MAX_TF_TEXT_LEN		256
#define MAX_TF_FONTNAME_LEN	64
#define MAX_TF_NUMBER_LEN	5
#define MAX_TF_ALIGN_LEN	32

/*
	TEXTFONT

	definisce il record del file words:
	frase (testo o macro), nome del font, dimensione del font (esatta o %), colore del font x 3 (in formato RGB), modalita' posizionamento frase
*/
typedef struct _textfont {
	char	text[MAX_TF_TEXT_LEN+1];
	char	fontname[MAX_TF_FONTNAME_LEN+1];
	char	fontsize[MAX_TF_NUMBER_LEN+1];		// come stringa perche' puo' contenere un numero esatto (22) o in percentuale (5%)
	int		fontcolor_r;
	int		fontcolor_g;
	int		fontcolor_b;
	int		textalign;
} TEXTFONT;

/*
	TEXTFONTRECORD

	struttura usata per mappare TEXTFONT sul blocco di dati usato dal codice in csvlib per caricare i records del file words
*/
typedef struct _textfontrecord {
	char	text[MAX_TF_TEXT_LEN+1];
	char	fontname[MAX_TF_FONTNAME_LEN+1];
	char	fontsize[MAX_TF_NUMBER_LEN+1];
	char	fontcolor_r[MAX_TF_NUMBER_LEN+1];
	char	fontcolor_g[MAX_TF_NUMBER_LEN+1];
	char	fontcolor_b[MAX_TF_NUMBER_LEN+1];
	char	textalign[MAX_TF_ALIGN_LEN+1];
} TEXTFONTRECORD;

#define MAX_TF_RECORD_LEN (sizeof(TEXTFONTRECORD)+1)

/*
	CTextFontList

	classe per la lista dei records di tipo TEXTFONT
	conterra' tutti i records letti dal file words
*/
class CTextFontList : public CNodeList
{
public:
	CTextFontList() : CNodeList() {}
	virtual ~CTextFontList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new TEXTFONT);
	}	
	void* Initialize(void* pVoid)
	{
		TEXTFONT* pData = (TEXTFONT*)pVoid;
		if(!pData)
			pData = (TEXTFONT*)Create();
		if(pData)
			memset(pData,'\0',sizeof(TEXTFONT));
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((TEXTFONT*)iter->data)
			delete ((TEXTFONT*)iter->data),iter->data = (TEXTFONT*)NULL;
		return(TRUE);
	}
	int Size(void) {return(sizeof(TEXTFONT));}
#ifdef _DEBUG
	const char* Signature(void) {return("CTextFontList");}
#endif
};

/*
	CTextFontManager
	
	classe per la manipolazione dei dati caricati nella lista di cui sopra, basicamente il motore che nutre il chiamante con i dati del file words
*/
class CTextFontManager
{
public:
	CTextFontManager();
	~CTextFontManager() {}

	void			Clean				(void);											// riazzera tutto
	BOOL			Load				(LPCSTR lpcszFilename,LPCSTR lpcszDefaultText);	// carica la lista di cui sopra con i record del file words
	TEXTFONT*		Next				(void);											// restituisce, in un ciclo rotatorio/infinito, i records della lista
	TEXTFONT*		Default				(void) {return(&m_defaultTextFont);}			// restituisce il record con i valori di default
	int				Count				(void) {return(m_textFontList.Count());}		// restituisce il totale dei record presenti nella lista
	BOOL			EnumLabelFonts		(LPSTR pString);								// carica la lista per i fonts con quelli specificati dall'etichetta
	BOOL			EnumInstalledFonts	(void);											// carica la lista per i fonts con quelli installati nel sistema
	LPCSTR			GetAFont			(void);											// ricava in modo random un nome font dalla lista interna

private:
	TEXTFONT		m_defaultTextFont;													// record di default
	CTextFontList	m_textFontList;														// lista per i records
	int				m_nCurrent;															// indice corrente per l'elemento della lista dei records
	CFontDataList	m_fontDataList;														// lista per i nomi dei fonts
};

#endif // _CTEXTFONT_H
