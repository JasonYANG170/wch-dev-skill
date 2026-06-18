/********************************** (C) COPYRIGHT *******************************
* File Name          : CH563BAS.H
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : BaseDefine
*******************************************************************************/



/******************************************************************************/
#ifndef __CH563BAS_H__
#define __CH563BAS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ********************************************************************************************************************* */
/* Base types & constants */

#ifndef TRUE
#define TRUE                    1
#define FALSE                   0
#endif
#ifndef NULL
#define NULL                    0
#endif

#ifndef VOID
#define VOID                    void
#endif
#ifndef CONST
#define CONST                   const
#endif
#ifndef BOOL
typedef unsigned char           BOOL;
#endif
#ifndef BOOLEAN
typedef unsigned char           BOOLEAN;
#endif
#ifndef CHAR
typedef char                    CHAR;
#endif
#ifndef INT8
typedef char                    INT8;
#endif
#ifndef INT16
typedef short                   INT16;
#endif
#ifndef INT32
typedef long                    INT32;
#endif
#ifndef UINT8
typedef unsigned char           UINT8;
#endif
#ifndef UINT16
typedef unsigned short          UINT16;
#endif
#ifndef UINT32
typedef unsigned long           UINT32;
#endif
#ifndef UINT8V
typedef unsigned char volatile  UINT8V;
#endif
#ifndef UINT16V
typedef unsigned short volatile UINT16V;
#endif
#ifndef UINT32V
typedef unsigned long volatile  UINT32V;
#endif

#ifndef PVOID
typedef void                    *PVOID;
#endif
#ifndef PCHAR
typedef char                    *PCHAR;
#endif
#ifndef PCHAR
typedef const char              *PCCHAR;
#endif
#ifndef PINT8
typedef char                    *PINT8;
#endif
#ifndef PINT16
typedef short                   *PINT16;
#endif
#ifndef PINT32
typedef long                    *PINT32;
#endif
#ifndef PUINT8
typedef unsigned char           *PUINT8;
#endif
#ifndef PUINT16
typedef unsigned short          *PUINT16;
#endif
#ifndef PUINT32
typedef unsigned long           *PUINT32;
#endif
#ifndef PUINT8V
typedef volatile unsigned char  *PUINT8V;
#endif
#ifndef PUINT16V
typedef volatile unsigned short *PUINT16V;
#endif
#ifndef PUINT32V
typedef volatile unsigned long  *PUINT32V;
#endif

/* ********************************************************************************************************************* */
/* Base macros */

#ifndef min
#define min(a,b)                (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a,b)                (((a) > (b)) ? (a) : (b))
#endif

#if		DEBUG
#define PRINT(X...) printf(X)
#else
#define PRINT(X...)
#endif

/* Calculate the byte offset of a field in a structure of type */
#define FIELD_OFFSET(Type, Field)    ((UINT16)&(((Type *)0)->Field))

/* Calculate the size of a field in a structure of type */
#define FIELD_SIZE(Type, Field)      (sizeof(((Type *)0)->Field))

/* An expression that yields the type of a field in a struct */
#define FIELD_TYPE(Type, Field)      (((Type *)0)->Field)

/* Return the number of elements in a statically sized array */
#define NUMBER_OF(Array)             (sizeof(Array)/sizeof((Array)[0]))
#define NUMBER_OF_FIELD(Type, Field) (NUMBER_OF(FIELD_TYPE(Type, Field)))

#ifdef __cplusplus
}
#endif

#endif
