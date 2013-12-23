/*
     File:       ControlManagerComponent.h
 
     Contains:   QuickTime Interfaces.
 
     Version:    Technology: QuickTime 6.0
                 Release:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1990-2002 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __CONTROLMANAGERCOMPONENT__
#define __CONTROLMANAGERCOMPONENT__

#ifndef __COMPONENTS__
#include <Components.h>
#endif

#ifndef __CONTROLS__
#include <Controls.h>
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

enum {
  kCTRLComponentType            = FOUR_CHAR_CODE('ctrl'),
  kCTRLComponentBaseSubType     = FOUR_CHAR_CODE('ctrl'),
  kCTRLComponentButtonSubType   = FOUR_CHAR_CODE('butn'),
  kCTRLComponentRadioSubType    = FOUR_CHAR_CODE('radi'),
  kCTRLComponentCheckSubType    = FOUR_CHAR_CODE('chec'),
  kCTRLComponentScrollSubType   = FOUR_CHAR_CODE('scro'),
  kCTRLComponentStaticTextSubType = FOUR_CHAR_CODE('stx '),
  kCTRLComponentEditTextSubType = FOUR_CHAR_CODE('edit'),
  kCTRLComponentIconSubType     = FOUR_CHAR_CODE('icon'),
  kCTRLComponentPopupMenuSubType = FOUR_CHAR_CODE('popu'),
  kCTRLComponentUserItemSubType = FOUR_CHAR_CODE('user'),
  kCTRLComponentCDEFSubType     = FOUR_CHAR_CODE('cdef'),
  kCTRLComponentPICTSubType     = FOUR_CHAR_CODE('pict'),
  kCTRLComponentGroupBoxSubType = FOUR_CHAR_CODE('gbox'),
  kCTRLComponentLittleArrowsSubType = FOUR_CHAR_CODE('larw'),
  kCTRLComponentBevelButtonSubType = FOUR_CHAR_CODE('bvbn'),
  kCTRLComponentImageWellSubType = FOUR_CHAR_CODE('imwl'),
  kCTRLComponentProgressBarSubType = FOUR_CHAR_CODE('prog')
};

#if CALL_NOT_IN_CARBON
/*
 *  CTRLGetCookie()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
CTRLGetCookie(
  ComponentInstance   ctrl,
  void *              cookie)                                 FIVEWORDINLINE(0x2F3C, 0x0004, 0x0001, 0x7000, 0xA82A);


/*
 *  CTRLSetCookie()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
CTRLSetCookie(
  ComponentInstance   ctrl,
  long                cookie)                                 FIVEWORDINLINE(0x2F3C, 0x0004, 0x0002, 0x7000, 0xA82A);


/*
 *  CTRLGetCapabilities()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
CTRLGetCapabilities(
  ComponentInstance   ctrl,
  long *              capabilities)                           FIVEWORDINLINE(0x2F3C, 0x0004, 0x0003, 0x7000, 0xA82A);


/*
 *  CTRLCreate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
CTRLCreate(
  ComponentInstance   ctrl,
  ControlHandle       hControl)                               FIVEWORDINLINE(0x2F3C, 0x0004, 0x0004, 0x7000, 0xA82A);


/*
 *  CTRLSetControlTitle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
CTRLSetControlTitle(
  ComponentInstance   ctrl,
  ControlHandle       hControl)                               FIVEWORDINLINE(0x2F3C, 0x0004, 0x0005, 0x7000, 0xA82A);


/*
 *  CTRLDisposeControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
CTRLDisposeControl(
  ComponentInstance   ctrl,
  ControlHandle       hControl)                               FIVEWORDINLINE(0x2F3C, 0x0004, 0x0006, 0x7000, 0xA82A);


/*
 *  CTRLHideControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
CTRLHideControl(
  ComponentInstance   ctrl,
  ControlHandle       hControl)                               FIVEWORDINLINE(0x2F3C, 0x0004, 0x0007, 0x7000, 0xA82A);


/*
 *  CTRLShowControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
CTRLShowControl(
  ComponentInstance   ctrl,
  ControlHandle       hControl)                               FIVEWORDINLINE(0x2F3C, 0x0004, 0x0008, 0x7000, 0xA82A);


/*
 *  CTRLDraw1Control()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
CTRLDraw1Control(
  ComponentInstance   ctrl,
  ControlHandle       hControl)                               FIVEWORDINLINE(0x2F3C, 0x0004, 0x0009, 0x7000, 0xA82A);


/*
 *  CTRLHiliteControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
CTRLHiliteControl(
  ComponentInstance   ctrl,
  ControlHandle       hControl)                               FIVEWORDINLINE(0x2F3C, 0x0004, 0x000A, 0x7000, 0xA82A);


/*
 *  CTRLMoveControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
CTRLMoveControl(
  ComponentInstance   ctrl,
  ControlHandle       hControl)                               FIVEWORDINLINE(0x2F3C, 0x0004, 0x000B, 0x7000, 0xA82A);


/*
 *  CTRLSizeControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
CTRLSizeControl(
  ComponentInstance   ctrl,
  ControlHandle       hControl)                               FIVEWORDINLINE(0x2F3C, 0x0004, 0x000C, 0x7000, 0xA82A);


/*
 *  CTRLSetControlValue()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
CTRLSetControlValue(
  ComponentInstance   ctrl,
  ControlHandle       hControl)                               FIVEWORDINLINE(0x2F3C, 0x0004, 0x000D, 0x7000, 0xA82A);


/*
 *  CTRLSetControlMinimum()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
CTRLSetControlMinimum(
  ComponentInstance   ctrl,
  ControlHandle       hControl)                               FIVEWORDINLINE(0x2F3C, 0x0004, 0x000E, 0x7000, 0xA82A);


/*
 *  CTRLSetControlMaximum()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
CTRLSetControlMaximum(
  ComponentInstance   ctrl,
  ControlHandle       hControl)                               FIVEWORDINLINE(0x2F3C, 0x0004, 0x000F, 0x7000, 0xA82A);


/*
 *  CTRLHaveFocus()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
CTRLHaveFocus(
  ComponentInstance   ctrl,
  short *             haveFocus)                              FIVEWORDINLINE(0x2F3C, 0x0004, 0x0010, 0x7000, 0xA82A);


/*
 *  CTRLSetPreFilterProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
CTRLSetPreFilterProc(
  ComponentInstance   ctrl,
  long                preFilterProc)                          FIVEWORDINLINE(0x2F3C, 0x0004, 0x0011, 0x7000, 0xA82A);


/*
 *  CTRLGetPreFilterProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
CTRLGetPreFilterProc(
  ComponentInstance   ctrl,
  void *              preFilterProc)                          FIVEWORDINLINE(0x2F3C, 0x0004, 0x0012, 0x7000, 0xA82A);




/*
 *  CTRLSetText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
CTRLSetText(
  ComponentInstance   ctrl,
  StringPtr           str)                                    FIVEWORDINLINE(0x2F3C, 0x0004, 0x0013, 0x7000, 0xA82A);


/*
 *  CTRLGetText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
CTRLGetText(
  ComponentInstance   ctrl,
  StringPtr           str)                                    FIVEWORDINLINE(0x2F3C, 0x0004, 0x0014, 0x7000, 0xA82A);


/*
 *  CTRLSelectText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
CTRLSelectText(
  ComponentInstance   ctrl,
  short               startSel,
  short               endSel)                                 FIVEWORDINLINE(0x2F3C, 0x0004, 0x0015, 0x7000, 0xA82A);


/*
 *  CTRLSetDefaultItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
CTRLSetDefaultItem(ComponentInstance ctrl)                    FIVEWORDINLINE(0x2F3C, 0x0000, 0x0016, 0x7000, 0xA82A);


/*
 *  CTRLSetDrawProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
CTRLSetDrawProc(
  ComponentInstance   ctrl,
  short               theItem,
  ProcPtr             drawProc)                               FIVEWORDINLINE(0x2F3C, 0x0006, 0x0017, 0x7000, 0xA82A);


/*
 *  CTRLTrackControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
CTRLTrackControl(
  ComponentInstance   ctrl,
  ControlHandle       hControl,
  Point               localPt,
  ControlActionUPP    actionProc)                             FIVEWORDINLINE(0x2F3C, 0x000C, 0x0018, 0x7000, 0xA82A);


/*
 *  CTRLSetFocus()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
CTRLSetFocus(
  ComponentInstance   ctrl,
  ControlHandle       hControl)                               FIVEWORDINLINE(0x2F3C, 0x0004, 0x0019, 0x7000, 0xA82A);


/*
 *  CTRLTestControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
CTRLTestControl(
  ComponentInstance   ctrl,
  ControlHandle       hControl,
  Point               localPt)                                FIVEWORDINLINE(0x2F3C, 0x0008, 0x001A, 0x7000, 0xA82A);


/*
 *  CTRLSetControlData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
CTRLSetControlData(
  ComponentInstance   ctrl,
  ControlHandle       hControl,
  ControlPartCode     part,
  ResType             tagName,
  Size                size,
  Ptr                 data)                                   FIVEWORDINLINE(0x2F3C, 0x0012, 0x001B, 0x7000, 0xA82A);


/*
 *  CTRLGetControlData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
CTRLGetControlData(
  ComponentInstance   ctrl,
  ControlHandle       hControl,
  ControlPartCode     part,
  ResType             tagName,
  Size                bufferSize,
  Ptr                 buffer,
  Size *              actualSize)                             FIVEWORDINLINE(0x2F3C, 0x0016, 0x001C, 0x7000, 0xA82A);




/* selectors for component calls */
enum {
    kCTRLGetCookieSelect                       = 0x0001,
    kCTRLSetCookieSelect                       = 0x0002,
    kCTRLGetCapabilitiesSelect                 = 0x0003,
    kCTRLCreateSelect                          = 0x0004,
    kCTRLSetControlTitleSelect                 = 0x0005,
    kCTRLDisposeControlSelect                  = 0x0006,
    kCTRLHideControlSelect                     = 0x0007,
    kCTRLShowControlSelect                     = 0x0008,
    kCTRLDraw1ControlSelect                    = 0x0009,
    kCTRLHiliteControlSelect                   = 0x000A,
    kCTRLMoveControlSelect                     = 0x000B,
    kCTRLSizeControlSelect                     = 0x000C,
    kCTRLSetControlValueSelect                 = 0x000D,
    kCTRLSetControlMinimumSelect               = 0x000E,
    kCTRLSetControlMaximumSelect               = 0x000F,
    kCTRLHaveFocusSelect                       = 0x0010,
    kCTRLSetPreFilterProcSelect                = 0x0011,
    kCTRLGetPreFilterProcSelect                = 0x0012,
    kCTRLSetTextSelect                         = 0x0013,
    kCTRLGetTextSelect                         = 0x0014,
    kCTRLSelectTextSelect                      = 0x0015,
    kCTRLSetDefaultItemSelect                  = 0x0016,
    kCTRLSetDrawProcSelect                     = 0x0017,
    kCTRLTrackControlSelect                    = 0x0018,
    kCTRLSetFocusSelect                        = 0x0019,
    kCTRLTestControlSelect                     = 0x001A,
    kCTRLSetControlDataSelect                  = 0x001B,
    kCTRLGetControlDataSelect                  = 0x001C
};

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

#endif /* __CONTROLMANAGERCOMPONENT__ */

