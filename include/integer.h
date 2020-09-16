/*-------------------------------------------*/
/* Integer type definitions for FatFs module */
/*-------------------------------------------*/

#ifndef _FF_INTEGER
#define _FF_INTEGER
#include "stdint.h"



#ifdef _WIN32	/* FatFs development platform */

#include <windows.h>
#include <tchar.h>
typedef unsigned __int64 QWORD;


#else			/* Embedded platform */

/* These types MUST be 16-bit or 32-bit */
typedef int				INT;
typedef unsigned int	UINT;

/* This type MUST be 8-bit */
typedef int8_t          CHAR;
typedef uint8_t         UCHAR;
typedef uint8_t         BYTE;
typedef uint8_t         BOOL;

/* These types MUST be 16-bit */
typedef short			SHORT;
typedef uint16_t        USHORT;
typedef unsigned short	WORD;
typedef unsigned short	WCHAR;

/* These types MUST be 32-bit */
typedef int32_t         LONG;
typedef uint32_t        ULONG;
typedef uint32_t        DWORD;

/* This type MUST be 64-bit (Remove this for C89 compatibility) */
typedef unsigned long long QWORD;


#endif

#endif
