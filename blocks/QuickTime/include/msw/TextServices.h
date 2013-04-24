/*
     File:       TextServices.h
 
     Contains:   Text Services Manager Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1991-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __TEXTSERVICES__
#define __TEXTSERVICES__

#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __COMPONENTS__
#include <Components.h>
#endif

#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif

#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif

#ifndef __EVENTS__
#include <Events.h>
#endif

#ifndef __MENUS__
#include <Menus.h>
#endif

#ifndef __AEINTERACTION__
#include <AEInteraction.h>
#endif

#ifndef __ATSUNICODE__
#include <ATSUnicode.h>
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
  kTextService                  = FOUR_CHAR_CODE('tsvc'), /* component type for the component description */
  kTSMVersion                   = 0x0150 /* Version of the Text Services Manager is 1.5 */
};


/*
    TextServiceClass constants supported by TSM
    Same as component subtype for the component description
*/
enum {
  kKeyboardInputMethodClass     = FOUR_CHAR_CODE('inpm'),
  kInkInputMethodClass          = FOUR_CHAR_CODE('ink '),
  kCharacterPaletteInputMethodClass = FOUR_CHAR_CODE('cplt')
};

typedef OSType                          TextServiceClass;
enum {
  kTSClassHonorUserSetting      = 1,
  kTSClassForceSetting          = 2,
  kTSClassForceToHonorUserSetting = 3
};

typedef UInt32                          TSClassEnablingForceLevel;
enum {
  kUnicodeDocument              = FOUR_CHAR_CODE('udoc'), /* TSM Document type for Unicode-savvy application */
  kUnicodeTextService           = FOUR_CHAR_CODE('utsv') /* Component type for Unicode Text Service */
};

/* TSMDocumentID property tags*/
enum {
  kTSMDocumentPropertySupportGlyphInfo = FOUR_CHAR_CODE('dpgi') /*  property value is arbitrary*/
};


/* Language and Script constants*/
enum {
  kUnknownLanguage              = 0xFFFF,
  kUnknownScript                = 0xFFFF,
  kNeutralScript                = 0xFFFF
};


enum {
                                        /* Component Flags in ComponentDescription */
  bTakeActiveEvent              = 15,   /* bit set if the component takes active event */
  bHandleAERecording            = 16,   /* bit set if the component takes care of recording Apple Events <new in vers2.0> */
  bScriptMask                   = 0x00007F00, /* bit 8 - 14 */
  bLanguageMask                 = 0x000000FF, /* bit 0 - 7  */
  bScriptLanguageMask           = bScriptMask + bLanguageMask /* bit 0 - 14  */
};

enum {
                                        /* Typing method property constants for Input Methods */
  kIMJaTypingMethodProperty     = FOUR_CHAR_CODE('jtyp'), /* Typing method property for Japanese input methods*/
  kIMJaTypingMethodRoman        = FOUR_CHAR_CODE('roma'), /* Roman typing*/
  kIMJaTypingMethodKana         = FOUR_CHAR_CODE('kana') /* Kana typing*/
};

enum {
                                        /* Low level routines which are dispatched directly to the Component Manager */
  kCMGetScriptLangSupport       = 0x0001, /* Component Manager call selector 1 */
  kCMInitiateTextService        = 0x0002, /* Component Manager call selector 2 */
  kCMTerminateTextService       = 0x0003, /* Component Manager call selector 3 */
  kCMActivateTextService        = 0x0004, /* Component Manager call selector 4 */
  kCMDeactivateTextService      = 0x0005, /* Component Manager call selector 5 */
  kCMTextServiceEvent           = 0x0006, /* Component Manager call selector 6 */
  kCMGetTextServiceMenu         = 0x0007, /* Component Manager call selector 7 */
  kCMTextServiceMenuSelect      = 0x0008, /* Component Manager call selector 8 */
  kCMFixTextService             = 0x0009, /* Component Manager call selector 9 */
  kCMSetTextServiceCursor       = 0x000A, /* Component Manager call selector 10 */
  kCMHidePaletteWindows         = 0x000B, /* Component Manager call selector 11 */
  kCMGetTextServiceProperty     = 0x000C, /* Component Manager call selector 12 */
  kCMSetTextServiceProperty     = 0x000D /* Component Manager call selector 13 */
};

enum {
                                        /* New low level routines which are dispatched directly to the Component Manager */
  kCMUCTextServiceEvent         = 0x000E /* Component Manager call selector 14 */
};


/* extract Script/Language code from Component flag ... */
#define     mGetScriptCode(cdRec)       ((ScriptCode)   ((cdRec.componentFlags & bScriptMask) >> 8))
#define     mGetLanguageCode(cdRec)     ((LangCode)     cdRec.componentFlags & bLanguageMask)

/* New opaque definitions for types */
typedef struct OpaqueTSMDocumentID*     TSMDocumentID;
typedef OSType                          InterfaceTypeList[1];

/* Text Service Info List */
struct TextServiceInfo {
  Component           fComponent;
  Str255              fItemName;
};
typedef struct TextServiceInfo          TextServiceInfo;
typedef TextServiceInfo *               TextServiceInfoPtr;
struct TextServiceList {
  short               fTextServiceCount;      /* number of entries in the 'fServices' array */
  TextServiceInfo     fServices[1];           /* Note: array of 'TextServiceInfo' records follows */
};
typedef struct TextServiceList          TextServiceList;
typedef TextServiceList *               TextServiceListPtr;
typedef TextServiceListPtr *            TextServiceListHandle;
struct ScriptLanguageRecord {
  ScriptCode          fScript;
  LangCode            fLanguage;
};
typedef struct ScriptLanguageRecord     ScriptLanguageRecord;
struct ScriptLanguageSupport {
  short               fScriptLanguageCount;   /* number of entries in the 'fScriptLanguageArray' array */
  ScriptLanguageRecord  fScriptLanguageArray[1]; /* Note: array of 'ScriptLanguageRecord' records follows */
};
typedef struct ScriptLanguageSupport    ScriptLanguageSupport;
typedef ScriptLanguageSupport *         ScriptLanguageSupportPtr;
typedef ScriptLanguageSupportPtr *      ScriptLanguageSupportHandle;
struct TSMGlyphInfo {
  CFRange             range;                  /*    two SInt32s*/
  ATSFontRef          fontRef;
  UInt16              collection;             /*    kGlyphCollectionXXX enum*/
  UInt16              glyphID;                /*    GID (when collection==0) or CID*/
};
typedef struct TSMGlyphInfo             TSMGlyphInfo;
struct TSMGlyphInfoArray {
  ItemCount           numGlyphInfo;           /*    UInt32*/
  TSMGlyphInfo        glyphInfo[1];
};
typedef struct TSMGlyphInfoArray        TSMGlyphInfoArray;

/* High level TSM Doucment routines */
/*
 *  NewTSMDocument()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
NewTSMDocument(
  short               numOfInterface,
  InterfaceTypeList   supportedInterfaceTypes,
  TSMDocumentID *     idocID,
  long                refcon)                                 TWOWORDINLINE(0x7000, 0xAA54);


/*
 *  DeleteTSMDocument()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DeleteTSMDocument(TSMDocumentID idocID)                       TWOWORDINLINE(0x7001, 0xAA54);


/*
 *  ActivateTSMDocument()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ActivateTSMDocument(TSMDocumentID idocID)                     TWOWORDINLINE(0x7002, 0xAA54);


/*
 *  DeactivateTSMDocument()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DeactivateTSMDocument(TSMDocumentID idocID)                   TWOWORDINLINE(0x7003, 0xAA54);


/*
 *  FixTSMDocument()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
FixTSMDocument(TSMDocumentID idocID)                          TWOWORDINLINE(0x7007, 0xAA54);


/*
 *  GetServiceList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetServiceList(
  short                    numOfInterface,
  OSType *                 supportedInterfaceTypes,
  TextServiceListHandle *  serviceInfo,
  long *                   seedValue)                         TWOWORDINLINE(0x7008, 0xAA54);


/*
 *  OpenTextService()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
OpenTextService(
  TSMDocumentID        idocID,
  Component            aComponent,
  ComponentInstance *  aComponentInstance)                    TWOWORDINLINE(0x7009, 0xAA54);


/*
 *  CloseTextService()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
CloseTextService(
  TSMDocumentID       idocID,
  ComponentInstance   aComponentInstance)                     TWOWORDINLINE(0x700A, 0xAA54);


/*
 *  SendAEFromTSMComponent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SendAEFromTSMComponent(
  const AppleEvent *  theAppleEvent,
  AppleEvent *        reply,
  AESendMode          sendMode,
  AESendPriority      sendPriority,
  long                timeOutInTicks,
  AEIdleUPP           idleProc,
  AEFilterUPP         filterProc)                             TWOWORDINLINE(0x700B, 0xAA54);


/*
 *  SendTextInputEvent()
 *  
 *  Discussion:
 *    This API replaces SendAEFromTSMComponent on Mac OS X only. Input
 *    Methods on Mac OS X are Carbon Event based instead of AppleEvent
 *    based.  The Carbon TextInput events which they generate are
 *    provided to TSM for dispatching via this API.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib N.e.v.e.r and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SendTextInputEvent(EventRef inEvent);


/*
 *  SetDefaultInputMethod()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetDefaultInputMethod(
  Component               ts,
  ScriptLanguageRecord *  slRecordPtr)                        TWOWORDINLINE(0x700C, 0xAA54);


/*
 *  GetDefaultInputMethod()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetDefaultInputMethod(
  Component *             ts,
  ScriptLanguageRecord *  slRecordPtr)                        TWOWORDINLINE(0x700D, 0xAA54);


/*
 *  SetTextServiceLanguage()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetTextServiceLanguage(ScriptLanguageRecord * slRecordPtr)    TWOWORDINLINE(0x700E, 0xAA54);


/*
 *  GetTextServiceLanguage()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetTextServiceLanguage(ScriptLanguageRecord * slRecordPtr)    TWOWORDINLINE(0x700F, 0xAA54);


/*
 *  UseInputWindow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
UseInputWindow(
  TSMDocumentID   idocID,
  Boolean         useWindow)                                  TWOWORDINLINE(0x7010, 0xAA54);


/*
 *  TSMSetInlineInputRegion()
 *  
 *  Discussion:
 *    Tell TSM about the region occupied by an inline input session. If
 *    the location of certain mouse events (clicks, mouse moved) occur
 *    within the specified inline input region, TSM will forward these
 *    events to the current Input Method so that it can interact with
 *    the user. Note: If you do not specify this information, TSM will
 *    need to intercept mouse events in the entire content region as
 *    the default, when an input method is active, in order to ensure
 *    that input methods can manage user interaction properly.
 *  
 *  Parameters:
 *    
 *    inTSMDocument:
 *      The document.
 *    
 *    inWindow:
 *      The window that contains the inline input session. You can pass
 *      NULL for this parameter to indicate the user focus window.
 *    
 *    inRegion:
 *      The region occupied by the current inline input region. This
 *      should be in the coordinates of the port associated with the
 *      window you passed to inPort. It will need to be recomputed when
 *      the text content of the inline input session content changes
 *      (i.e. due to Update Active Input Area events) and when the
 *      region moves for other reasons, such as window resized,
 *      scrolling, etc. If you pass a NULL region for this parameter,
 *      TSM will default to intercept mouse events in the focus
 *      window's content region.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TSMSetInlineInputRegion(
  TSMDocumentID   inTSMDocument,
  WindowRef       inWindow,
  RgnHandle       inRegion);



/* Following calls from Classic event loops not needed for Carbon clients. */
#if CALL_NOT_IN_CARBON
/*
 *  TSMEvent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Boolean )
TSMEvent(EventRecord * event)                                 TWOWORDINLINE(0x7004, 0xAA54);


/*
 *  TSMMenuSelect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Boolean )
TSMMenuSelect(long menuResult)                                TWOWORDINLINE(0x7005, 0xAA54);


/*
 *  SetTSMCursor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Boolean )
SetTSMCursor(Point mousePos)                                  TWOWORDINLINE(0x7006, 0xAA54);


/* Following ServiceWindow API replaced by Window Manager API in Carbon. */
/*
 *  NewServiceWindow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
NewServiceWindow(
  void *              wStorage,
  const Rect *        boundsRect,
  ConstStr255Param    title,
  Boolean             visible,
  short               theProc,
  WindowRef           behind,
  Boolean             goAwayFlag,
  ComponentInstance   ts,
  WindowRef *         window)                                 TWOWORDINLINE(0x7011, 0xAA54);


/*
 *  CloseServiceWindow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
CloseServiceWindow(WindowRef window)                          TWOWORDINLINE(0x7012, 0xAA54);


/*
 *  GetFrontServiceWindow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
GetFrontServiceWindow(WindowRef * window)                     TWOWORDINLINE(0x7013, 0xAA54);


/*
 *  FindServiceWindow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
FindServiceWindow(
  Point        thePoint,
  WindowRef *  theWindow)                                     TWOWORDINLINE(0x7017, 0xAA54);


/*
 *  NewCServiceWindow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
NewCServiceWindow(
  void *              wStorage,
  const Rect *        boundsRect,
  ConstStr255Param    title,
  Boolean             visible,
  short               theProc,
  WindowRef           behind,
  Boolean             goAwayFlag,
  ComponentInstance   ts,
  WindowRef *         window)                                 TWOWORDINLINE(0x701A, 0xAA54);


/* Explicit initialization not needed for Carbon clients, since TSM is */
/* instanciated per-context. */
/*
 *  InitTSMAwareApplication()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
InitTSMAwareApplication(void)                                 TWOWORDINLINE(0x7014, 0xAA54);


/*
 *  CloseTSMAwareApplication()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
CloseTSMAwareApplication(void)                                TWOWORDINLINE(0x7015, 0xAA54);



/* Component Manager Interfaces to Input Methods */
#endif  /* CALL_NOT_IN_CARBON */

/*
 *  GetScriptLanguageSupport()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( ComponentResult )
GetScriptLanguageSupport(
  ComponentInstance              ts,
  ScriptLanguageSupportHandle *  scriptHdl)                   FIVEWORDINLINE(0x2F3C, 0x0004, 0x0001, 0x7000, 0xA82A);


/*
 *  InitiateTextService()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( ComponentResult )
InitiateTextService(ComponentInstance ts)                     FIVEWORDINLINE(0x2F3C, 0x0000, 0x0002, 0x7000, 0xA82A);


/*
 *  TerminateTextService()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( ComponentResult )
TerminateTextService(ComponentInstance ts)                    FIVEWORDINLINE(0x2F3C, 0x0000, 0x0003, 0x7000, 0xA82A);


/*
 *  ActivateTextService()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( ComponentResult )
ActivateTextService(ComponentInstance ts)                     FIVEWORDINLINE(0x2F3C, 0x0000, 0x0004, 0x7000, 0xA82A);


/*
 *  DeactivateTextService()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( ComponentResult )
DeactivateTextService(ComponentInstance ts)                   FIVEWORDINLINE(0x2F3C, 0x0000, 0x0005, 0x7000, 0xA82A);


/*
 *  GetTextServiceMenu()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( ComponentResult )
GetTextServiceMenu(
  ComponentInstance   ts,
  MenuRef *           serviceMenu)                            FIVEWORDINLINE(0x2F3C, 0x0004, 0x0007, 0x7000, 0xA82A);


/* New Text Service call in Carbon. */
/* Note: Only Raw Key and Mouse-flavored events are passed to Text Services on MacOS X. */
/*
 *  TextServiceEventRef()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( ComponentResult )
TextServiceEventRef(
  ComponentInstance   ts,
  EventRef            event)                                  FIVEWORDINLINE(0x2F3C, 0x0006, 0x0006, 0x7000, 0xA82A);


#if CALL_NOT_IN_CARBON
/*
 *  TextServiceEvent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
TextServiceEvent(
  ComponentInstance   ts,
  short               numOfEvents,
  EventRecord *       event)                                  FIVEWORDINLINE(0x2F3C, 0x0006, 0x0006, 0x7000, 0xA82A);


/*
 *  UCTextServiceEvent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
UCTextServiceEvent(
  ComponentInstance   ts,
  short               numOfEvents,
  EventRecord *       event,
  UniChar             unicodeString[],
  UniCharCount        unicodeStrLength)                       FIVEWORDINLINE(0x2F3C, 0x000E, 0x000E, 0x7000, 0xA82A);


/*
 *  TextServiceMenuSelect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
TextServiceMenuSelect(
  ComponentInstance   ts,
  MenuRef             serviceMenu,
  short               item)                                   FIVEWORDINLINE(0x2F3C, 0x0006, 0x0008, 0x7000, 0xA82A);


/*
 *  SetTextServiceCursor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( ComponentResult )
SetTextServiceCursor(
  ComponentInstance   ts,
  Point               mousePos)                               FIVEWORDINLINE(0x2F3C, 0x0004, 0x000A, 0x7000, 0xA82A);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  FixTextService()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( ComponentResult )
FixTextService(ComponentInstance ts)                          FIVEWORDINLINE(0x2F3C, 0x0000, 0x0009, 0x7000, 0xA82A);


/*
 *  HidePaletteWindows()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( ComponentResult )
HidePaletteWindows(ComponentInstance ts)                      FIVEWORDINLINE(0x2F3C, 0x0000, 0x000B, 0x7000, 0xA82A);


/*
 *  GetTextServiceProperty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( ComponentResult )
GetTextServiceProperty(
  ComponentInstance   ts,
  OSType              propertySelector,
  SInt32 *            result)                                 FIVEWORDINLINE(0x2F3C, 0x0008, 0x000C, 0x7000, 0xA82A);


/*
 *  SetTextServiceProperty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( ComponentResult )
SetTextServiceProperty(
  ComponentInstance   ts,
  OSType              propertySelector,
  SInt32              value)                                  FIVEWORDINLINE(0x2F3C, 0x0008, 0x000D, 0x7000, 0xA82A);


/* Get the active TSMDocument in the current application context.       */
/* If TSM has enabled bottom line input mode because no TSMDocument     */
/* is active, NULL will be returned.                                    */
/*
 *  TSMGetActiveDocument()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( TSMDocumentID )
TSMGetActiveDocument(void);


/*
 *  GetDefaultInputMethodOfClass()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API( OSStatus )
GetDefaultInputMethodOfClass(
  Component *             aComp,
  ScriptLanguageRecord *  slRecPtr,
  TextServiceClass        tsClass);


/*
 *  SetDefaultInputMethodOfClass()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API( OSStatus )
SetDefaultInputMethodOfClass(
  Component               aComp,
  ScriptLanguageRecord *  slRecPtr,
  TextServiceClass        tsClass);


/*
 *  DeselectTextService()
 *  
 *  Discussion:
 *    This API is currently only intended for use by Character Palette
 *    class input methods. It allows such an input method to notify TSM
 *    that it has been closed by the user as a result of interaction
 *    with the input method's own UI, such a palette window's close
 *    button, instead of via the normal UI provided by the System, such
 *    as the Keyboard Menu.  Note that this API is only meant for use
 *    with "additive" text service classes (such as Character
 *    Palettes), and not with traditional input methods which are
 *    exclusively selected in a given class and script, such as
 *    keyboard input methods.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API( OSStatus )
DeselectTextService(Component aComp);


/*
 *  SelectTextService()
 *  
 *  Discussion:
 *    This API is currently only intended for use by the system UI
 *    (Keyboard Menu or prefs panel) and input methods that wish to set
 *    to the "selected" state a text service which is "additive" in
 *    nature.  It is not intended for use with input methods that are
 *    "exclusive" in nature (see DeselectTextService).  It allows an
 *    input method to notify TSM that it has been selected by the user
 *    as a result of interaction with some UI (possibly another input
 *    method), instead of via the normal UI provided by the System,
 *    such as the Keyboard Menu.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API( OSStatus )
SelectTextService(Component aComp);


/*
 *  TSMSetDocumentProperty()
 *  
 *  Discussion:
 *    This API is currently only needed for applications that wish to
 *    signal to TSM that it supports GlyphID specification in TSM
 *    events containing Unicode text (see
 *    kTSMDocumentPropertySupportGlyphInfo).  Input Methods call
 *    TSMGetDocumentProperty() against the currently active TSMDocument
 *    to test whether the app supports this glyph info. These TSM
 *    property API can also be freely used by applications to relate
 *    arbitrary data to a given TSMDocument.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API( OSStatus )
TSMSetDocumentProperty(
  TSMDocumentID   docID,
  OSType          propertyTag,
  UInt32          propertySize,
  void *          propertyData);


/*
 *  TSMGetDocumentProperty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API( OSStatus )
TSMGetDocumentProperty(
  TSMDocumentID   docID,
  OSType          propertyTag,
  UInt32          bufferSize,
  UInt32 *        actualSize,
  void *          propertyBuffer);      /* can be NULL */


/*
 *  TSMRemoveDocumentProperty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API( OSStatus )
TSMRemoveDocumentProperty(
  TSMDocumentID   docID,
  OSType          propertyTag);


#if OLDROUTINENAMES
enum {
  kInputMethodService           = kKeyboardInputMethodClass
};

#endif  /* OLDROUTINENAMES */


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

#endif /* __TEXTSERVICES__ */

