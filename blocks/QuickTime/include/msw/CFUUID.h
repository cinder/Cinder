/*
     File:       CFUUID.h
 
     Contains:   CoreFoundation UUIDs
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1999-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __CFUUID__
#define __CFUUID__



#if PRAGMA_ONCE
#pragma once
#endif

#if PRAGMA_IMPORT
#pragma import on
#endif

#if PRAGMA_STRUCT_ALIGN
    #pragma options align=mac68k
#elif PRAGMA_STRUCT_PACKPUSH
    #pragma pack(push, 2)
#elif PRAGMA_STRUCT_PACK
    #pragma pack(2)
#endif

#include <CFBase.h>

typedef struct __CFUUID*                CFUUIDRef;
struct CFUUIDBytes {
  UInt8               byte0;
  UInt8               byte1;
  UInt8               byte2;
  UInt8               byte3;
  UInt8               byte4;
  UInt8               byte5;
  UInt8               byte6;
  UInt8               byte7;
  UInt8               byte8;
  UInt8               byte9;
  UInt8               byte10;
  UInt8               byte11;
  UInt8               byte12;
  UInt8               byte13;
  UInt8               byte14;
  UInt8               byte15;
};
typedef struct CFUUIDBytes              CFUUIDBytes;
/* The CFUUIDBytes struct is a 128-bit struct that contains the
raw UUID. A CFUUIDRef can provide such a struct from the
CFUUIDGetUUIDBytes() function. This struct is suitable for
passing to APIs that expect a raw UUID.
*/

#if PRAGMA_STRUCT_ALIGN
    #pragma options align=reset
#elif PRAGMA_STRUCT_PACKPUSH
    #pragma pack(pop)
#elif PRAGMA_STRUCT_PACK
    #pragma pack()
#endif

#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif


#endif /* __CFUUID__ */

