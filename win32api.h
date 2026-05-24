/*$
	win32api.h
	Implementazione di quanto omesso dall' API (SDK/MFC).
	Luca Piergentili, 13/09/98
	lpiergentili@yahoo.com

	Vedi le note in win32api.c
*/
#ifndef _WIN32API_H
#define _WIN32API_H 1

/* interfaccia C++ */
#ifdef __cplusplus
  extern "C" {
#endif

#include "window.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <wctype.h>
#include <winreg.h>

/*
  macro   |   header    |     uso
--------- | ----------- | -------------
MAX_PATH  | <windows.h> | Win32 puro
_MAX_PATH | <stdlib.h>  | CRT/portabile

definite in stdlib.h:
_MAX_PATH   260
_MAX_DRIVE  3
_MAX_DIR    256
_MAX_FNAME  256
_MAX_EXT    256
*/
#ifndef _MAX_FILEPATH
  #define _MAX_FILEPATH (_MAX_DIR+_MAX_FNAME)
#endif

#define PACKVERSION(major,minor) MAKELONG(minor,major)

#define HKEY_MICROSOFT_WINDOWS_CURRENTVERSION "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"

#define REG_VALUE_MAXSTR 256

/*
	REG_VALUE
*/
typedef union  _reg_value {
	CHAR		chr;
	BYTE		byte;
	WORD		word;
	DWORD		dword;
	ULONG		ulong;
	char		string[REG_VALUE_MAXSTR+1];
} REG_VALUE;

/*
	REGVALUEINFO
*/
typedef struct _regvalueinfo {
    LPCTSTR		name;
    DWORD		type;
	REG_VALUE	value;
} REGVALUEINFO;

/*
	OSVERSIONTYPE
*/
typedef enum _osversiontype {
	WINDOWS_31,
	WINDOWS_95,
	WINDOWS_98,
	WINDOWS_MILLENNIUM,
	WINDOWS_NT,
	WINDOWS_2000,
	WINDOWS_XP,
	WINDOWS_VISTA,
	WINDOWS_SEVEN,
	UNKNOWN_WINDOWS_VERSION
} OSVERSIONTYPE;

OSVERSIONTYPE	GetWindowsVersion		(LPSTR lpszWindowsPlatform,UINT nSize,LPDWORD pdwMajorVersion,LPDWORD pdwMinorVersion);
BOOL			GetWindowsVersionEx		(OSVERSIONINFOEX* osvi);
BOOL			GetWindowsVersionData	(REGVALUEINFO *pRegValues,UINT nSize);

BOOL			GetWindowsUAPlatform	(LPSTR lpszVersion,UINT bufferSize,double* nVersion);
BOOL			GetFriendlyWindowsName	(LPSTR buffer,UINT bufferSize);

LPSTR			GetThisModuleFileName	(LPSTR lpszFileName,UINT nSize);
DWORD			GetDllVersion			(LPCSTR lpszDllName);

LPSTR			GetUniqueMutexName		(void);

BOOL			WritePrivateProfileInt	(LPCSTR lpcszSectioneName,LPCSTR lpcszKeyName,int iValue,LPCSTR lpcszIniFile);

LPVOID			GetLastErrorString		(DWORD dwError);
DWORD			GetLastErrorDescription	(LPSTR lpBuffer,UINT nSize);

UINT			MessageBoxResource		(HWND hWnd,UINT nStyle,LPCSTR lpcszTitle,UINT nID);
UINT			MessageBoxResourceEx	(HWND hWnd,UINT nStyle,LPCSTR lpcszTitle,UINT nID,...);
UINT			FormatResourceString	(LPSTR lpszString,UINT nSize,UINT nID);
UINT			FormatResourceStringEx	(LPSTR lpszString,UINT nSize,UINT nID,...);

BOOL			ExtractResource			(UINT resource,LPCSTR lpcszResName,LPCSTR lpszFileName);
BOOL			ExtractResourceIntoBuffer(UINT nID,LPCSTR lpcszResName,LPSTR lpBuffer,UINT nSize);
BOOL			CreateShortcut			(LPCSTR lpcszTarget,LPCSTR lpcszArguments,LPCSTR lpcszLinkFileName,LPCSTR lpcszLinkLocation,LPCSTR lpcszWorkingDir,UINT nIconIndex);


#undef Yield
inline BOOL		Yield(void)				{return(SwitchToThread());} /* concede la CPU solo se c'e' un altro thread pronto, equivalente Windows del sched_yield() */
void			Delay					(UINT nMillisec);
BOOL			PeekAndPump				(void);

/*
	DISKINFO
*/
typedef struct _diskinfo {
    char		rootPath[_MAX_PATH+1];		/* "C:\" */
    char		volumeName[_MAX_PATH+1];	/* Nome volume */
    char		fileSystem[_MAX_PATH+1];	/* NTFS, FAT32, exFAT, ... */
    DWORD		serialNumber;
    DWORD		maxComponentLength;
    DWORD		fileSystemFlags;
    DWORD		driveType;					/* DRIVE_FIXED, DRIVE_REMOVABLE, ... */
    ULONGLONG	totalBytes;
    ULONGLONG	freeBytes;
    ULONGLONG	freeBytesForUser;
    ULONGLONG	usedBytes;
    DWORD		sectorsPerCluster;
    DWORD		bytesPerSector;
    DWORD		numberOfFreeClusters;
    DWORD		totalNumberOfClusters;
    BOOL		supportsCompression;		/* flags derivati */
    BOOL		supportsEncryption;
    BOOL		supportsSparseFiles;
    BOOL		supportsHardLinks;
    BOOL		supportsACLs;
    BOOL		isReadOnly;
    BOOL		isCompressedVolume;
    BOOL		isRemoteDrive;
} DISKINFO;

BOOL			GetDiskInfo				(LPCSTR lpcszRootPath,DISKINFO* pDiskInfo);
LPCSTR			GetDiskType				(DWORD dwType); /* <- DISKINFO.driveType */

/*
	FILETYPEPROBABILITY
*/
typedef struct _filetypeprobability {
    int text;
    int binary;
} FILETYPEPROBABILITY;

FILETYPEPROBABILITY GetFileTypeProbability(LPCSTR lpcszFilename);

UINT			GetDriveFromPath		(LPCSTR lpcszPathName,LPSTR lpszDrive,UINT nSize);

BOOL			CopyFileTo				(HWND hWnd,LPCSTR lpcszSourceFile,LPCSTR lpcszDestFile,BOOL bMoveInsteadCopy,BOOL bShowDialog);
BOOL			RenameFileTo			(LPCSTR lpcszOldFileName,LPCSTR lpcszNewFileName);
BOOL			DeleteFileToRecycleBin	(HWND hWnd,LPCSTR lpcszFileName,BOOL bShowDialog,BOOL bAllowUndo);

BOOL			DoesFileExist			(LPCSTR lpcszFileName,LPDWORD pdwLastError);
BOOL			DoesDirectoryExist		(LPCSTR lpcszDirectory,LPDWORD pdwLastError/* passare NULL se non serve */);
BOOL			FileExists				(LPCSTR lpcszFileName);
inline BOOL		DirectoryExists			(LPCSTR lpcszDirectory) {DWORD dwError=0L; return(DoesDirectoryExist(lpcszDirectory,&dwError));}

LPSTR			FormatFileSize			(QWORD qwFilesize,LPSTR lpszBuffer,UINT nSize);
BOOL			GetFileSizeBytes		(LPCSTR lpcszFileName,LPQWORD pqwFileSize);
QWORD			GetFileSizeExtbyHandle	(HANDLE hFile,PLARGE_INTEGER lpFileSize);
QWORD			GetFileSizeExtbyName	(LPCSTR lpcszFileName);

/*
	FILE_ATTR
*/
typedef struct _file_attr {
	const char* filename;
	WIN32_FIND_DATA finddata;
	QWORD filesize;
} FILE_ATTR;

BOOL			GetFileAttr				(FILE_ATTR* fileAttr);

/*
	MAPPEDFILE
*/
typedef struct _mappedfile {
    char		szFileName[_MAX_FILEPATH+1];
    HANDLE		hFile;
    HANDLE		hFileMap;
    LPVOID		lpFileView;
    FILETIME	ftCreationTime;
    FILETIME	ftLastAccessTime;
    FILETIME	ftLastWriteTime;
    LARGE_INTEGER largeInt;
    QWORD		qwFileSize;
    char*		pData;
	DWORD		dwError;
} MAPPEDFILE;

BOOL			OpenMappedFile			(MAPPEDFILE* pMappedFile);
BOOL			CloseMappedFile			(MAPPEDFILE* pMappedFile);

DWORD			CopyFileMapped			(LPCSTR lpcszSrcFileName,LPCSTR lpcszDstFileName);

BOOL			ChangeFileExtension		(LPSTR lpszFilename,UINT nSize,LPCSTR lpcszNewExt);
BOOL			CheckFileExtension		(LPCSTR lpcszFilename,LPCSTR lpszExt);
LPCSTR			GetFileExtension		(LPCSTR lpcszFileName);

LPCSTR			StripPathFromFile		(LPCSTR lpcszFileName);
LPSTR			StripFileFromPath		(LPCSTR lpcszPathName,LPSTR lpszPath,UINT nSize,BOOL bRemoveBackslash);
LPCSTR			SplitFileName			(LPCSTR lpcszFilename,LPSTR lpszPath,LPSTR pszName,LPSTR pszExt);
LPSTR			EnsureBackslash			(LPSTR lpszFileName,UINT nSize);
LPSTR			RemoveBackslash			(LPSTR lpszFileName);

BOOL			EnsurePathname			(LPCSTR lpcszPathname,LPDWORD pdwError);
BOOL			CreatePathname			(LPCSTR lpcszPathname,LPDWORD pdwError);

LPCSTR			EnsureValidFileName		(LPCSTR lpcszFileName,LPSTR lpszNewName,UINT cbNewName);

LPSTR			YetAnotherFileName		(LPCSTR lpcszFilename,LPSTR lpszNewName,UINT nNewNameSize);

LPSTR			DateTimeToString		(UINT uDate,UINT uTime,LPSTR lpszBuffer,UINT nSize);
BOOL			StringToDateTime		(LPCSTR lpcszDateTime,LPWORD pwdate,LPWORD pwtime);

BOOL			SetFileDateTime			(LPCSTR lpcszFileName,WORD uDate,WORD uTime);
BOOL			GetFileDateTime			(LPCSTR lpcszFileName,LPWORD lpuDate,LPWORD lpuTime,LPQWORD pqwSize/* passare NULL se non serve */);

BOOL			GetFileLastTime			(LPCSTR lpcszFileName,FILETIME* lpLastWriteTime);

/*
	GREGORIANDATETIME
*/
typedef struct _gregoriandatetime {
	int nYear;
	int nMonth;
	int nDay;
	int nDayOfWeek;
	char szDayOfWeek[5];
	int nHour;
	int nMinute;
	int nSecond;
	WORD wDate;
	WORD wTime;
} GREGORIANDATETIME;

void			FileDateTimeToGregorian	(GREGORIANDATETIME* pGregorianDateTime);
BOOL			GregorianDateTimeToFile	(GREGORIANDATETIME* pGregorianDateTime);

int				CompareFileDateTime		(WORD wFileAdate,WORD wFileAtime,WORD wFileBdate,WORD wFileBtime);

BOOL			CompareFilebyDate		(LPCSTR lpcszFileSrc,FILETIME* targetFileTime);
BOOL			CompareFileTimebyName	(LPCSTR lpcszFileSrc,LPCSTR lpcszFileDst);

long			GetFileDateSysDateDiff	(FILETIME* pftFileTime);

/*
	TASKBARPOS
*/
typedef struct _taskbarpos {
	HWND	hWnd;
	RECT	rc;
	int		nTaskbarPlacement;
	int		nTaskbarWidth;
	int		nTaskbarHeight;
	int		nScreenWidth;
	int		nScreenHeight;
} TASKBARPOS;

/* per l'aggiustamento sulle coordinate della taskbar */
#define TASKBAR_X_TOLERANCE	40
#define TASKBAR_Y_TOLERANCE	40

BOOL			GetTaskBarPos			(TASKBARPOS* tbi);

void			SetForegroundWindowEx	(HWND hWnd,BOOL bInvalidate);

wchar_t*		AnsiToWideChar			(LPCSTR pszAnsi);
LPSTR			WideCharToAnsi			(const wchar_t* pwszWide);

/*
	ANSWER
*/
typedef enum _answer {
	YES,
	NO
} ANSWER;

/* <conio.h>, getch() */
UINT			GetLineFromStdin		(LPSTR lpszBuffer,UINT nSize);
ANSWER			ConsolePromptYesOrNo	(void);
void			ConsolePromptEnter		(void);
void			ClearConsoleScreen		(void);
void			MinimizeConsoleWindow	(void);
void			RestoreConsoleWindow	(void);
void			CloseConsoleWindow		(void);
int				GetConsoleWidth			(void);
void			SetCustomConsoleIcon	(HICON* hOldIconSmall,HICON* hOldIconBig,UINT nIconID);
void			RestoreConsoleIcon		(HICON hOldIconSmall,HICON hOldIconBig);

LPSTR			GenerateUniqueRandomSalt(LPSTR lpszBuffer,UINT nSize);

DWORD			PackTwoWORD				(WORD low_word,WORD high_word);
WORD			GetLowWORD				(DWORD packed_value);
WORD			GetHighWORD				(DWORD packed_value);

QWORD			PackTwoDWORD			(DWORD low_dword,DWORD high_dword);
DWORD			GetLowDWORD				(QWORD packed_value);
DWORD			GetHighDWORD			(QWORD packed_value);

QWORD			PackBYTEintoQWORD		(QWORD q,BYTE val,int pos);
BYTE			UnpackBYTEfromQWORD		(QWORD q,int pos);

inline void		Crash					(void)
{
	printf("Wanna crash? ");
	if(ConsolePromptYesOrNo()==YES)
	{
		char* p = NULL;
		for(;;)
			*p++ = '*';
	}
}

/* interfaccia C++ */
#ifdef __cplusplus
  }
#endif

#endif /* _WIN32API_H */
