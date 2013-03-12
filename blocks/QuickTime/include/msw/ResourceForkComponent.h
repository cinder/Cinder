/*
     File:       ResourceForkComponent.h
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 1984-2007 by Apple Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
/*.#endif forMasterInterfaces*/
/*.#ifndef forMergedInterface*/
#ifndef __RESOURCEFORKCOMPONENT__
#define __RESOURCEFORKCOMPONENT__

#ifndef __COMPONENTS__
#include <Components.h>
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

/*.#endif forMergedInterfaces*/
enum {
  resourceForkComponentType     = FOUR_CHAR_CODE('rfrk')
};

enum {
  rfmCanExpand                  = FOUR_CHAR_CODE('expn'),
  rfmCanExpandEasily            = FOUR_CHAR_CODE('expe'),
  rfmIsMultiStream              = FOUR_CHAR_CODE('muls')
};

enum {
  rfmQueryForkMask              = 1 << 0, /* bit on == resource fork, off == data fork*/
  rfmQueryResourceFork          = 1 << 0,
  rfmQueryDataFork              = 0
};

#if CALL_NOT_IN_CARBON
/*
 *  ResourceForkPathNameMap()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
ResourceForkPathNameMap(
  ComponentInstance   rfi,
  char *              inDataPath,
  char *              outRsrcPath,
  unsigned long       maxLen)                                 FIVEWORDINLINE(0x2F3C, 0x000C, 0x0001, 0x7000, 0xA82A);


/*
 *  ResourceForkOffsetAndLength()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
ResourceForkOffsetAndLength(
  ComponentInstance   rfi,
  char *              accessPath,
  long                queryFlags,
  long *              dataOffset,
  long *              dataLength)                             FIVEWORDINLINE(0x2F3C, 0x0010, 0x0002, 0x7000, 0xA82A);


/*
 *  ResourceForkGetMethodInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
ResourceForkGetMethodInfo(
  ComponentInstance   rfi,
  long                queryFlags,
  long                queryType,
  long *              answer)                                 FIVEWORDINLINE(0x2F3C, 0x000C, 0x0003, 0x7000, 0xA82A);


/*
 *  ResourceForkFileLength()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
ResourceForkFileLength(
  ComponentInstance   rfi,
  char *              accessPath,
  long                queryFlags,
  long *              fileLength)                             FIVEWORDINLINE(0x2F3C, 0x000C, 0x0004, 0x7000, 0xA82A);


/*
 *  ResourceForkOffsetAndLength64()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
ResourceForkOffsetAndLength64(
  ComponentInstance   rfi,
  char *              accessPath,
  long                queryFlags,
  wide *              dataOffset,
  wide *              dataLength)                             FIVEWORDINLINE(0x2F3C, 0x0010, 0x0005, 0x7000, 0xA82A);


/*
 *  ResourceForkFileLength64()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
ResourceForkFileLength64(
  ComponentInstance   rfi,
  char *              accessPath,
  long                queryFlags,
  wide *              fileLength)                             FIVEWORDINLINE(0x2F3C, 0x000C, 0x0006, 0x7000, 0xA82A);


/*.#ifndef forMergedInterfaces*/


/* selectors for component calls */
enum {
    kResourceForkPathNameMapSelect             = 0x0001,
    kResourceForkOffsetAndLengthSelect         = 0x0002,
    kResourceForkGetMethodInfoSelect           = 0x0003,
    kResourceForkFileLengthSelect              = 0x0004,
    kResourceForkOffsetAndLength64Select       = 0x0005,
    kResourceForkFileLength64Select            = 0x0006
};
/*.#endif forMergedInterfaces*/

#endif  /* CALL_NOT_IN_CARBON */


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

#endif /* __RESOURCEFORKCOMPONENT__ */

