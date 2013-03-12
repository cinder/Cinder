/*
     File:       LowMem.h
 
     Contains:   Low Memory Accessor Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1993-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __LOWMEM__
#define __LOWMEM__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __FILES__
#include <Files.h>
#endif

#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif

#ifndef __OSUTILS__
#include <OSUtils.h>
#endif

#ifndef __RESOURCES__
#include <Resources.h>
#endif



#if !OPAQUE_TOOLBOX_STRUCTS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

#endif  /* !OPAQUE_TOOLBOX_STRUCTS */



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

/**************************************************************************************

    SIMPLE LOWMEM ACCESSORS

**************************************************************************************/
/*
    The following functions were moved to Quickdraw.h:
    
        LMSetDeviceList
        LMSetLastSPExtra
        LMGetWidthListHand
        LMSetWidthListHand
        LMGetScrHRes
        LMSetScrHRes
        LMSetScrVRes
        LMGetScrVRes
        LMGetHiliteMode
        LMSetHiliteMode
        LMGetMainDevice
        LMSetMainDevice
        LMGetDeviceList
        LMGetQDColors
        LMSetQDColors
        LMGetWidthPtr
        LMSetWidthPtr
        LMGetWidthTabHandle
        LMSetWidthTabHandle
        LMGetLastSPExtra
        LMGetLastFOND
        LMSetLastFOND
        LMGetFractEnable
        LMSetFractEnable
        LMGetTheGDevice
        LMSetTheGDevice
        LMGetCursorNew
        LMSetCursorNew
        LMGetHiliteRGB
        LMSetHiliteRGB
    
    The following functions were moved to TextEdit.h:
    
        LMGetWordRedraw
        LMSetWordRedraw

    The following functions were moved to Menus.h:
    
        LMGetTheMenu
    
    The following functions were moved to Events.h:
    
        LMGetKeyRepThresh
        LMSetKeyRepThresh
        LMGetKeyThresh
        LMSetKeyRepThresh
        LMGetKbdLast
        LMSetKbdLast
        LMGetKbdType
        LMSetKbdType

*/

/*
 *  LMGetMemTop()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Ptr ) LMGetMemTop(void)                           TWOWORDINLINE(0x2EB8, 0x0108);


/*
 *  LMSetMemTop()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetMemTop(Ptr value)                     TWOWORDINLINE(0x21DF, 0x0108);



/*
 *  LMGetBufPtr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Ptr ) LMGetBufPtr(void)                           TWOWORDINLINE(0x2EB8, 0x010C);


/*
 *  LMSetBufPtr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetBufPtr(Ptr value)                     TWOWORDINLINE(0x21DF, 0x010C);



/*
 *  LMGetHeapEnd()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Ptr ) LMGetHeapEnd(void)                          TWOWORDINLINE(0x2EB8, 0x0114);


/*
 *  LMSetHeapEnd()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetHeapEnd(Ptr value)                    TWOWORDINLINE(0x21DF, 0x0114);



#if CALL_NOT_IN_CARBON
/*
 *  LMGetTheZone()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( THz ) LMGetTheZone(void)                          TWOWORDINLINE(0x2EB8, 0x0118);


/*
 *  LMSetTheZone()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetTheZone(THz value)                    TWOWORDINLINE(0x21DF, 0x0118);



/*
 *  LMGetUTableBase()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Ptr ) LMGetUTableBase(void)                       TWOWORDINLINE(0x2EB8, 0x011C);


/*
 *  LMSetUTableBase()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetUTableBase(Ptr value)                 TWOWORDINLINE(0x21DF, 0x011C);



#endif  /* CALL_NOT_IN_CARBON */

/*
 *  LMGetCPUFlag()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( UInt8 ) LMGetCPUFlag(void)                        TWOWORDINLINE(0x1EB8, 0x012F);


/*
 *  LMSetCPUFlag()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetCPUFlag(UInt8 value)                  TWOWORDINLINE(0x11DF, 0x012F);



#if CALL_NOT_IN_CARBON
/*
 *  LMGetApplLimit()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Ptr ) LMGetApplLimit(void)                        TWOWORDINLINE(0x2EB8, 0x0130);


/*
 *  LMSetApplLimit()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetApplLimit(Ptr value)                  TWOWORDINLINE(0x21DF, 0x0130);



/*
 *  LMGetSysEvtMask()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetSysEvtMask(void)                    TWOWORDINLINE(0x3EB8, 0x0144);


/* Carbon Usage: use SetEventMask*/
/*
 *  LMSetSysEvtMask()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetSysEvtMask(SInt16 value)              TWOWORDINLINE(0x31DF, 0x0144);



#endif  /* CALL_NOT_IN_CARBON */

/*
 *  LMGetRndSeed()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt32 ) LMGetRndSeed(void)                       TWOWORDINLINE(0x2EB8, 0x0156);


/*
 *  LMSetRndSeed()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetRndSeed(SInt32 value)                 TWOWORDINLINE(0x21DF, 0x0156);



/*
 *  LMGetSEvtEnb()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( UInt8 ) LMGetSEvtEnb(void)                        TWOWORDINLINE(0x1EB8, 0x015C);


/*
 *  LMSetSEvtEnb()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetSEvtEnb(UInt8 value)                  TWOWORDINLINE(0x11DF, 0x015C);



#if CALL_NOT_IN_CARBON
/*
 *  LMGetTicks()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UInt32 ) LMGetTicks(void)                         TWOWORDINLINE(0x2EB8, 0x016A);


/*
 *  LMSetTicks()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetTicks(UInt32 value)                   TWOWORDINLINE(0x21DF, 0x016A);



/*
 *  LMGetVIA()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Ptr ) LMGetVIA(void)                              TWOWORDINLINE(0x2EB8, 0x01D4);


/*
 *  LMSetVIA()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetVIA(Ptr value)                        TWOWORDINLINE(0x21DF, 0x01D4);



/*
 *  LMGetSCCRd()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Ptr ) LMGetSCCRd(void)                            TWOWORDINLINE(0x2EB8, 0x01D8);


/*
 *  LMSetSCCRd()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetSCCRd(Ptr value)                      TWOWORDINLINE(0x21DF, 0x01D8);



/*
 *  LMGetSCCWr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Ptr ) LMGetSCCWr(void)                            TWOWORDINLINE(0x2EB8, 0x01DC);


/*
 *  LMSetSCCWr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetSCCWr(Ptr value)                      TWOWORDINLINE(0x21DF, 0x01DC);



/*
 *  LMGetSPValid()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UInt8 ) LMGetSPValid(void)                        TWOWORDINLINE(0x1EB8, 0x01F8);


/*
 *  LMSetSPValid()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetSPValid(UInt8 value)                  TWOWORDINLINE(0x11DF, 0x01F8);



/*
 *  LMGetSPATalkA()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UInt8 ) LMGetSPATalkA(void)                       TWOWORDINLINE(0x1EB8, 0x01F9);


/*
 *  LMSetSPATalkA()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetSPATalkA(UInt8 value)                 TWOWORDINLINE(0x11DF, 0x01F9);



/*
 *  LMGetSPATalkB()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UInt8 ) LMGetSPATalkB(void)                       TWOWORDINLINE(0x1EB8, 0x01FA);


/*
 *  LMSetSPATalkB()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetSPATalkB(UInt8 value)                 TWOWORDINLINE(0x11DF, 0x01FA);



/*
 *  LMGetSPConfig()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UInt8 ) LMGetSPConfig(void)                       TWOWORDINLINE(0x1EB8, 0x01FB);


/*
 *  LMSetSPConfig()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetSPConfig(UInt8 value)                 TWOWORDINLINE(0x11DF, 0x01FB);



/*
 *  LMGetSPPortA()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetSPPortA(void)                       TWOWORDINLINE(0x3EB8, 0x01FC);


/*
 *  LMSetSPPortA()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetSPPortA(SInt16 value)                 TWOWORDINLINE(0x31DF, 0x01FC);



/*
 *  LMGetSPPortB()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetSPPortB(void)                       TWOWORDINLINE(0x3EB8, 0x01FE);


/*
 *  LMSetSPPortB()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetSPPortB(SInt16 value)                 TWOWORDINLINE(0x31DF, 0x01FE);



/*
 *  LMGetSPAlarm()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt32 ) LMGetSPAlarm(void)                       TWOWORDINLINE(0x2EB8, 0x0200);


/*
 *  LMSetSPAlarm()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetSPAlarm(SInt32 value)                 TWOWORDINLINE(0x21DF, 0x0200);



/*
 *  LMGetSPFont()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetSPFont(void)                        TWOWORDINLINE(0x3EB8, 0x0204);


/*
 *  LMSetSPFont()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetSPFont(SInt16 value)                  TWOWORDINLINE(0x31DF, 0x0204);



/*
 *  LMGetSPKbd()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UInt8 ) LMGetSPKbd(void)                          TWOWORDINLINE(0x1EB8, 0x0206);


/*
 *  LMSetSPKbd()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetSPKbd(UInt8 value)                    TWOWORDINLINE(0x11DF, 0x0206);



/*
 *  LMGetSPPrint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UInt8 ) LMGetSPPrint(void)                        TWOWORDINLINE(0x1EB8, 0x0207);


/*
 *  LMSetSPPrint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetSPPrint(UInt8 value)                  TWOWORDINLINE(0x11DF, 0x0207);



/*
 *  LMGetSPVolCtl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UInt8 ) LMGetSPVolCtl(void)                       TWOWORDINLINE(0x1EB8, 0x0208);


/*
 *  LMSetSPVolCtl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetSPVolCtl(UInt8 value)                 TWOWORDINLINE(0x11DF, 0x0208);



/*
 *  LMGetSPClikCaret()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UInt8 ) LMGetSPClikCaret(void)                    TWOWORDINLINE(0x1EB8, 0x0209);


/*
 *  LMSetSPClikCaret()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetSPClikCaret(UInt8 value)              TWOWORDINLINE(0x11DF, 0x0209);



/*
 *  LMGetSPMisc2()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UInt8 ) LMGetSPMisc2(void)                        TWOWORDINLINE(0x1EB8, 0x020B);


/*
 *  LMSetSPMisc2()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetSPMisc2(UInt8 value)                  TWOWORDINLINE(0x11DF, 0x020B);




/* Carbon Usage: use GetDateTime*/
/*
 *  LMGetTime()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt32 ) LMGetTime(void)                          TWOWORDINLINE(0x2EB8, 0x020C);


/* Carbon Usage: use SetDateTime*/
/*
 *  LMSetTime()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetTime(SInt32 value)                    TWOWORDINLINE(0x21DF, 0x020C);



#endif  /* CALL_NOT_IN_CARBON */

/*
 *  LMGetBootDrive()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt16 ) LMGetBootDrive(void)                     TWOWORDINLINE(0x3EB8, 0x0210);


/*
 *  LMSetBootDrive()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetBootDrive(SInt16 value)               TWOWORDINLINE(0x31DF, 0x0210);



#if CALL_NOT_IN_CARBON
/*
 *  LMGetSFSaveDisk()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetSFSaveDisk(void)                    TWOWORDINLINE(0x3EB8, 0x0214);


/*
 *  LMSetSFSaveDisk()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetSFSaveDisk(SInt16 value)              TWOWORDINLINE(0x31DF, 0x0214);



#endif  /* CALL_NOT_IN_CARBON */

/*
 *  LMGetMemErr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt16 ) LMGetMemErr(void)                        TWOWORDINLINE(0x3EB8, 0x0220);


/*
 *  LMSetMemErr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetMemErr(SInt16 value)                  TWOWORDINLINE(0x31DF, 0x0220);



/*
 *  LMGetSdVolume()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( UInt8 ) LMGetSdVolume(void)                       TWOWORDINLINE(0x1EB8, 0x0260);


/*
 *  LMSetSdVolume()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetSdVolume(UInt8 value)                 TWOWORDINLINE(0x11DF, 0x0260);



/*
 *  LMGetSoundPtr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Ptr ) LMGetSoundPtr(void)                         TWOWORDINLINE(0x2EB8, 0x0262);


/*
 *  LMSetSoundPtr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetSoundPtr(Ptr value)                   TWOWORDINLINE(0x21DF, 0x0262);



/*
 *  LMGetSoundBase()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Ptr ) LMGetSoundBase(void)                        TWOWORDINLINE(0x2EB8, 0x0266);


/*
 *  LMSetSoundBase()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetSoundBase(Ptr value)                  TWOWORDINLINE(0x21DF, 0x0266);



/*
 *  LMGetSoundLevel()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( UInt8 ) LMGetSoundLevel(void)                     TWOWORDINLINE(0x1EB8, 0x027F);


/*
 *  LMSetSoundLevel()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetSoundLevel(UInt8 value)               TWOWORDINLINE(0x11DF, 0x027F);



/*
 *  LMGetCurPitch()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt16 ) LMGetCurPitch(void)                      TWOWORDINLINE(0x3EB8, 0x0280);


/*
 *  LMSetCurPitch()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetCurPitch(SInt16 value)                TWOWORDINLINE(0x31DF, 0x0280);



#if CALL_NOT_IN_CARBON
/*
 *  LMGetROM85()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetROM85(void)                         TWOWORDINLINE(0x3EB8, 0x028E);


/*
 *  LMSetROM85()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetROM85(SInt16 value)                   TWOWORDINLINE(0x31DF, 0x028E);



/*
 *  LMGetPortBUse()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UInt8 ) LMGetPortBUse(void)                       TWOWORDINLINE(0x1EB8, 0x0291);


/*
 *  LMSetPortBUse()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetPortBUse(UInt8 value)                 TWOWORDINLINE(0x11DF, 0x0291);



/*
 *  LMGetGNEFilter()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( GetNextEventFilterUPP ) LMGetGNEFilter(void)      TWOWORDINLINE(0x2EB8, 0x029A);


/*
 *  LMSetGNEFilter()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetGNEFilter(GetNextEventFilterUPP value) TWOWORDINLINE(0x21DF, 0x029A);



#endif  /* CALL_NOT_IN_CARBON */

/*
 *  LMGetSysZone()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( THz ) LMGetSysZone(void)                          TWOWORDINLINE(0x2EB8, 0x02A6);


/*
 *  LMSetSysZone()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetSysZone(THz value)                    TWOWORDINLINE(0x21DF, 0x02A6);



/*
 *  LMGetApplZone()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( THz ) LMGetApplZone(void)                         TWOWORDINLINE(0x2EB8, 0x02AA);


/*
 *  LMSetApplZone()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetApplZone(THz value)                   TWOWORDINLINE(0x21DF, 0x02AA);



#if CALL_NOT_IN_CARBON
/*
 *  LMGetROMBase()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Ptr ) LMGetROMBase(void)                          TWOWORDINLINE(0x2EB8, 0x02AE);


/*
 *  LMSetROMBase()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetROMBase(Ptr value)                    TWOWORDINLINE(0x21DF, 0x02AE);



/*
 *  LMGetRAMBase()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Ptr ) LMGetRAMBase(void)                          TWOWORDINLINE(0x2EB8, 0x02B2);


/*
 *  LMSetRAMBase()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetRAMBase(Ptr value)                    TWOWORDINLINE(0x21DF, 0x02B2);




/*
 *  LMGetDSAlertTab()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Ptr ) LMGetDSAlertTab(void)                       TWOWORDINLINE(0x2EB8, 0x02BA);


/*
 *  LMSetDSAlertTab()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetDSAlertTab(Ptr value)                 TWOWORDINLINE(0x21DF, 0x02BA);




/*
    NOTE:   LMGetABusVars and LMSetABusVars have been removed.
            Their implememtation in InterfaceLib was inconsistent
            with their prototypes here.  In InterfaceLib LMSetABusVars 
            would copy eight bytes and LMGetABusVars would return the
            value 0x02D8 instead of the long at that location.
            
            Use LMGetABusGlobals/LMSetABusGlobals to get/set the
            long at location 0x02D8 which is a pointer to the AppleTalk
            globals.  Use LMGetABusDCE/LMSetABusDCE to get/set the
            long at location 0x02DC which is the .MPP driver
            Device Control Entry. 
            
*/
/*
 *  LMGetABusGlobals()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Ptr ) LMGetABusGlobals(void)                      TWOWORDINLINE(0x2EB8, 0x02D8);


/*
 *  LMGetABusDCE()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Ptr ) LMGetABusDCE(void)                          TWOWORDINLINE(0x2EB8, 0x02DC);



/*
 *  LMSetABusGlobals()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetABusGlobals(Ptr value)                TWOWORDINLINE(0x21DF, 0x02D8);


/*
 *  LMSetABusDCE()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetABusDCE(Ptr value)                    TWOWORDINLINE(0x21DF, 0x02DC);



/* Carbon Usage: use GetDblTime*/
/*
 *  LMGetDoubleTime()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UInt32 ) LMGetDoubleTime(void)                    TWOWORDINLINE(0x2EB8, 0x02F0);


/*
 *  LMSetDoubleTime()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetDoubleTime(UInt32 value)              TWOWORDINLINE(0x21DF, 0x02F0);


/* Carbon Usage: use GetCaretTime*/
/*
 *  LMGetCaretTime()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UInt32 ) LMGetCaretTime(void)                     TWOWORDINLINE(0x2EB8, 0x02F4);


/*
 *  LMSetCaretTime()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetCaretTime(UInt32 value)               TWOWORDINLINE(0x21DF, 0x02F4);



#endif  /* CALL_NOT_IN_CARBON */

/*
 *  LMGetScrDmpEnb()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( UInt8 ) LMGetScrDmpEnb(void)                      TWOWORDINLINE(0x1EB8, 0x02F8);


/*
 *  LMSetScrDmpEnb()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetScrDmpEnb(UInt8 value)                TWOWORDINLINE(0x11DF, 0x02F8);



/*
 *  LMGetBufTgFNum()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt32 ) LMGetBufTgFNum(void)                     TWOWORDINLINE(0x2EB8, 0x02FC);


/*
 *  LMSetBufTgFNum()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetBufTgFNum(SInt32 value)               TWOWORDINLINE(0x21DF, 0x02FC);



/*
 *  LMGetBufTgFFlg()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt16 ) LMGetBufTgFFlg(void)                     TWOWORDINLINE(0x3EB8, 0x0300);


/*
 *  LMSetBufTgFFlg()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetBufTgFFlg(SInt16 value)               TWOWORDINLINE(0x31DF, 0x0300);



/*
 *  LMGetBufTgFBkNum()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt16 ) LMGetBufTgFBkNum(void)                   TWOWORDINLINE(0x3EB8, 0x0302);


/*
 *  LMSetBufTgFBkNum()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetBufTgFBkNum(SInt16 value)             TWOWORDINLINE(0x31DF, 0x0302);



/*
 *  LMGetBufTgDate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt32 ) LMGetBufTgDate(void)                     TWOWORDINLINE(0x2EB8, 0x0304);


/*
 *  LMSetBufTgDate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetBufTgDate(SInt32 value)               TWOWORDINLINE(0x21DF, 0x0304);



#if CALL_NOT_IN_CARBON
/*
 *  LMGetLo3Bytes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt32 ) LMGetLo3Bytes(void)                      TWOWORDINLINE(0x2EB8, 0x031A);


/*
 *  LMSetLo3Bytes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetLo3Bytes(SInt32 value)                TWOWORDINLINE(0x21DF, 0x031A);



#endif  /* CALL_NOT_IN_CARBON */

/*
 *  LMGetMinStack()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt32 ) LMGetMinStack(void)                      TWOWORDINLINE(0x2EB8, 0x031E);


/*
 *  LMSetMinStack()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetMinStack(SInt32 value)                TWOWORDINLINE(0x21DF, 0x031E);



/*
 *  LMGetDefltStack()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt32 ) LMGetDefltStack(void)                    TWOWORDINLINE(0x2EB8, 0x0322);


/*
 *  LMSetDefltStack()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetDefltStack(SInt32 value)              TWOWORDINLINE(0x21DF, 0x0322);



/*
 *  LMGetGZRootHnd()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Handle ) LMGetGZRootHnd(void)                     TWOWORDINLINE(0x2EB8, 0x0328);


/*
 *  LMSetGZRootHnd()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetGZRootHnd(Handle value)               TWOWORDINLINE(0x21DF, 0x0328);



/*
 *  LMGetGZMoveHnd()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Handle ) LMGetGZMoveHnd(void)                     TWOWORDINLINE(0x2EB8, 0x0330);


/*
 *  LMSetGZMoveHnd()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetGZMoveHnd(Handle value)               TWOWORDINLINE(0x21DF, 0x0330);



/*
   LMGetFCBSPtr, LMSetFCBSPtr and LMSetFSFCBLen are not supported with Mac OS 9
   and later. Access to information in File Control Blocks or Fork Control Blocks
   (FCBs) should, if at all possible, be made with the GetFCBInfo or GetForkCBInfo
   routines. See the Technote "FCBs, Now and Forever" or the Technical Q&A
   "Accessing File Control Blocks" for complete information on this subject.
   Direct access to FCBs is not allowed by Carbon. Non-Carbon programs that
   require direct access to FCBs should use the File System Manager (FSM) FCB accessor
   functions if FSM is available (use the Gestalt selector gestaltFSAttr to determine
   this). Non-Carbon programs needing direct access to FCBs when FSM is not available
   can define ENABLE_FCB_ARRAY_ACCESS to be true when compiling.
*/
#ifndef ENABLE_FCB_ARRAY_ACCESS
#define ENABLE_FCB_ARRAY_ACCESS 0
#endif  /* !defined(ENABLE_FCB_ARRAY_ACCESS) */

#if ENABLE_FCB_ARRAY_ACCESS
#if CALL_NOT_IN_CARBON
/*
 *  LMGetFCBSPtr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Ptr ) LMGetFCBSPtr(void)                          TWOWORDINLINE(0x2EB8, 0x034E);


/*
 *  LMSetFCBSPtr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetFCBSPtr(Ptr value)                    TWOWORDINLINE(0x21DF, 0x034E);


/*
 *  LMSetFSFCBLen()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetFSFCBLen(SInt16 value)                TWOWORDINLINE(0x31DF, 0x03F6);


#endif  /* CALL_NOT_IN_CARBON */

#endif  /* ENABLE_FCB_ARRAY_ACCESS */

/*
   LMGetFSFCBLen is supported only for the purpose of determining that the HFS
   file system is available as documented in developer Technotes (the HFS file system
   is available in System 3.2 and later). There is no documented use of FSFCBLen
   other than testing it for a positive value.
*/
#if CALL_NOT_IN_CARBON
/*
 *  LMGetFSFCBLen()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetFSFCBLen(void)                      TWOWORDINLINE(0x3EB8, 0x03F6);



/*
 *  LMGetDefVCBPtr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Ptr ) LMGetDefVCBPtr(void)                        TWOWORDINLINE(0x2EB8, 0x0352);


/*
 *  LMSetDefVCBPtr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetDefVCBPtr(Ptr value)                  TWOWORDINLINE(0x21DF, 0x0352);



/*
 *  LMGetCurDirStore()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt32 ) LMGetCurDirStore(void)                   TWOWORDINLINE(0x2EB8, 0x0398);


/*
 *  LMSetCurDirStore()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetCurDirStore(SInt32 value)             TWOWORDINLINE(0x21DF, 0x0398);



#endif  /* CALL_NOT_IN_CARBON */

/*
 *  LMGetToExtFS()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( UniversalProcPtr ) LMGetToExtFS(void)             TWOWORDINLINE(0x2EB8, 0x03F2);


/*
 *  LMSetToExtFS()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetToExtFS(UniversalProcPtr value)       TWOWORDINLINE(0x21DF, 0x03F2);


#if CALL_NOT_IN_CARBON
/*
 *  LMGetScrnBase()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Ptr ) LMGetScrnBase(void)                         TWOWORDINLINE(0x2EB8, 0x0824);


/*
 *  LMSetScrnBase()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetScrnBase(Ptr value)                   TWOWORDINLINE(0x21DF, 0x0824);




/*
 *  LMGetCrsrBusy()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UInt8 ) LMGetCrsrBusy(void)                       TWOWORDINLINE(0x1EB8, 0x08CD);


/*
 *  LMSetCrsrBusy()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetCrsrBusy(UInt8 value)                 TWOWORDINLINE(0x11DF, 0x08CD);



/*
 *  LMGetJournalRef()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetJournalRef(void)                    TWOWORDINLINE(0x3EB8, 0x08E8);


/*
 *  LMSetJournalRef()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetJournalRef(SInt16 value)              TWOWORDINLINE(0x31DF, 0x08E8);



/*
 *  LMGetCrsrThresh()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetCrsrThresh(void)                    TWOWORDINLINE(0x3EB8, 0x08EC);


/*
 *  LMSetCrsrThresh()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetCrsrThresh(SInt16 value)              TWOWORDINLINE(0x31DF, 0x08EC);



/*
 *  LMGetJFetch()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UniversalProcPtr ) LMGetJFetch(void)              TWOWORDINLINE(0x2EB8, 0x08F4);


/*
 *  LMSetJFetch()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetJFetch(UniversalProcPtr value)        TWOWORDINLINE(0x21DF, 0x08F4);



#endif  /* CALL_NOT_IN_CARBON */

/*
 *  LMGetJStash()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( UniversalProcPtr ) LMGetJStash(void)              TWOWORDINLINE(0x2EB8, 0x08F8);


/*
 *  LMSetJStash()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetJStash(UniversalProcPtr value)        TWOWORDINLINE(0x21DF, 0x08F8);



#if CALL_NOT_IN_CARBON
/*
 *  LMGetJIODone()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UniversalProcPtr ) LMGetJIODone(void)             TWOWORDINLINE(0x2EB8, 0x08FC);


/*
 *  LMSetJIODone()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetJIODone(UniversalProcPtr value)       TWOWORDINLINE(0x21DF, 0x08FC);



#endif  /* CALL_NOT_IN_CARBON */

/*
 *  LMGetCurApRefNum()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt16 ) LMGetCurApRefNum(void)                   TWOWORDINLINE(0x3EB8, 0x0900);


/*
 *  LMSetCurApRefNum()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetCurApRefNum(SInt16 value)             TWOWORDINLINE(0x31DF, 0x0900);



#if CALL_NOT_IN_CARBON
/*
 *  LMGetCurrentA5()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Ptr ) LMGetCurrentA5(void)                        TWOWORDINLINE(0x2EB8, 0x0904);


/*
 *  LMSetCurrentA5()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetCurrentA5(Ptr value)                  TWOWORDINLINE(0x21DF, 0x0904);



#endif  /* CALL_NOT_IN_CARBON */

/*
 *  LMGetCurStackBase()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Ptr ) LMGetCurStackBase(void)                     TWOWORDINLINE(0x2EB8, 0x0908);


/*
 *  LMSetCurStackBase()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetCurStackBase(Ptr value)               TWOWORDINLINE(0x21DF, 0x0908);



#if CALL_NOT_IN_CARBON
/*
 *  LMGetCurJTOffset()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetCurJTOffset(void)                   TWOWORDINLINE(0x3EB8, 0x0934);


/*
 *  LMSetCurJTOffset()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetCurJTOffset(SInt16 value)             TWOWORDINLINE(0x31DF, 0x0934);



#endif  /* CALL_NOT_IN_CARBON */

/*
 *  LMGetCurPageOption()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt16 ) LMGetCurPageOption(void)                 TWOWORDINLINE(0x3EB8, 0x0936);


/*
 *  LMSetCurPageOption()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetCurPageOption(SInt16 value)           TWOWORDINLINE(0x31DF, 0x0936);




/*
 *  LMGetPrintErr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt16 ) LMGetPrintErr(void)                      TWOWORDINLINE(0x3EB8, 0x0944);


/*
 *  LMSetPrintErr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetPrintErr(SInt16 value)                TWOWORDINLINE(0x31DF, 0x0944);




/*  Carbon Scrap Manager does not support low memory.*/

#if CALL_NOT_IN_CARBON
/*
 *  LMGetScrapSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt32 ) LMGetScrapSize(void)                     TWOWORDINLINE(0x2EB8, 0x0960);


/*
 *  LMSetScrapSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetScrapSize(SInt32 value)               TWOWORDINLINE(0x21DF, 0x0960);


/*
 *  LMGetScrapHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Handle ) LMGetScrapHandle(void)                   TWOWORDINLINE(0x2EB8, 0x0964);


/*
 *  LMSetScrapHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetScrapHandle(Handle value)             TWOWORDINLINE(0x21DF, 0x0964);


/*
 *  LMGetScrapCount()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetScrapCount(void)                    TWOWORDINLINE(0x3EB8, 0x0968);


/*
 *  LMSetScrapCount()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetScrapCount(SInt16 value)              TWOWORDINLINE(0x31DF, 0x0968);


/*
 *  LMGetScrapState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetScrapState(void)                    TWOWORDINLINE(0x3EB8, 0x096A);


/*
 *  LMSetScrapState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetScrapState(SInt16 value)              TWOWORDINLINE(0x31DF, 0x096A);


/*
 *  LMGetScrapName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( StringPtr ) LMGetScrapName(void)                  TWOWORDINLINE(0x2EB8, 0x096C);


/*
 *  LMSetScrapName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetScrapName(StringPtr value)            TWOWORDINLINE(0x21DF, 0x096C);



/*
 *  LMGetROMFont0()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Handle ) LMGetROMFont0(void)                      TWOWORDINLINE(0x2EB8, 0x0980);


/*
 *  LMSetROMFont0()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetROMFont0(Handle value)                TWOWORDINLINE(0x21DF, 0x0980);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  LMGetApFontID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt16 ) LMGetApFontID(void)                      TWOWORDINLINE(0x3EB8, 0x0984);


/*
 *  LMSetApFontID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetApFontID(SInt16 value)                TWOWORDINLINE(0x31DF, 0x0984);




/* Carbon versions of the Window Manager do not support LowMem. */
/* Carbon Usage: use GetWindowList*/
#if CALL_NOT_IN_CARBON
/*
 *  LMGetWindowList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( WindowRef ) LMGetWindowList(void)                 TWOWORDINLINE(0x2EB8, 0x09D6);


/*
 *  LMGetSaveUpdate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetSaveUpdate(void)                    TWOWORDINLINE(0x3EB8, 0x09DA);


/*
 *  LMSetSaveUpdate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetSaveUpdate(SInt16 value)              TWOWORDINLINE(0x31DF, 0x09DA);


/*
 *  LMGetPaintWhite()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetPaintWhite(void)                    TWOWORDINLINE(0x3EB8, 0x09DC);


/* Carbon Usage : use InstallWindowContentPaintProc*/
/*
 *  LMSetPaintWhite()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetPaintWhite(SInt16 value)              TWOWORDINLINE(0x31DF, 0x09DC);


/*
 *  LMGetWMgrPort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( GrafPtr ) LMGetWMgrPort(void)                     TWOWORDINLINE(0x2EB8, 0x09DE);


/*
 *  LMSetWMgrPort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetWMgrPort(GrafPtr value)               TWOWORDINLINE(0x21DF, 0x09DE);


/* Carbon Usage: use GetGrayRgn*/
/*
 *  LMGetGrayRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( RgnHandle ) LMGetGrayRgn(void)                    TWOWORDINLINE(0x2EB8, 0x09EE);


/*
 *  LMGetDragHook()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( DragGrayRgnUPP ) LMGetDragHook(void)              TWOWORDINLINE(0x2EB8, 0x09F6);


/*
 *  LMSetDragHook()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetDragHook(DragGrayRgnUPP value)        TWOWORDINLINE(0x21DF, 0x09F6);


/*
 *  LMSetWindowList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetWindowList(WindowRef value)           TWOWORDINLINE(0x21DF, 0x09D6);


/*
 *  LMGetGhostWindow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( WindowRef ) LMGetGhostWindow(void)                TWOWORDINLINE(0x2EB8, 0x0A84);


/*
 *  LMSetGhostWindow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetGhostWindow(WindowRef value)          TWOWORDINLINE(0x21DF, 0x0A84);


#endif  /* CALL_NOT_IN_CARBON */

#if !OPAQUE_TOOLBOX_STRUCTS
#if CALL_NOT_IN_CARBON
/*
 *  LMGetAuxWinHead()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( AuxWinHandle ) LMGetAuxWinHead(void)              TWOWORDINLINE(0x2EB8, 0x0CD0);


/*
 *  LMSetAuxWinHead()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetAuxWinHead(AuxWinHandle value)        TWOWORDINLINE(0x21DF, 0x0CD0);


#endif  /* CALL_NOT_IN_CARBON */

#endif  /* !OPAQUE_TOOLBOX_STRUCTS */

#if CALL_NOT_IN_CARBON
/*
 *  LMGetCurActivate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( WindowRef ) LMGetCurActivate(void)                TWOWORDINLINE(0x2EB8, 0x0A64);


/*
 *  LMSetCurActivate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetCurActivate(WindowRef value)          TWOWORDINLINE(0x21DF, 0x0A64);


/*
 *  LMGetCurDeactive()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( WindowRef ) LMGetCurDeactive(void)                TWOWORDINLINE(0x2EB8, 0x0A68);


/*
 *  LMSetCurDeactive()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetCurDeactive(WindowRef value)          TWOWORDINLINE(0x21DF, 0x0A68);


/*
 *  LMGetOldStructure()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( RgnHandle ) LMGetOldStructure(void)               TWOWORDINLINE(0x2EB8, 0x09E6);


/*
 *  LMSetOldStructure()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetOldStructure(RgnHandle value)         TWOWORDINLINE(0x21DF, 0x09E6);


/*
 *  LMGetOldContent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( RgnHandle ) LMGetOldContent(void)                 TWOWORDINLINE(0x2EB8, 0x09EA);


/*
 *  LMSetOldContent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetOldContent(RgnHandle value)           TWOWORDINLINE(0x21DF, 0x09EA);


/*
 *  LMSetGrayRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetGrayRgn(RgnHandle value)              TWOWORDINLINE(0x21DF, 0x09EE);


/*
 *  LMGetSaveVisRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( RgnHandle ) LMGetSaveVisRgn(void)                 TWOWORDINLINE(0x2EB8, 0x09F2);


/*
 *  LMSetSaveVisRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetSaveVisRgn(RgnHandle value)           TWOWORDINLINE(0x21DF, 0x09F2);



#endif  /* CALL_NOT_IN_CARBON */

/*
 *  LMGetOneOne()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt32 ) LMGetOneOne(void)                        TWOWORDINLINE(0x2EB8, 0x0A02);


/*
 *  LMSetOneOne()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetOneOne(SInt32 value)                  TWOWORDINLINE(0x21DF, 0x0A02);



/*
 *  LMGetMinusOne()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt32 ) LMGetMinusOne(void)                      TWOWORDINLINE(0x2EB8, 0x0A06);


/*
 *  LMSetMinusOne()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetMinusOne(SInt32 value)                TWOWORDINLINE(0x21DF, 0x0A06);



/* Carbon Usage: use GetMenuTrackingData*/
#if CALL_NOT_IN_CARBON
/*
 *  LMGetTopMenuItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetTopMenuItem(void)                   TWOWORDINLINE(0x3EB8, 0x0A0A);


/* Carbon Usage: replaced by MDEF messages and GetMenuTrackingData API*/
/*
 *  LMSetTopMenuItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetTopMenuItem(SInt16 value)             TWOWORDINLINE(0x31DF, 0x0A0A);



/* Carbon Usage: use GetMenuTrackingData*/
/*
 *  LMGetAtMenuBottom()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetAtMenuBottom(void)                  TWOWORDINLINE(0x3EB8, 0x0A0C);


/* Carbon Usage: replaced by MDEF messages and GetMenuTrackingData API*/
/*
 *  LMSetAtMenuBottom()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetAtMenuBottom(SInt16 value)            TWOWORDINLINE(0x31DF, 0x0A0C);



/*
   Carbon usage: use GetMenuBar (which returns a newly allocated handle in
   the same format as that returned by LMGetMenuList; dispose with DisposeMenuBar)
   or GetRootMenu.
*/
/*
 *  LMGetMenuList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Handle ) LMGetMenuList(void)                      TWOWORDINLINE(0x2EB8, 0x0A1C);


/*
 *  LMSetMenuList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetMenuList(Handle value)                TWOWORDINLINE(0x21DF, 0x0A1C);



/* Carbon usage: no replacement*/
/*
 *  LMGetMBarEnable()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetMBarEnable(void)                    TWOWORDINLINE(0x3EB8, 0x0A20);


/*
 *  LMSetMBarEnable()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetMBarEnable(SInt16 value)              TWOWORDINLINE(0x31DF, 0x0A20);



/* Carbon usage: no replacement*/
/*
 *  LMGetMenuFlash()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetMenuFlash(void)                     TWOWORDINLINE(0x3EB8, 0x0A24);


/*
 *  LMSetMenuFlash()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetMenuFlash(SInt16 value)               TWOWORDINLINE(0x31DF, 0x0A24);



/* LMGetTheMenu() moved to Menus.h */
/*
 *  LMSetTheMenu()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetTheMenu(SInt16 value)                 TWOWORDINLINE(0x31DF, 0x0A26);


/*
 *  LMGetMBarHook()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( MBarHookUPP ) LMGetMBarHook(void)                 TWOWORDINLINE(0x2EB8, 0x0A2C);


/*
 *  LMSetMBarHook()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetMBarHook(MBarHookUPP value)           TWOWORDINLINE(0x21DF, 0x0A2C);



/*
 *  LMGetMenuHook()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( MenuHookUPP ) LMGetMenuHook(void)                 TWOWORDINLINE(0x2EB8, 0x0A30);


/*
 *  LMSetMenuHook()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetMenuHook(MenuHookUPP value)           TWOWORDINLINE(0x21DF, 0x0A30);


/*
 *  LMGetTopMapHndl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Handle ) LMGetTopMapHndl(void)                    TWOWORDINLINE(0x2EB8, 0x0A50);


/*
 *  LMSetTopMapHndl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetTopMapHndl(Handle value)              TWOWORDINLINE(0x21DF, 0x0A50);



/*
 *  LMGetSysMapHndl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Handle ) LMGetSysMapHndl(void)                    TWOWORDINLINE(0x2EB8, 0x0A54);


/*
 *  LMSetSysMapHndl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetSysMapHndl(Handle value)              TWOWORDINLINE(0x21DF, 0x0A54);



#endif  /* CALL_NOT_IN_CARBON */

/*
 *  LMGetSysMap()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt16 ) LMGetSysMap(void)                        TWOWORDINLINE(0x3EB8, 0x0A58);


/*
 *  LMSetSysMap()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetSysMap(SInt16 value)                  TWOWORDINLINE(0x31DF, 0x0A58);




#if CALL_NOT_IN_CARBON
/*
 *  LMGetCurMap()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetCurMap(void)                        TWOWORDINLINE(0x3EB8, 0x0A5A);


/*
 *  LMSetCurMap()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetCurMap(SInt16 value)                  TWOWORDINLINE(0x31DF, 0x0A5A);



#endif  /* CALL_NOT_IN_CARBON */

/*
 *  LMGetResLoad()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( UInt8 ) LMGetResLoad(void)                        TWOWORDINLINE(0x1EB8, 0x0A5E);


/*
 *  LMSetResLoad()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetResLoad(UInt8 value)                  TWOWORDINLINE(0x11DF, 0x0A5E);



/*
 *  LMGetResErr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt16 ) LMGetResErr(void)                        TWOWORDINLINE(0x3EB8, 0x0A60);


/*
 *  LMSetResErr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetResErr(SInt16 value)                  TWOWORDINLINE(0x31DF, 0x0A60);



#if CALL_NOT_IN_CARBON
/*
 *  LMGetFScaleDisable()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UInt8 ) LMGetFScaleDisable(void)                  TWOWORDINLINE(0x1EB8, 0x0A63);


/*
 *  LMSetFScaleDisable()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetFScaleDisable(UInt8 value)            TWOWORDINLINE(0x11DF, 0x0A63);




/*
 *  LMGetDeskHook()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UniversalProcPtr ) LMGetDeskHook(void)            TWOWORDINLINE(0x2EB8, 0x0A6C);


/*
 *  LMSetDeskHook()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetDeskHook(UniversalProcPtr value)      TWOWORDINLINE(0x21DF, 0x0A6C);



/* Carbon Usage: Use TEGetDoTextHook.*/
/*
 *  LMGetTEDoText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UniversalProcPtr ) LMGetTEDoText(void)            TWOWORDINLINE(0x2EB8, 0x0A70);


/* Carbon Usage: Use TESetDoTextHook.*/
/*
 *  LMSetTEDoText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetTEDoText(UniversalProcPtr value)      TWOWORDINLINE(0x21DF, 0x0A70);



/* Carbon Usage: Use TEGetRecalcHook.*/
/*
 *  LMGetTERecal()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UniversalProcPtr ) LMGetTERecal(void)             TWOWORDINLINE(0x2EB8, 0x0A74);


/* Carbon Usage: Use TESetRecalcHook.*/
/*
 *  LMSetTERecal()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetTERecal(UniversalProcPtr value)       TWOWORDINLINE(0x21DF, 0x0A74);


/*
 *  LMGetResumeProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UniversalProcPtr ) LMGetResumeProc(void)          TWOWORDINLINE(0x2EB8, 0x0A8C);


/*
 *  LMSetResumeProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetResumeProc(UniversalProcPtr value)    TWOWORDINLINE(0x21DF, 0x0A8C);


/*
 *  LMGetANumber()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetANumber(void)                       TWOWORDINLINE(0x3EB8, 0x0A98);


/*
 *  LMSetANumber()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetANumber(SInt16 value)                 TWOWORDINLINE(0x31DF, 0x0A98);


/* Carbon Usage: Use GetAlertStage.*/
/*
 *  LMGetACount()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetACount(void)                        TWOWORDINLINE(0x3EB8, 0x0A9A);


/* Carbon Usage: Use ResetAlertStage.*/
/*
 *  LMSetACount()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetACount(SInt16 value)                  TWOWORDINLINE(0x31DF, 0x0A9A);


/*
 *  LMGetDABeeper()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UniversalProcPtr ) LMGetDABeeper(void)            TWOWORDINLINE(0x2EB8, 0x0A9C);


/*
 *  LMSetDABeeper()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetDABeeper(UniversalProcPtr value)      TWOWORDINLINE(0x21DF, 0x0A9C);


/* Carbon Usage: use TEGetScrapLength*/
/*
 *  LMGetTEScrpLength()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UInt16 ) LMGetTEScrpLength(void)                  TWOWORDINLINE(0x3EB8, 0x0AB0);


/* Carbon Usage: use TESetScrapLength*/
/*
 *  LMSetTEScrpLength()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetTEScrpLength(UInt16 value)            TWOWORDINLINE(0x31DF, 0x0AB0);



/* Carbon Usage: use TEGetScrapHandle*/
/*
 *  LMGetTEScrpHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Handle ) LMGetTEScrpHandle(void)                  TWOWORDINLINE(0x2EB8, 0x0AB4);


/* Carbon Usage: use TESetScrapHandle*/
/*
 *  LMSetTEScrpHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetTEScrpHandle(Handle value)            TWOWORDINLINE(0x21DF, 0x0AB4);



/*
 *  LMGetAppParmHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Handle ) LMGetAppParmHandle(void)                 TWOWORDINLINE(0x2EB8, 0x0AEC);


/*
 *  LMSetAppParmHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetAppParmHandle(Handle value)           TWOWORDINLINE(0x21DF, 0x0AEC);



/*
 *  LMGetDSErrCode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetDSErrCode(void)                     TWOWORDINLINE(0x3EB8, 0x0AF0);


/*
 *  LMSetDSErrCode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetDSErrCode(SInt16 value)               TWOWORDINLINE(0x31DF, 0x0AF0);



/*
 *  LMGetResErrProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ResErrUPP ) LMGetResErrProc(void)                 TWOWORDINLINE(0x2EB8, 0x0AF2);


/*
 *  LMSetResErrProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetResErrProc(ResErrUPP value)           TWOWORDINLINE(0x21DF, 0x0AF2);



/*
 *  LMGetDlgFont()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetDlgFont(void)                       TWOWORDINLINE(0x3EB8, 0x0AFA);


/* Carbon Usage: use SetDialogFont*/
/*
 *  LMSetDlgFont()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetDlgFont(SInt16 value)                 TWOWORDINLINE(0x31DF, 0x0AFA);


/*
 *  LMGetATalkHk2()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Ptr ) LMGetATalkHk2(void)                         TWOWORDINLINE(0x2EB8, 0x0B18);


/*
 *  LMSetATalkHk2()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetATalkHk2(Ptr value)                   TWOWORDINLINE(0x21DF, 0x0B18);



/*
 *  LMGetHWCfgFlags()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetHWCfgFlags(void)                    TWOWORDINLINE(0x3EB8, 0x0B22);


/*
 *  LMSetHWCfgFlags()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetHWCfgFlags(SInt16 value)              TWOWORDINLINE(0x31DF, 0x0B22);




/* Carbon Usage: use GetMenuTrackingData*/
/*
 *  LMGetMenuDisable()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt32 ) LMGetMenuDisable(void)                   TWOWORDINLINE(0x2EB8, 0x0B54);


/* Carbon Usage: use new MDEF messages*/
/*
 *  LMSetMenuDisable()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetMenuDisable(SInt32 value)             TWOWORDINLINE(0x21DF, 0x0B54);



/*
 *  LMGetROMMapInsert()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UInt8 ) LMGetROMMapInsert(void)                   TWOWORDINLINE(0x1EB8, 0x0B9E);


/*
 *  LMSetROMMapInsert()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetROMMapInsert(UInt8 value)             TWOWORDINLINE(0x11DF, 0x0B9E);



#endif  /* CALL_NOT_IN_CARBON */

/*
 *  LMGetTmpResLoad()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( UInt8 ) LMGetTmpResLoad(void)                     TWOWORDINLINE(0x1EB8, 0x0B9F);


/*
 *  LMSetTmpResLoad()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetTmpResLoad(UInt8 value)               TWOWORDINLINE(0x11DF, 0x0B9F);



/*
 *  LMGetIntlSpec()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Ptr ) LMGetIntlSpec(void)                         TWOWORDINLINE(0x2EB8, 0x0BA0);


/*
 *  LMSetIntlSpec()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetIntlSpec(Ptr value)                   TWOWORDINLINE(0x21DF, 0x0BA0);


/* LMGetWordRedraw and LMSetWordRedraw moved to TextEdit.h */
/*
 *  LMGetSysFontFam()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt16 ) LMGetSysFontFam(void)                    TWOWORDINLINE(0x3EB8, 0x0BA6);


/*
 *  LMSetSysFontFam()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetSysFontFam(SInt16 value)              TWOWORDINLINE(0x31DF, 0x0BA6);


/*
 *  LMGetSysFontSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt16 ) LMGetSysFontSize(void)                   TWOWORDINLINE(0x3EB8, 0x0BA8);


/*
 *  LMSetSysFontSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetSysFontSize(SInt16 value)             TWOWORDINLINE(0x31DF, 0x0BA8);



/* Carbon Usge: use GetMBarHeight*/
#if CALL_NOT_IN_CARBON
/*
 *  LMGetMBarHeight()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetMBarHeight(void)                    TWOWORDINLINE(0x3EB8, 0x0BAA);


/* Carbon Usage: use Hide/ShowMenuBar*/
/*
 *  LMSetMBarHeight()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetMBarHeight(SInt16 value)              TWOWORDINLINE(0x31DF, 0x0BAA);



/*
 *  LMGetTESysJust()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetTESysJust(void)                     TWOWORDINLINE(0x3EB8, 0x0BAC);


/*
 *  LMSetTESysJust()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetTESysJust(SInt16 value)               TWOWORDINLINE(0x31DF, 0x0BAC);




/*
 *  LMGetMMU32Bit()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UInt8 ) LMGetMMU32Bit(void)                       TWOWORDINLINE(0x1EB8, 0x0CB2);


/*
 *  LMSetMMU32Bit()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetMMU32Bit(UInt8 value)                 TWOWORDINLINE(0x11DF, 0x0CB2);




/*
 *  LMGetDeskCPat()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( PixPatHandle ) LMGetDeskCPat(void)                TWOWORDINLINE(0x2EB8, 0x0CD8);


/*
 *  LMSetDeskCPat()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetDeskCPat(PixPatHandle value)          TWOWORDINLINE(0x21DF, 0x0CD8);



/*
 *  LMGetTimeDBRA()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetTimeDBRA(void)                      TWOWORDINLINE(0x3EB8, 0x0D00);


/*
 *  LMSetTimeDBRA()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetTimeDBRA(SInt16 value)                TWOWORDINLINE(0x31DF, 0x0D00);



/*
 *  LMGetTimeSCCDB()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetTimeSCCDB(void)                     TWOWORDINLINE(0x3EB8, 0x0D02);


/*
 *  LMSetTimeSCCDB()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetTimeSCCDB(SInt16 value)               TWOWORDINLINE(0x31DF, 0x0D02);



/*
 *  LMGetJVBLTask()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UniversalProcPtr ) LMGetJVBLTask(void)            TWOWORDINLINE(0x2EB8, 0x0D28);


/*
 *  LMSetJVBLTask()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetJVBLTask(UniversalProcPtr value)      TWOWORDINLINE(0x21DF, 0x0D28);



/*
 *  LMGetSynListHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Handle ) LMGetSynListHandle(void)                 TWOWORDINLINE(0x2EB8, 0x0D32);


/*
 *  LMSetSynListHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetSynListHandle(Handle value)           TWOWORDINLINE(0x21DF, 0x0D32);



/*
 *  LMGetMenuCInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( MCTableHandle ) LMGetMenuCInfo(void)              TWOWORDINLINE(0x2EB8, 0x0D50);


/*
 *  LMSetMenuCInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetMenuCInfo(MCTableHandle value)        TWOWORDINLINE(0x21DF, 0x0D50);



/*
 *  LMGetJDTInstall()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UniversalProcPtr ) LMGetJDTInstall(void)          TWOWORDINLINE(0x2EB8, 0x0D9C);


/*
 *  LMSetJDTInstall()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetJDTInstall(UniversalProcPtr value)    TWOWORDINLINE(0x21DF, 0x0D9C);



/*
 *  LMGetTimeSCSIDB()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( SInt16 ) LMGetTimeSCSIDB(void)                    TWOWORDINLINE(0x3EB8, 0x0B24);


/*
 *  LMSetTimeSCSIDB()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetTimeSCSIDB(SInt16 value)              TWOWORDINLINE(0x31DF, 0x0B24);




/**************************************************************************************

    MORE COMPLEX LOWMEM ACCESSORS

**************************************************************************************/
#endif  /* CALL_NOT_IN_CARBON */

#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
/**************************************************************************************
    "BIG DATA"
    
        These lowmem accessors access big (> 4 bytes) values.
**************************************************************************************/
#define LMGetDSAlertRect(dsAlertRectValue)  (*(dsAlertRectValue) = * (Rect*) 0x03F8)
#define LMSetDSAlertRect(dsAlertRectValue)  ((* (Rect *) 0x03F8) = *(dsAlertRectValue))
#define LMGetDragPattern(dragPatternValue)  (*(dragPatternValue) = * (Pattern *) 0x0A34)
#define LMSetDragPattern(dragPatternValue)  ((* (Pattern *) 0x0A34) = *(dragPatternValue))
#define LMGetDeskPattern(deskPatternValue)  (*(deskPatternValue) = * (Pattern *) 0x0A3C)
#define LMSetDeskPattern(deskPatternValue)  ((* (Pattern *) 0x0A3C) = *(deskPatternValue))
#define LMGetEventQueue()   ( (QHdrPtr) 0x014A)
#define LMSetEventQueue(eventQueueValue)    ((* (QHdrPtr) 0x014A) = *(QHdrPtr)(eventQueueValue))
#define LMGetVBLQueue() ( (QHdrPtr) 0x0160)
#define LMSetVBLQueue(vblQueueValue)    ((* (QHdrPtr) 0x0160) = *(QHdrPtr)(vblQueueValue))
#define LMGetDrvQHdr()  ( (QHdrPtr) 0x0308)
#define LMSetDrvQHdr(drvQHdrValue)  ((* (QHdrPtr) 0x0308) = *(QHdrPtr)(drvQHdrValue))
#define LMGetVCBQHdr()  ( (QHdrPtr) 0x0356)
#define LMSetVCBQHdr(vcbQHdrValue)  ((* (QHdrPtr) 0x0356) = *(QHdrPtr)(vcbQHdrValue))
#define LMGetDTQueue()  ( (QHdrPtr) 0x0D92)
#define LMSetDTQueue(dtQueueValue)  ((* (QHdrPtr) 0x0D92) = *(QHdrPtr)(dtQueueValue))
#define LMGetFSQHdr()   ( (QHdrPtr) 0x0360)
/**************************************************************************************
    "BLOCKMOVE ACCESSORS"
    
        These lowmem accessors use the BlockMove to set
**************************************************************************************/
#define LMGetCurApName()    ((StringPtr) 0x0910)
#define LMSetCurApName(curApNameValue)  (BlockMoveData((Ptr)(curApNameValue), (Ptr)0x0910, sizeof(Str31)))
#define LMGetSysResName()   ( (StringPtr) 0x0AD8)
#define LMSetSysResName(sysResNameValue)    (BlockMoveData((Ptr)(sysResNameValue), (Ptr)0x0AD8, sizeof(Str15)))
#define LMGetFinderName()   ((StringPtr)0x02E0)
#define LMSetFinderName(finderName) (BlockMoveData((Ptr)(finderName), (Ptr)0x02E0, sizeof(Str15)))
#define LMGetScratch20()    ((Ptr) 0x01E4)
#define LMSetScratch20(scratch20Value)  (BlockMoveData((Ptr) (scratch20Value), (Ptr) 0x01E4, 20))
#define LMGetToolScratch()  ((Ptr) 0x09CE)
#define LMSetToolScratch(toolScratchValue)  (BlockMoveData((Ptr)(toolScratchValue), (Ptr) 0x09CE, 8))
#define LMGetApplScratch()  ((Ptr) 0x0A78)
#define LMSetApplScratch(applScratchValue)  (BlockMoveData((Ptr) (applScratchValue), (Ptr) 0x0A78, 12))
/**************************************************************************************
    "INDEXED ACCESSORS"
    
        These lowmem accessors take an index parameter to get/set an indexed
        lowmem global.
**************************************************************************************/
/*************************************************************************************
    The DAString accessors are gone with Carbon. Please use ParamText and
    GetParamText instead.
**************************************************************************************/
#define LMGetDAStrings(whichString) ( ((StringHandle*)0x0AA0)[whichString] )
#define LMSetDAStrings(stringsValue, whichString) ( ((StringHandle*)0x0AA0)[whichString] = (stringsValue) )
#define LMGetLvl2DT(vectorNumber)   ( ((UniversalProcPtr*)0x01B2)[vectorNumber] )
#define LMSetLvl2DT(lvl2DTValue, vectorNumber)  ( ((UniversalProcPtr*)0x01B2)[vectorNumber] = (lvl2DTValue) )
#define LMGetExtStsDT(vectorNumber) ( ((UniversalProcPtr*)0x02BE)[vectorNumber] )
#define LMSetExtStsDT(extStsDTValue, vectorNumber)  ( ((UniversalProcPtr*)0x02BE)[vectorNumber] = (extStsDTValue) )

#else
/**************************************************************************************
    "BIG DATA"
    
        These lowmem accessors access big (> 4 bytes) values.
**************************************************************************************/

#if CALL_NOT_IN_CARBON
/*
 *  LMGetDSAlertRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMGetDSAlertRect(Rect * dsAlertRectValue);


/*
 *  LMSetDSAlertRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetDSAlertRect(const Rect * dsAlertRectValue);


/*
 *  LMGetDragPattern()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMGetDragPattern(Pattern * dragPatternValue);


/*
 *  LMSetDragPattern()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetDragPattern(const Pattern * dragPatternValue);


/*
 *  LMGetDeskPattern()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMGetDeskPattern(Pattern * deskPatternValue);


/*
 *  LMSetDeskPattern()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetDeskPattern(const Pattern * deskPatternValue);


/*
 *  LMGetEventQueue()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( QHdrPtr ) LMGetEventQueue(void);


/*
 *  LMSetEventQueue()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetEventQueue(QHdrPtr eventQueueValue);



/*
 *  LMGetVBLQueue()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( QHdrPtr ) LMGetVBLQueue(void);


/*
 *  LMSetVBLQueue()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetVBLQueue(QHdrPtr vblQueueValue);


/*
 *  LMGetDrvQHdr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( QHdrPtr ) LMGetDrvQHdr(void);


/*
 *  LMSetDrvQHdr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetDrvQHdr(QHdrPtr drvQHdrValue);


/*
 *  LMGetVCBQHdr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( QHdrPtr ) LMGetVCBQHdr(void);


/*
 *  LMSetVCBQHdr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetVCBQHdr(QHdrPtr vcbQHdrValue);


/*
 *  LMGetDTQueue()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( QHdrPtr ) LMGetDTQueue(void);


/*
 *  LMSetDTQueue()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetDTQueue(QHdrPtr dtQueueValue);


/*
 *  LMGetFSQHdr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( QHdrPtr ) LMGetFSQHdr(void);


/**************************************************************************************
    "BLOCKMOVE ACCESSORS"
    
        These lowmem accessors use the BlockMove to set
**************************************************************************************/
#endif  /* CALL_NOT_IN_CARBON */

/*
 *  LMGetCurApName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( StringPtr ) LMGetCurApName(void);


/*
 *  LMSetCurApName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetCurApName(ConstStr31Param curApNameValue);


/*
 *  LMGetSysResName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( StringPtr ) LMGetSysResName(void);


/*
 *  LMSetSysResName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetSysResName(ConstStr15Param sysResNameValue);


/*
 *  LMGetFinderName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( StringPtr ) LMGetFinderName(void);


/*
 *  LMSetFinderName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetFinderName(ConstStr15Param finderNameValue);


#if CALL_NOT_IN_CARBON
/*
 *  LMGetScratch20()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Ptr ) LMGetScratch20(void);


/*
 *  LMSetScratch20()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetScratch20(const void * scratch20Value);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  LMGetToolScratch()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Ptr ) LMGetToolScratch(void);


/*
 *  LMSetToolScratch()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetToolScratch(const void * toolScratchValue);


#if CALL_NOT_IN_CARBON
/*
 *  LMGetApplScratch()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Ptr ) LMGetApplScratch(void);


/*
 *  LMSetApplScratch()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetApplScratch(const void * applScratchValue);



/**************************************************************************************
    "INDEXED ACCESSORS"
    
        These lowmem accessors take an index parameter to get/set an indexed
        lowmem global.
**************************************************************************************/

/* Carbon Usage: use GetParamText*/
/*
 *  LMGetDAStrings()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( StringHandle ) LMGetDAStrings(short whichString);


/* Carbon Usage: use ParamText*/
/*
 *  LMSetDAStrings()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetDAStrings(StringHandle stringsValue, short whichString);



#endif  /* CALL_NOT_IN_CARBON */

/*
 *  LMGetLvl2DT()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( UniversalProcPtr ) LMGetLvl2DT(short vectorNumber);


/*
 *  LMSetLvl2DT()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetLvl2DT(UniversalProcPtr Lvl2DTValue, short vectorNumber);


#if CALL_NOT_IN_CARBON
/*
 *  LMGetExtStsDT()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UniversalProcPtr ) LMGetExtStsDT(short vectorNumber);


/*
 *  LMSetExtStsDT()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetExtStsDT(UniversalProcPtr ExtStsDTValue, short vectorNumber);


#endif  /* CALL_NOT_IN_CARBON */

#endif  /* TARGET_CPU_68K && !TARGET_RT_MAC_CFM */

/**************************************************************************************
    "Missing Accessors"
    
        These lowmem accessors are not in the original InterfaceLib.  They were
        added to InterfaceLib in Mac OS 8.5.  In Universal Interfaces 3.2 they
        were defined via a macro. In you want to use these functions on a pre-8.5
        systems, you must write your own macros to override the function prototype
        or write your own implementation.
    
**************************************************************************************/


#if CALL_NOT_IN_CARBON
/*
 *  LMSetMouseTemp()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetMouseTemp(Point value)                TWOWORDINLINE(0x21DF, 0x0828);



/* accesses "MTemp"*/
/*
 *  LMGetMouseTemp()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Point ) LMGetMouseTemp(void)                      TWOWORDINLINE(0x2EB8, 0x0828);


/*
   accesses "MBState"
   Carbon Usage: use Button()
*/
/*
 *  LMGetMouseButtonState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UInt8 ) LMGetMouseButtonState(void)               TWOWORDINLINE(0x1EB8, 0x0172);


/*
 *  LMSetMouseButtonState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetMouseButtonState(UInt8 value)         TWOWORDINLINE(0x11DF, 0x0172);


/*
   accesses "RawMouse"
   Carbon Usage: use GetGlobalMouse
*/
/*
 *  LMGetRawMouseLocation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Point ) LMGetRawMouseLocation(void)               TWOWORDINLINE(0x2EB8, 0x082C);


/* Carbon Usage: use GetGlobalMouse*/
/*
 *  LMSetRawMouseLocation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetRawMouseLocation(Point value)         TWOWORDINLINE(0x21DF, 0x082C);



/*
   accesses "Mouse"
   Carbon Usage: use GetGlobalMouse
*/
/*
 *  LMGetMouseLocation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Point ) LMGetMouseLocation(void)                  TWOWORDINLINE(0x2EB8, 0x0830);


/*
 *  LMSetMouseLocation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetMouseLocation(Point value)            TWOWORDINLINE(0x21DF, 0x0830);


/* accesses "TheCrsr"*/
/*
 *  LMGetTheCursor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void ) LMGetTheCursor(Cursor * currentCursor);


/*
 *  LMSetTheCursor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void ) LMSetTheCursor(const Cursor * newCursor);



/* accesses "HiHeapMark"*/
#endif  /* CALL_NOT_IN_CARBON */

/*
 *  LMGetHighHeapMark()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Ptr ) LMGetHighHeapMark(void)                     TWOWORDINLINE(0x2EB8, 0x0BAE);


/*
 *  LMSetHighHeapMark()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetHighHeapMark(Ptr value)               TWOWORDINLINE(0x21DF, 0x0BAE);



/* accesses "StkLowPt"*/
/*
 *  LMGetStackLowPoint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Ptr ) LMGetStackLowPoint(void)                    TWOWORDINLINE(0x2EB8, 0x0110);


/*
 *  LMSetStackLowPoint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetStackLowPoint(Ptr value)              TWOWORDINLINE(0x21DF, 0x0110);



/* accesses "ROMMapHndl"*/
#if CALL_NOT_IN_CARBON
/*
 *  LMGetROMMapHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Handle ) LMGetROMMapHandle(void)                  TWOWORDINLINE(0x2EB8, 0x0B06);


/*
 *  LMSetROMMapHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetROMMapHandle(Handle value)            TWOWORDINLINE(0x21DF, 0x0B06);



/* accesses "UnitNtryCnt"*/
/*
 *  LMGetUnitTableEntryCount()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short ) LMGetUnitTableEntryCount(void)            TWOWORDINLINE(0x3EB8, 0x01D2);


/*
 *  LMSetUnitTableEntryCount()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetUnitTableEntryCount(short value)      TWOWORDINLINE(0x31DF, 0x01D2);



/* accesses "FmtDefaults"*/
#endif  /* CALL_NOT_IN_CARBON */

/*
 *  LMGetDiskFormatingHFSDefaults()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Ptr ) LMGetDiskFormatingHFSDefaults(void)         TWOWORDINLINE(0x2EB8, 0x039E);


/*
 *  LMSetDiskFormatingHFSDefaults()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void ) LMSetDiskFormatingHFSDefaults(Ptr value)   TWOWORDINLINE(0x21DF, 0x039E);



/* accesses "PortAUse"*/
#if CALL_NOT_IN_CARBON
/*
 *  LMGetPortAInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UInt8 ) LMGetPortAInfo(void)                      TWOWORDINLINE(0x1EB8, 0x0290);


/*
 *  LMSetPortAInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetPortAInfo(UInt8 value)                TWOWORDINLINE(0x11DF, 0x0290);



/*
 *  LMGetMBTicks()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( long ) LMGetMBTicks(void)                         TWOWORDINLINE(0x2EB8, 0x016E);


/*
 *  LMSetMBTicks()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetMBTicks(long value)                   TWOWORDINLINE(0x21DF, 0x016E);



/*
 *  LMGetKeyTime()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( long ) LMGetKeyTime(void)                         TWOWORDINLINE(0x2EB8, 0x0186);


/*
 *  LMSetKeyTime()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void ) LMSetKeyTime(long value)                   TWOWORDINLINE(0x21DF, 0x0186);




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

#endif /* __LOWMEM__ */

