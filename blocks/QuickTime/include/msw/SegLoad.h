/*
     File:       SegLoad.h
 
     Contains:   Segment Loader Interfaces.
 
     Version:    Technology: Mac OS 8
                 Release:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1985-1999 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __SEGLOAD__
#define __SEGLOAD__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif




#if PRAGMA_ONCE
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
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

#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM || !TARGET_OS_MAC
/*
   CountAppFiles, GetAppFiles, ClrAppFiles, GetAppParms, getappparms, 
   and the AppFile data structure and enums are obsolete. 
   They are still supported for writing old style 68K apps, 
   but they are not supported for CFM-based apps.
   Use AppleEvents to determine which files are to be 
   opened or printed from the Finder.
*/
enum {
  appOpen                       = 0,    /*Open the Document (s)*/
  appPrint                      = 1     /*Print the Document (s)*/
};

struct AppFile {
  short               vRefNum;
  OSType              fType;
  short               versNum;                /*versNum in high byte*/
  Str255              fName;
};
typedef struct AppFile                  AppFile;
#if CALL_NOT_IN_CARBON
/*
 *  CountAppFiles()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
CountAppFiles(
  short *  message,
  short *  count);


/*
 *  GetAppFiles()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
GetAppFiles(
  short      index,
  AppFile *  theFile);


/*
 *  ClrAppFiles()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
ClrAppFiles(short index);


/*
 *  GetAppParms()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
GetAppParms(
  Str255    apName,
  short *   apRefNum,
  Handle *  apParam)                                          ONEWORDINLINE(0xA9F5);


#endif  /* CALL_NOT_IN_CARBON */

#if CALL_NOT_IN_CARBON
/*
 *  getappparms()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
getappparms(
  char *    apName,
  short *   apRefNum,
  Handle *  apParam);


#endif  /* CALL_NOT_IN_CARBON */

#endif  /* TARGET_CPU_68K && !TARGET_RT_MAC_CFM || !TARGET_OS_MAC */

 
/*
   Because PowerPC applications don't have segments.
   But, in order to allow applications to not have conditionalized
   source code, UnloadSeg is macro'ed away for PowerPC.
*/
#if TARGET_CPU_68K
#if CALL_NOT_IN_CARBON
/*
 *  UnloadSeg()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
UnloadSeg(void * routineAddr)                                 ONEWORDINLINE(0xA9F1);


#endif  /* CALL_NOT_IN_CARBON */

#else
#define UnloadSeg(x)
#endif  /* TARGET_CPU_68K */

/* ExitToShell() has moved to Process.h*/





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

#ifdef __cplusplus
}
#endif

#endif /* __SEGLOAD__ */

