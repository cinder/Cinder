/*
     File:       Streaming.h
 
     Contains:   xxx put contents here xxx
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1994, 1996 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
/*.#endif forMasterInterfaces*/

#ifndef __STREAMING__
#define __STREAMING__

#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
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

/*  interfaces to streammode for c*/

/* streaming constants for foreground and background streaming*/
enum {
  AlphaStream                   = 1,
  GraphicsStream                = 2
};

/* mode bits*/
enum {
  CopyToAlpha                   = 0x0200,
  CopyToGraphics                = 0x0100
};

#if CALL_NOT_IN_CARBON
/*
 *  AlphaVersion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
AlphaVersion(void)                                            TWOWORDINLINE(0x7000, 0xABC0);


/*
 *  SetForeStream()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( long )
SetForeStream(long streamID)                                  TWOWORDINLINE(0x7001, 0xABC0);


/*
 *  SetForeTransparency()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
SetForeTransparency(short streamRatio)                        TWOWORDINLINE(0x7002, 0xABC0);


/*
 *  SetBackStream()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( long )
SetBackStream(long streamID)                                  TWOWORDINLINE(0x7003, 0xABC0);


/*
 *  SetBackTransparency()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
SetBackTransparency(short streamRatio)                        TWOWORDINLINE(0x7004, 0xABC0);


/*
 *  GetForeStream()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
GetForeStream(void)                                           TWOWORDINLINE(0x7005, 0xABC0);


/*
 *  GetForeTransparency()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
GetForeTransparency(void)                                     TWOWORDINLINE(0x7006, 0xABC0);


/*
 *  GetBackStream()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
GetBackStream(void)                                           TWOWORDINLINE(0x7007, 0xABC0);


/*
 *  GetBackTransparency()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
GetBackTransparency(void)                                     TWOWORDINLINE(0x7008, 0xABC0);


/*
 *  ResizeGrafVars()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
ResizeGrafVars(void)                                          TWOWORDINLINE(0x7009, 0xABC0);


/*
 *  GetStreamMode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
GetStreamMode(void)                                           TWOWORDINLINE(0x700A, 0xABC0);


/* these are thew new ones*/
/*
 *  SetForeAlpha()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
SetForeAlpha(
  long    streamType,
  short   value)                                              TWOWORDINLINE(0x700B, 0xABC0);


/*
 *  SetBackAlpha()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
SetBackAlpha(
  long    streamType,
  short   value)                                              TWOWORDINLINE(0x700C, 0xABC0);


/*
 *  GetForeAlpha()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
GetForeAlpha(void)                                            TWOWORDINLINE(0x700D, 0xABC0);


/*
 *  GetBackAlpha()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
GetBackAlpha(void)                                            TWOWORDINLINE(0x700E, 0xABC0);


/*
 *  GetForeAlphaMask()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
GetForeAlphaMask(void)                                        TWOWORDINLINE(0x700F, 0xABC0);


/*
 *  GetBackAlphaMask()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
GetBackAlphaMask(void)                                        TWOWORDINLINE(0x7010, 0xABC0);


#endif  /* CALL_NOT_IN_CARBON */

#if !TARGET_OS_MAC

#define AlphaVersion()                     0
#define SetForeStream(streamID)               0
#define SetForeTransparency(streamRatio)
#define SetBackStream(streamID)              0
#define SetBackTransparency(streamRatio)
#define GetForeStream()                      0
#define GetForeTransparency()             0
#define GetBackStream()                       0
#define GetBackTransparency()             0
#define GetStreamMode()                       0
#define SetForeAlpha( streamType, value)  0
#define SetBackAlpha( streamType, value)  0
#define GetForeAlpha()                        0
#define GetBackAlpha()                        0
#define GetForeAlphaMask()                    0
#define GetBackAlphaMask()                    0

#endif  /* !TARGET_OS_MAC */


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

#endif /* __STREAMING__ */

