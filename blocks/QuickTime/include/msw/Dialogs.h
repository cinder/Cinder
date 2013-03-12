/*
     File:       Dialogs.h
 
     Contains:   Dialog Manager interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1985-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __DIALOGS__
#define __DIALOGS__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __MIXEDMODE__
#include <MixedMode.h>
#endif

#ifndef __MACERRORS__
#include <MacErrors.h>
#endif

#ifndef __EVENTS__
#include <Events.h>
#endif

#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

#ifndef __TEXTEDIT__
#include <TextEdit.h>
#endif

#ifndef __CONTROLS__
#include <Controls.h>
#endif

#ifndef __CARBONEVENTS__
#include <CarbonEvents.h>
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
                                        /* new, more standard names for dialog item types*/
  kControlDialogItem            = 4,
  kButtonDialogItem             = kControlDialogItem | 0,
  kCheckBoxDialogItem           = kControlDialogItem | 1,
  kRadioButtonDialogItem        = kControlDialogItem | 2,
  kResourceControlDialogItem    = kControlDialogItem | 3,
  kStaticTextDialogItem         = 8,
  kEditTextDialogItem           = 16,
  kIconDialogItem               = 32,
  kPictureDialogItem            = 64,
  kUserDialogItem               = 0,
  kHelpDialogItem               = 1,
  kItemDisableBit               = 128
};

enum {
                                        /* old names for dialog item types*/
  ctrlItem                      = 4,
  btnCtrl                       = 0,
  chkCtrl                       = 1,
  radCtrl                       = 2,
  resCtrl                       = 3,
  statText                      = 8,
  editText                      = 16,
  iconItem                      = 32,
  picItem                       = 64,
  userItem                      = 0,
  itemDisable                   = 128
};

enum {
                                        /* standard dialog item numbers*/
  kStdOkItemIndex               = 1,
  kStdCancelItemIndex           = 2,    /* old names*/
  ok                            = kStdOkItemIndex,
  cancel                        = kStdCancelItemIndex
};

enum {
                                        /* standard icon resource id's    */
  kStopIcon                     = 0,
  kNoteIcon                     = 1,
  kCautionIcon                  = 2,    /* old names*/
  stopIcon                      = kStopIcon,
  noteIcon                      = kNoteIcon,
  cautionIcon                   = kCautionIcon
};




#if OLDROUTINENAMES
/*
   These constants lived briefly on ETO 16.  They suggest
   that there is only one index you can use for the OK 
   item, which is not true.  You can put the ok item 
   anywhere you want in the DITL.
*/
enum {
  kOkItemIndex                  = 1,
  kCancelItemIndex              = 2
};

#endif  /* OLDROUTINENAMES */

/*  Dialog Item List Manipulation Constants */
typedef SInt16                          DITLMethod;
enum {
  overlayDITL                   = 0,
  appendDITLRight               = 1,
  appendDITLBottom              = 2
};

typedef SInt16                          StageList;
/* DialogPtr is obsolete. Use DialogRef instead.*/
typedef DialogPtr                       DialogRef;
#if !OPAQUE_TOOLBOX_STRUCTS
struct DialogRecord {
  WindowRecord        window;                 /* in Carbon use GetDialogWindow or GetDialogPort*/
  Handle              items;                  /* in Carbon use Get/SetDialogItem*/
  TEHandle            textH;                  /* in Carbon use GetDialogTextEditHandle*/
  SInt16              editField;              /* in Carbon use SelectDialogItemText/GetDialogKeyboardFocusItem*/
  SInt16              editOpen;               /* not available in Carbon */
  SInt16              aDefItem;               /* in Carbon use Get/SetDialogDefaultItem*/
};
typedef struct DialogRecord             DialogRecord;
typedef DialogRecord *                  DialogPeek;
#endif  /* !OPAQUE_TOOLBOX_STRUCTS */

struct DialogTemplate {
  Rect                boundsRect;
  SInt16              procID;
  Boolean             visible;
  Boolean             filler1;
  Boolean             goAwayFlag;
  Boolean             filler2;
  SInt32              refCon;
  SInt16              itemsID;
  Str255              title;
};
typedef struct DialogTemplate           DialogTemplate;
typedef DialogTemplate *                DialogTPtr;
typedef DialogTPtr *                    DialogTHndl;
struct AlertTemplate {
  Rect                boundsRect;
  SInt16              itemsID;
  StageList           stages;
};
typedef struct AlertTemplate            AlertTemplate;
typedef AlertTemplate *                 AlertTPtr;
typedef AlertTPtr *                     AlertTHndl;
/* new type abstractions for the dialog manager */
typedef SInt16                          DialogItemIndexZeroBased;
typedef SInt16                          DialogItemIndex;
typedef SInt16                          DialogItemType;
/* dialog manager callbacks */
typedef CALLBACK_API( void , SoundProcPtr )(SInt16 soundNumber);
typedef CALLBACK_API( Boolean , ModalFilterProcPtr )(DialogRef theDialog, EventRecord *theEvent, DialogItemIndex *itemHit);
/* ModalFilterYDProcPtr was previously in StandardFile.h */
typedef CALLBACK_API( Boolean , ModalFilterYDProcPtr )(DialogRef theDialog, EventRecord *theEvent, short *itemHit, void *yourDataPtr);
typedef CALLBACK_API( void , UserItemProcPtr )(DialogRef theDialog, DialogItemIndex itemNo);
typedef STACK_UPP_TYPE(SoundProcPtr)                            SoundUPP;
typedef STACK_UPP_TYPE(ModalFilterProcPtr)                      ModalFilterUPP;
typedef STACK_UPP_TYPE(ModalFilterYDProcPtr)                    ModalFilterYDUPP;
typedef STACK_UPP_TYPE(UserItemProcPtr)                         UserItemUPP;
#if CALL_NOT_IN_CARBON
/*
 *  NewSoundUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( SoundUPP )
NewSoundUPP(SoundProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppSoundProcInfo = 0x00000080 };  /* pascal no_return_value Func(2_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(SoundUPP) NewSoundUPP(SoundProcPtr userRoutine) { return (SoundUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppSoundProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewSoundUPP(userRoutine) (SoundUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppSoundProcInfo, GetCurrentArchitecture())
  #endif
#endif

#endif  /* CALL_NOT_IN_CARBON */

/*
 *  NewModalFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ModalFilterUPP )
NewModalFilterUPP(ModalFilterProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppModalFilterProcInfo = 0x00000FD0 };  /* pascal 1_byte Func(4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ModalFilterUPP) NewModalFilterUPP(ModalFilterProcPtr userRoutine) { return (ModalFilterUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppModalFilterProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewModalFilterUPP(userRoutine) (ModalFilterUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppModalFilterProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewModalFilterYDUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ModalFilterYDUPP )
NewModalFilterYDUPP(ModalFilterYDProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppModalFilterYDProcInfo = 0x00003FD0 };  /* pascal 1_byte Func(4_bytes, 4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ModalFilterYDUPP) NewModalFilterYDUPP(ModalFilterYDProcPtr userRoutine) { return (ModalFilterYDUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppModalFilterYDProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewModalFilterYDUPP(userRoutine) (ModalFilterYDUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppModalFilterYDProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewUserItemUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( UserItemUPP )
NewUserItemUPP(UserItemProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppUserItemProcInfo = 0x000002C0 };  /* pascal no_return_value Func(4_bytes, 2_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(UserItemUPP) NewUserItemUPP(UserItemProcPtr userRoutine) { return (UserItemUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppUserItemProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewUserItemUPP(userRoutine) (UserItemUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppUserItemProcInfo, GetCurrentArchitecture())
  #endif
#endif

#if CALL_NOT_IN_CARBON
/*
 *  DisposeSoundUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
DisposeSoundUPP(SoundUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeSoundUPP(SoundUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeSoundUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

#endif  /* CALL_NOT_IN_CARBON */

/*
 *  DisposeModalFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeModalFilterUPP(ModalFilterUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeModalFilterUPP(ModalFilterUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeModalFilterUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeModalFilterYDUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeModalFilterYDUPP(ModalFilterYDUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeModalFilterYDUPP(ModalFilterYDUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeModalFilterYDUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeUserItemUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeUserItemUPP(UserItemUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeUserItemUPP(UserItemUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeUserItemUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

#if CALL_NOT_IN_CARBON
/*
 *  InvokeSoundUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
InvokeSoundUPP(
  SInt16    soundNumber,
  SoundUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeSoundUPP(SInt16 soundNumber, SoundUPP userUPP) { CALL_ONE_PARAMETER_UPP(userUPP, uppSoundProcInfo, soundNumber); }
  #else
    #define InvokeSoundUPP(soundNumber, userUPP) CALL_ONE_PARAMETER_UPP((userUPP), uppSoundProcInfo, (soundNumber))
  #endif
#endif

#endif  /* CALL_NOT_IN_CARBON */

/*
 *  InvokeModalFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
InvokeModalFilterUPP(
  DialogRef          theDialog,
  EventRecord *      theEvent,
  DialogItemIndex *  itemHit,
  ModalFilterUPP     userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeModalFilterUPP(DialogRef theDialog, EventRecord * theEvent, DialogItemIndex * itemHit, ModalFilterUPP userUPP) { return (Boolean)CALL_THREE_PARAMETER_UPP(userUPP, uppModalFilterProcInfo, theDialog, theEvent, itemHit); }
  #else
    #define InvokeModalFilterUPP(theDialog, theEvent, itemHit, userUPP) (Boolean)CALL_THREE_PARAMETER_UPP((userUPP), uppModalFilterProcInfo, (theDialog), (theEvent), (itemHit))
  #endif
#endif

/*
 *  InvokeModalFilterYDUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
InvokeModalFilterYDUPP(
  DialogRef         theDialog,
  EventRecord *     theEvent,
  short *           itemHit,
  void *            yourDataPtr,
  ModalFilterYDUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeModalFilterYDUPP(DialogRef theDialog, EventRecord * theEvent, short * itemHit, void * yourDataPtr, ModalFilterYDUPP userUPP) { return (Boolean)CALL_FOUR_PARAMETER_UPP(userUPP, uppModalFilterYDProcInfo, theDialog, theEvent, itemHit, yourDataPtr); }
  #else
    #define InvokeModalFilterYDUPP(theDialog, theEvent, itemHit, yourDataPtr, userUPP) (Boolean)CALL_FOUR_PARAMETER_UPP((userUPP), uppModalFilterYDProcInfo, (theDialog), (theEvent), (itemHit), (yourDataPtr))
  #endif
#endif

/*
 *  InvokeUserItemUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeUserItemUPP(
  DialogRef        theDialog,
  DialogItemIndex  itemNo,
  UserItemUPP      userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeUserItemUPP(DialogRef theDialog, DialogItemIndex itemNo, UserItemUPP userUPP) { CALL_TWO_PARAMETER_UPP(userUPP, uppUserItemProcInfo, theDialog, itemNo); }
  #else
    #define InvokeUserItemUPP(theDialog, itemNo, userUPP) CALL_TWO_PARAMETER_UPP((userUPP), uppUserItemProcInfo, (theDialog), (itemNo))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewSoundProc(userRoutine)                           NewSoundUPP(userRoutine)
    #define NewModalFilterProc(userRoutine)                     NewModalFilterUPP(userRoutine)
    #define NewModalFilterYDProc(userRoutine)                   NewModalFilterYDUPP(userRoutine)
    #define NewUserItemProc(userRoutine)                        NewUserItemUPP(userRoutine)
    #define CallSoundProc(userRoutine, soundNumber)             InvokeSoundUPP(soundNumber, userRoutine)
    #define CallModalFilterProc(userRoutine, theDialog, theEvent, itemHit) InvokeModalFilterUPP(theDialog, theEvent, itemHit, userRoutine)
    #define CallModalFilterYDProc(userRoutine, theDialog, theEvent, itemHit, yourDataPtr) InvokeModalFilterYDUPP(theDialog, theEvent, itemHit, yourDataPtr, userRoutine)
    #define CallUserItemProc(userRoutine, theDialog, itemNo)    InvokeUserItemUPP(theDialog, itemNo, userRoutine)
#endif /* CALL_NOT_IN_CARBON */


#if !TARGET_OS_MAC
/* QuickTime 3.0 */
typedef CALLBACK_API_C( void , QTModelessCallbackProcPtr )(EventRecord *theEvent, DialogRef theDialog, DialogItemIndex itemHit);
#if CALL_NOT_IN_CARBON
/*
 *  SetModelessDialogCallbackProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
SetModelessDialogCallbackProc(
  DialogRef                   theDialog,
  QTModelessCallbackProcPtr   callbackProc);


#endif  /* CALL_NOT_IN_CARBON */

typedef QTModelessCallbackProcPtr       QTModelessCallbackUPP;
#if CALL_NOT_IN_CARBON
/*
 *  GetDialogControlNotificationProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
GetDialogControlNotificationProc(void * theProc);


/*
 *  SetDialogMovableModal()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
SetDialogMovableModal(DialogRef theDialog);


/*
 *  GetDialogParent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void * )
GetDialogParent(DialogRef theDialog);


#endif  /* CALL_NOT_IN_CARBON */

#endif  /* !TARGET_OS_MAC */

/*
  ---------------------------------------------------------------------------------------------------------
    o Following types are valid with Appearance 1.0 and later
  ---------------------------------------------------------------------------------------------------------
*/
enum {
                                        /* Alert types to pass into StandardAlert */
  kAlertStopAlert               = 0,
  kAlertNoteAlert               = 1,
  kAlertCautionAlert            = 2,
  kAlertPlainAlert              = 3
};

typedef SInt16                          AlertType;
enum {
  kAlertDefaultOKText           = -1,   /* "OK"*/
  kAlertDefaultCancelText       = -1,   /* "Cancel"*/
  kAlertDefaultOtherText        = -1    /* "Don't Save"*/
};

/* StandardAlert alert button numbers */
enum {
  kAlertStdAlertOKButton        = 1,
  kAlertStdAlertCancelButton    = 2,
  kAlertStdAlertOtherButton     = 3,
  kAlertStdAlertHelpButton      = 4
};

enum {
                                        /* Dialog Flags for use in NewFeaturesDialog or dlgx resource */
  kDialogFlagsUseThemeBackground = (1 << 0),
  kDialogFlagsUseControlHierarchy = (1 << 1),
  kDialogFlagsHandleMovableModal = (1 << 2),
  kDialogFlagsUseThemeControls  = (1 << 3)
};

enum {
                                        /* Alert Flags for use in alrx resource */
  kAlertFlagsUseThemeBackground = (1 << 0),
  kAlertFlagsUseControlHierarchy = (1 << 1),
  kAlertFlagsAlertIsMovable     = (1 << 2),
  kAlertFlagsUseThemeControls   = (1 << 3)
};

/* For dftb resource */
enum {
  kDialogFontNoFontStyle        = 0,
  kDialogFontUseFontMask        = 0x0001,
  kDialogFontUseFaceMask        = 0x0002,
  kDialogFontUseSizeMask        = 0x0004,
  kDialogFontUseForeColorMask   = 0x0008,
  kDialogFontUseBackColorMask   = 0x0010,
  kDialogFontUseModeMask        = 0x0020,
  kDialogFontUseJustMask        = 0x0040,
  kDialogFontUseAllMask         = 0x00FF,
  kDialogFontAddFontSizeMask    = 0x0100,
  kDialogFontUseFontNameMask    = 0x0200,
  kDialogFontAddToMetaFontMask  = 0x0400
};

/* Also for dftb resource. This one is available in Mac OS X or later. */
/* It corresponds directly to kControlUseThemeFontIDMask from Controls.h. */
enum {
  kDialogFontUseThemeFontIDMask = 0x0080
};

struct AlertStdAlertParamRec {
  Boolean             movable;                /* Make alert movable modal */
  Boolean             helpButton;             /* Is there a help button? */
  ModalFilterUPP      filterProc;             /* Event filter */
  ConstStringPtr      defaultText;            /* Text for button in OK position */
  ConstStringPtr      cancelText;             /* Text for button in cancel position */
  ConstStringPtr      otherText;              /* Text for button in left position */
  SInt16              defaultButton;          /* Which button behaves as the default */
  SInt16              cancelButton;           /* Which one behaves as cancel (can be 0) */
  UInt16              position;               /* Position (kWindowDefaultPosition in this case */
                                              /* equals kWindowAlertPositionParentWindowScreen) */
};
typedef struct AlertStdAlertParamRec    AlertStdAlertParamRec;
typedef AlertStdAlertParamRec *         AlertStdAlertParamPtr;
enum {
  kHICommandOther               = FOUR_CHAR_CODE('othr') /* sent by standard sheet dialogs when the "other" button is pressed */
};

enum {
  kStdCFStringAlertVersionOne   = 1     /* current version of AlertStdCFStringAlertParamRec */
};


/*
 *  Summary:
 *    Flags to CreateStandardAlert that are specified in the
 *    AlertStdCFStringAlertParamRec.flags field.
 */
enum {

  /*
   * Applies to StandardSheet only. Do not dispose of the sheet window
   * after closing it; allows the sheet to be re-used again in a later
   * call to ShowSheetWindow.
   */
  kStdAlertDoNotDisposeSheet    = 1 << 0,

  /*
   * Applies to StandardSheet only. Causes the sheet window to be
   * hidden immediately without animation effects when the default
   * button is chosen by the user.
   */
  kStdAlertDoNotAnimateOnDefault = 1 << 1,

  /*
   * Applies to StandardSheet only. Causes the sheet window to be
   * hidden immediately without animation effects when the cancel
   * button is chosen by the user.
   */
  kStdAlertDoNotAnimateOnCancel = 1 << 2,

  /*
   * Applies to StandardSheet only. Causes the sheet window to be
   * hidden immediately without animation effects when the other button
   * is chosen by the user.
   */
  kStdAlertDoNotAnimateOnOther  = 1 << 3
};

struct AlertStdCFStringAlertParamRec {
  UInt32              version;                /* kStdCFStringAlertVersionOne */
  Boolean             movable;                /* Make alert movable modal */
  Boolean             helpButton;             /* Is there a help button? */
  CFStringRef         defaultText;            /* Text for button in OK position */
  CFStringRef         cancelText;             /* Text for button in cancel position */
  CFStringRef         otherText;              /* Text for button in left position */
  SInt16              defaultButton;          /* Which button behaves as the default */
  SInt16              cancelButton;           /* Which one behaves as cancel (can be 0) */
  UInt16              position;               /* Position (kWindowDefaultPosition in this case */
                                              /* equals kWindowAlertPositionParentWindowScreen) */
  OptionBits          flags;                  /* Options for the behavior of the alert or sheet */
};
typedef struct AlertStdCFStringAlertParamRec AlertStdCFStringAlertParamRec;
typedef AlertStdCFStringAlertParamRec * AlertStdCFStringAlertParamPtr;
/* --- end Appearance 1.0 or later stuff*/


/*
    NOTE: Code running under MultiFinder or System 7.0 or newer
    should always pass NULL to InitDialogs.
*/
#if CALL_NOT_IN_CARBON
/*
 *  InitDialogs()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
InitDialogs(void * ignored)                                   ONEWORDINLINE(0xA97B);


/*
 *  ErrorSound()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
ErrorSound(SoundUPP soundProc)                                ONEWORDINLINE(0xA98C);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  NewDialog()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( DialogRef )
NewDialog(
  void *             dStorage,
  const Rect *       boundsRect,
  ConstStr255Param   title,
  Boolean            visible,
  SInt16             procID,
  WindowRef          behind,
  Boolean            goAwayFlag,
  SInt32             refCon,
  Handle             items)                                   ONEWORDINLINE(0xA97D);


/*
 *  GetNewDialog()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( DialogRef )
GetNewDialog(
  SInt16      dialogID,
  void *      dStorage,
  WindowRef   behind)                                         ONEWORDINLINE(0xA97C);


/*
 *  NewColorDialog()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( DialogRef )
NewColorDialog(
  void *             dStorage,
  const Rect *       boundsRect,
  ConstStr255Param   title,
  Boolean            visible,
  SInt16             procID,
  WindowRef          behind,
  Boolean            goAwayFlag,
  SInt32             refCon,
  Handle             items)                                   ONEWORDINLINE(0xAA4B);


#if CALL_NOT_IN_CARBON
/*
 *  CloseDialog()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
CloseDialog(DialogRef theDialog)                              ONEWORDINLINE(0xA982);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  DisposeDialog()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DisposeDialog(DialogRef theDialog)                            ONEWORDINLINE(0xA983);


/*
 *  ModalDialog()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
ModalDialog(
  ModalFilterUPP     modalFilter,
  DialogItemIndex *  itemHit)                                 ONEWORDINLINE(0xA991);


/*
 *  IsDialogEvent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
IsDialogEvent(const EventRecord * theEvent)                   ONEWORDINLINE(0xA97F);


/*
 *  DialogSelect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
DialogSelect(
  const EventRecord *  theEvent,
  DialogRef *          theDialog,
  DialogItemIndex *    itemHit)                               ONEWORDINLINE(0xA980);


/*
 *  DrawDialog()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DrawDialog(DialogRef theDialog)                               ONEWORDINLINE(0xA981);


/*
 *  UpdateDialog()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
UpdateDialog(
  DialogRef   theDialog,
  RgnHandle   updateRgn)                                      ONEWORDINLINE(0xA978);


/*
 *  HideDialogItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
HideDialogItem(
  DialogRef         theDialog,
  DialogItemIndex   itemNo)                                   ONEWORDINLINE(0xA827);


/*
 *  ShowDialogItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
ShowDialogItem(
  DialogRef         theDialog,
  DialogItemIndex   itemNo)                                   ONEWORDINLINE(0xA828);


/*
 *  FindDialogItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( DialogItemIndexZeroBased )
FindDialogItem(
  DialogRef   theDialog,
  Point       thePt)                                          ONEWORDINLINE(0xA984);


/*
 *  DialogCut()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DialogCut(DialogRef theDialog);


/*
 *  DialogPaste()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DialogPaste(DialogRef theDialog);


/*
 *  DialogCopy()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DialogCopy(DialogRef theDialog);


/*
 *  DialogDelete()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DialogDelete(DialogRef theDialog);


/*
 *  Alert()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( DialogItemIndex )
Alert(
  SInt16           alertID,
  ModalFilterUPP   modalFilter)                               ONEWORDINLINE(0xA985);


/*
 *  StopAlert()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( DialogItemIndex )
StopAlert(
  SInt16           alertID,
  ModalFilterUPP   modalFilter)                               ONEWORDINLINE(0xA986);


/*
 *  NoteAlert()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( DialogItemIndex )
NoteAlert(
  SInt16           alertID,
  ModalFilterUPP   modalFilter)                               ONEWORDINLINE(0xA987);


/*
 *  CautionAlert()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( DialogItemIndex )
CautionAlert(
  SInt16           alertID,
  ModalFilterUPP   modalFilter)                               ONEWORDINLINE(0xA988);


/*
 *  GetDialogItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
GetDialogItem(
  DialogRef         theDialog,
  DialogItemIndex   itemNo,
  DialogItemType *  itemType,
  Handle *          item,
  Rect *            box)                                      ONEWORDINLINE(0xA98D);


/*
 *  SetDialogItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetDialogItem(
  DialogRef         theDialog,
  DialogItemIndex   itemNo,
  DialogItemType    itemType,
  Handle            item,
  const Rect *      box)                                      ONEWORDINLINE(0xA98E);


/*
 *  ParamText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
ParamText(
  ConstStr255Param   param0,
  ConstStr255Param   param1,
  ConstStr255Param   param2,
  ConstStr255Param   param3)                                  ONEWORDINLINE(0xA98B);


/*
 *  SelectDialogItemText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SelectDialogItemText(
  DialogRef         theDialog,
  DialogItemIndex   itemNo,
  SInt16            strtSel,
  SInt16            endSel)                                   ONEWORDINLINE(0xA97E);


/*
 *  GetDialogItemText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
GetDialogItemText(
  Handle   item,
  Str255   text)                                              ONEWORDINLINE(0xA990);


/*
 *  SetDialogItemText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetDialogItemText(
  Handle             item,
  ConstStr255Param   text)                                    ONEWORDINLINE(0xA98F);


/*
 *  GetAlertStage()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt16 )
GetAlertStage(void)                                           TWOWORDINLINE(0x3EB8, 0x0A9A);


/*
 *  SetDialogFont()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetDialogFont(SInt16 fontNum)                                 TWOWORDINLINE(0x31DF, 0x0AFA);


/*
 *  ResetAlertStage()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
ResetAlertStage(void)                                         TWOWORDINLINE(0x4278, 0x0A9A);


/* APIs in Carbon*/
/*
 *  GetParamText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
GetParamText(
  StringPtr   param0,
  StringPtr   param1,
  StringPtr   param2,
  StringPtr   param3);



#if CALL_NOT_IN_CARBON
/*
 *  newdialog()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( DialogRef )
newdialog(
  void *        dStorage,
  const Rect *  boundsRect,
  const char *  title,
  Boolean       visible,
  SInt16        procID,
  WindowRef     behind,
  Boolean       goAwayFlag,
  SInt32        refCon,
  Handle        items);


/*
 *  newcolordialog()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( DialogRef )
newcolordialog(
  void *        dStorage,
  const Rect *  boundsRect,
  const char *  title,
  Boolean       visible,
  SInt16        procID,
  WindowRef     behind,
  Boolean       goAwayFlag,
  SInt32        refCon,
  Handle        items);


/*
 *  paramtext()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
paramtext(
  const char *  param0,
  const char *  param1,
  const char *  param2,
  const char *  param3);


/*
 *  getdialogitemtext()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
getdialogitemtext(
  Handle   item,
  char *   text);


/*
 *  setdialogitemtext()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
setdialogitemtext(
  Handle        item,
  const char *  text);


/*
 *  finddialogitem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( DialogItemIndexZeroBased )
finddialogitem(
  DialogRef   theDialog,
  Point *     thePt);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  AppendDITL()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
AppendDITL(
  DialogRef    theDialog,
  Handle       theHandle,
  DITLMethod   method);


/*
 *  CountDITL()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( DialogItemIndex )
CountDITL(DialogRef theDialog);


/*
 *  ShortenDITL()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
ShortenDITL(
  DialogRef         theDialog,
  DialogItemIndex   numberItems);


/*
 *  InsertDialogItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
InsertDialogItem(
  DialogRef         theDialog,
  DialogItemIndex   afterItem,
  DialogItemType    itemType,
  Handle            itemHandle,
  const Rect *      box);


/*
 *  RemoveDialogItems()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
RemoveDialogItems(
  DialogRef         theDialog,
  DialogItemIndex   itemNo,
  DialogItemIndex   amountToRemove,
  Boolean           disposeItemData);


/*
 *  StdFilterProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
StdFilterProc(
  DialogRef          theDialog,
  EventRecord *      event,
  DialogItemIndex *  itemHit);


/*
 *  GetStdFilterProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetStdFilterProc(ModalFilterUPP * theProc)                    THREEWORDINLINE(0x303C, 0x0203, 0xAA68);


/*
 *  SetDialogDefaultItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetDialogDefaultItem(
  DialogRef         theDialog,
  DialogItemIndex   newItem)                                  THREEWORDINLINE(0x303C, 0x0304, 0xAA68);


/*
 *  SetDialogCancelItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetDialogCancelItem(
  DialogRef         theDialog,
  DialogItemIndex   newItem)                                  THREEWORDINLINE(0x303C, 0x0305, 0xAA68);


/*
 *  SetDialogTracksCursor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetDialogTracksCursor(
  DialogRef   theDialog,
  Boolean     tracks)                                         THREEWORDINLINE(0x303C, 0x0306, 0xAA68);




/*
  ---------------------------------------------------------------------------------------------------------
    o Appearance Dialog Routines (available only with Appearance 1.0 and later)
  ---------------------------------------------------------------------------------------------------------
*/

/*
 *  NewFeaturesDialog()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( DialogRef )
NewFeaturesDialog(
  void *             inStorage,
  const Rect *       inBoundsRect,
  ConstStr255Param   inTitle,
  Boolean            inIsVisible,
  SInt16             inProcID,
  WindowRef          inBehind,
  Boolean            inGoAwayFlag,
  SInt32             inRefCon,
  Handle             inItemListHandle,
  UInt32             inFlags)                                 THREEWORDINLINE(0x303C, 0x110C, 0xAA68);


/*
 *  AutoSizeDialog()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AutoSizeDialog(DialogRef inDialog)                            THREEWORDINLINE(0x303C, 0x020D, 0xAA68);


/*
    Regarding StandardAlert and constness:
    Even though the inAlertParam parameter is marked const here, there was
    a chance Dialog Manager would modify it on versions of Mac OS prior to 9.
*/
/*
 *  StandardAlert()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
StandardAlert(
  AlertType                      inAlertType,
  ConstStr255Param               inError,
  ConstStr255Param               inExplanation,
  const AlertStdAlertParamRec *  inAlertParam,        /* can be NULL */
  SInt16 *                       outItemHit)                  THREEWORDINLINE(0x303C, 0x090E, 0xAA68);


/* CFString-based StandardAlert and StandardSheet APIs are only available on Mac OS X and later*/

/*
 *  GetStandardAlertDefaultParams()
 *  
 *  Summary:
 *    Fills out an AlertStdCFStringAlertParamRec with default values: -
 *      not movable -   no help button -   default button with title
 *    kAlertDefaultOKText, meaning "OK" -   no cancel or other buttons
 *  
 *  Parameters:
 *    
 *    param:
 *      The parameter block to initialize.
 *    
 *    version:
 *      The parameter block version; pass kStdCFStringAlertVersionOne.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetStandardAlertDefaultParams(
  AlertStdCFStringAlertParamPtr   param,
  UInt32                          version);


/*
 *  CreateStandardAlert()
 *  
 *  Summary:
 *    Creates an alert containing standard elements and using standard
 *    formatting rules.
 *  
 *  Discussion:
 *    CreateStandardAlert should be used in conjunction with
 *    RunStandardAlert. After CreateStandardAlert returns, the alert is
 *    still invisible. RunStandardAlert will show the alert and run a
 *    modal dialog loop to process events in the alert. The strings
 *    passed to this API in the error, explanation, and
 *    AlertStdCFStringAlertParamRec button title parameters will all be
 *    retained during the creation of the alert, and released when the
 *    alert is disposed by RunStandardAlert. There is no net change to
 *    the refcount of these strings across CreateStandardAlert and
 *    RunStandardAlert.
 *  
 *  Parameters:
 *    
 *    alertType:
 *      The type of alert to create.
 *    
 *    error:
 *      The error string to display. CreateStandardAlert increments the
 *      refcount on this string, so you may release it after
 *      CreateStandardAlert returns if you don't need it later.
 *    
 *    explanation:
 *      The explanation string to display. May be NULL or empty to
 *      display no explanation. CreateStandardAlert increments the
 *      refcount on this string, so you may release it after
 *      CreateStandardAlert returns if you don't need it later.
 *    
 *    param:
 *      The parameter block describing how to create the alert. May be
 *      NULL. CreateStandardAlert increments the refcount on the button
 *      title strings in the parameter block, so you may release them
 *      after CreateStandardAlert returns if you don't need them later.
 *    
 *    outAlert:
 *      On exit, contains the new alert.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateStandardAlert(
  AlertType                              alertType,
  CFStringRef                            error,
  CFStringRef                            explanation,       /* can be NULL */
  const AlertStdCFStringAlertParamRec *  param,             /* can be NULL */
  DialogRef *                            outAlert);


/*
 *  RunStandardAlert()
 *  
 *  Summary:
 *    Shows, runs, and destroys a standard alert using a modal dialog
 *    loop.
 *  
 *  Discussion:
 *    RunStandardAlert displays and runs an alert created by
 *    CreateStandardAlert. It handles all user interaction with the
 *    alert. After the user has dismissed the alert, RunStandardAlert
 *    destroys the alert dialog; the DialogRef will be invalid after
 *    RunStandardAlert returns.
 *    
 *    NOTE: DO NOT call this function for a dialog that was not created
 *    with CreateStandardAlert! You will sorely regret it, I promise
 *    you.
 *  
 *  Parameters:
 *    
 *    inAlert:
 *      The alert to display.
 *    
 *    filterProc:
 *      An event filter function for handling events that do not apply
 *      to the alert. May be NULL.
 *    
 *    outItemHit:
 *      On exit, contains the item index of the button that was pressed
 *      to close the alert.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
RunStandardAlert(
  DialogRef          inAlert,
  ModalFilterUPP     filterProc,       /* can be NULL */
  DialogItemIndex *  outItemHit);


/*
 *  CreateStandardSheet()
 *  
 *  Summary:
 *    Creates an alert containing standard elements and using standard
 *    formatting rules, and prepares it to be displayed as a sheet.
 *  
 *  Discussion:
 *    CreateStandardSheet should be used in conjunction with
 *    ShowSheetWindow. After CreateStandardSheet returns, the alert is
 *    still invisible. ShowSheetWindow will show the alert and then
 *    return. Events in the sheet are handled asynchronously; the
 *    application should be prepared for the sheet window to be part of
 *    its windowlist while running its own event loop. When a button in
 *    the sheet is pressed, the EventTargetRef passed to
 *    CreateStandardSheet will receive a command event with one of the
 *    command IDs kHICommandOK, kHICommandCancel, or kHICommandOther.
 *    The sheet is closed before the command is sent. The strings
 *    passed to this API in the error, explanation, and
 *    AlertStdCFStringAlertParamRec button title parameters will all be
 *    retained during the creation of the sheet, and released when the
 *    sheet is disposed. There is no net change to the refcount of
 *    these strings across CreateStandardSheet and sheet destruction.
 *  
 *  Parameters:
 *    
 *    alertType:
 *      The type of alert to create.
 *    
 *    error:
 *      The error string to display. CreateStandardSheet increments the
 *      refcount on this string, so you may release it after
 *      CreateStandardSheet returns if you don't need it later.
 *    
 *    explanation:
 *      The explanation string to display. May be NULL or empty to
 *      display no explanation. CreateStandardSheet increments the
 *      refcount on this string, so you may release it after
 *      CreateStandardSheet returns if you don't need it later.
 *    
 *    param:
 *      The parameter block describing how to create the alert. May be
 *      NULL. CreateStandardSheet increments the refcount on the button
 *      title strings in the parameter block, so you may release them
 *      after CreateStandardSheet returns if you don't need them later.
 *    
 *    notifyTarget:
 *      The event target to be notified when the sheet is closed. The
 *      caller should install an event handler on this target for the
 *      [kEventClassCommand, kEventProcessCommand] event.
 *    
 *    outSheet:
 *      On exit, contains the new alert.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateStandardSheet(
  AlertType                              alertType,
  CFStringRef                            error,
  CFStringRef                            explanation,        /* can be NULL */
  const AlertStdCFStringAlertParamRec *  param,              /* can be NULL */
  EventTargetRef                         notifyTarget,
  DialogRef *                            outSheet);


/*
 *  CloseStandardSheet()
 *  
 *  Summary:
 *    Closes a standard sheet dialog and releases the dialog data
 *    structures.
 *  
 *  Discussion:
 *    CloseStandardSheet is meant to be used when you need to remove a
 *    sheet because of a higher-priority request to close the sheet's
 *    document window. For example, you might have a Save Changes sheet
 *    open on a document window. Meanwhile, the user drags the document
 *    into the trash. When your application sees that the document has
 *    been moved to the trash, it knows that it should close the
 *    document window, but first it needs to close the sheet.
 *    CloseStandardSheet should not be used by your Carbon event
 *    handler in response to a click in one of the sheet buttons; the
 *    Dialog Manager will close the sheet automatically in that case.
 *    If kStdAlertDoNotDisposeSheet was specified when the sheet was
 *    created, the sheet dialog will be hidden but not released, and
 *    you can reuse the sheet later.
 *  
 *  Parameters:
 *    
 *    inSheet:
 *      The sheet to close.
 *    
 *    inResultCommand:
 *      This command, if not zero, will be sent to the EventTarget
 *      specified when the sheet was created.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CloseStandardSheet(
  DialogRef   inSheet,
  UInt32      inResultCommand);


/*
 *  GetDialogItemAsControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetDialogItemAsControl(
  DialogRef     inDialog,
  SInt16        inItemNo,
  ControlRef *  outControl)                                   THREEWORDINLINE(0x303C, 0x050F, 0xAA68);


/*
 *  MoveDialogItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
MoveDialogItem(
  DialogRef   inDialog,
  SInt16      inItemNo,
  SInt16      inHoriz,
  SInt16      inVert)                                         THREEWORDINLINE(0x303C, 0x0510, 0xAA68);


/*
 *  SizeDialogItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SizeDialogItem(
  DialogRef   inDialog,
  SInt16      inItemNo,
  SInt16      inWidth,
  SInt16      inHeight)                                       THREEWORDINLINE(0x303C, 0x0511, 0xAA68);


/*
 *  AppendDialogItemList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DialogsLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AppendDialogItemList(
  DialogRef    dialog,
  SInt16       ditlID,
  DITLMethod   method)                                        THREEWORDINLINE(0x303C, 0x0412, 0xAA68);


/*
  ---------------------------------------------------------------------------------------------------------
    o Dialog Routines available only with Appearance 1.1 (Mac OS 8.5) and later
  ---------------------------------------------------------------------------------------------------------
*/

/*
 *  SetDialogTimeout()
 *  
 *  Summary:
 *    Sets the timeout for a modal dialog.
 *  
 *  Discussion:
 *    SetDialogTimeout sets the delay after which a dialog will be
 *    automatically dismissed. When SetDialogTimeout is called, the
 *    Dialog Manager takes the current time, adds the timeout to it,
 *    and stores the result as the time to dismiss the dialog. If the
 *    dismissal time is reached, the dialog is automatically closed and
 *    the specified dialog item index is returned from ModalDialog in
 *    the itemHit parameter. If the user moves the mouse or presses a
 *    key, the dismissal time is reset by adding the original timeout
 *    to the time of the event. Only the ModalDialog API observes the
 *    timeout value; if you are handling events in a modeless dialog or
 *    sheet using IsDialogEvent and DialogSelect, the timeout will be
 *    ignored.
 *  
 *  Parameters:
 *    
 *    inDialog:
 *      The dialog on which to set a timeout.
 *    
 *    inButtonToPress:
 *      The dialog item index that should be returned from ModalDialog
 *      when the timeout expires.
 *    
 *    inSecondsToWait:
 *      The time to wait before dismissing the dialog, in seconds.
 *  
 *  Result:
 *    An operating system result code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DialogsLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDialogTimeout(
  DialogRef   inDialog,
  SInt16      inButtonToPress,
  UInt32      inSecondsToWait);


/*
 *  GetDialogTimeout()
 *  
 *  Summary:
 *    Retrieves the timeout for a modal dialog.
 *  
 *  Parameters:
 *    
 *    inDialog:
 *      The dialog from which to retrieve the timeout.
 *    
 *    outButtonToPress:
 *      On exit, contains the dialog item index that should be returned
 *      from ModalDialog when the dialog is dismissed. May be NULL if
 *      you do not need this information.
 *    
 *    outSecondsToWait:
 *      On exit, contains the time to wait before dismissing the
 *      dialog, in seconds. May be NULL if you do not need this
 *      information.
 *    
 *    outSecondsRemaining:
 *      On exit, contains the time until the dialog is dismissed, in
 *      seconds. May be NULL if you do not need this information.
 *  
 *  Result:
 *    An operating system result code. Returns dialogNoTimeoutErr if no
 *    timeout has been set for this dialog.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DialogsLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDialogTimeout(
  DialogRef   inDialog,
  SInt16 *    outButtonToPress,          /* can be NULL */
  UInt32 *    outSecondsToWait,          /* can be NULL */
  UInt32 *    outSecondsRemaining);      /* can be NULL */


/*
 *  SetModalDialogEventMask()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DialogsLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetModalDialogEventMask(
  DialogRef   inDialog,
  EventMask   inMask);


/*
 *  GetModalDialogEventMask()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DialogsLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetModalDialogEventMask(
  DialogRef    inDialog,
  EventMask *  outMask);




/*
  ---------------------------------------------------------------------------------------------------------
    o Accessor functions
  ---------------------------------------------------------------------------------------------------------
*/


/*
 *  GetDialogWindow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( WindowRef )
GetDialogWindow(DialogRef dialog);
#if !OPAQUE_TOOLBOX_STRUCTS && !ACCESSOR_CALLS_ARE_FUNCTIONS
  #ifdef __cplusplus
    inline DEFINE_API(WindowRef ) GetDialogWindow(DialogRef dialog) { return (WindowRef)dialog; }
  #else
    #define GetDialogWindow(dialog) ((WindowRef)dialog)
  #endif
#endif



/*
 *  GetDialogTextEditHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( TEHandle )
GetDialogTextEditHandle(DialogRef dialog);
#if !OPAQUE_TOOLBOX_STRUCTS && !ACCESSOR_CALLS_ARE_FUNCTIONS
  #ifdef __cplusplus
    inline DEFINE_API(TEHandle ) GetDialogTextEditHandle(DialogRef dialog) { return ((DialogPeek)dialog)->textH; }
  #else
    #define GetDialogTextEditHandle(dialog) (((DialogPeek)dialog)->textH)
  #endif
#endif



/*
 *  GetDialogDefaultItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt16 )
GetDialogDefaultItem(DialogRef dialog);
#if !OPAQUE_TOOLBOX_STRUCTS && !ACCESSOR_CALLS_ARE_FUNCTIONS
  #ifdef __cplusplus
    inline DEFINE_API(SInt16 ) GetDialogDefaultItem(DialogRef dialog) { return ((DialogPeek)dialog)->aDefItem; }
  #else
    #define GetDialogDefaultItem(dialog) (((DialogPeek)dialog)->aDefItem)
  #endif
#endif



/*
 *  GetDialogCancelItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt16 )
GetDialogCancelItem(DialogRef dialog);
#if !OPAQUE_TOOLBOX_STRUCTS && !ACCESSOR_CALLS_ARE_FUNCTIONS
  #ifdef __cplusplus
    inline DEFINE_API(SInt16 ) GetDialogCancelItem(DialogRef dialog) { return ((DialogPeek)dialog)->editOpen; }
  #else
    #define GetDialogCancelItem(dialog) (((DialogPeek)dialog)->editOpen)
  #endif
#endif



/*
 *  GetDialogKeyboardFocusItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt16 )
GetDialogKeyboardFocusItem(DialogRef dialog);
#if !OPAQUE_TOOLBOX_STRUCTS && !ACCESSOR_CALLS_ARE_FUNCTIONS
  #ifdef __cplusplus
    inline DEFINE_API(SInt16 ) GetDialogKeyboardFocusItem(DialogRef dialog) { return ((DialogPeek)dialog)->editField < 0 ? (short)(-1): (short)(((DialogPeek)dialog)->editField + 1); }
  #else
    #define GetDialogKeyboardFocusItem(dialog) (((DialogPeek)dialog)->editField < 0 ? (short)(-1): (short)(((DialogPeek)dialog)->editField + 1))
  #endif
#endif



/*
 *  SetPortDialogPort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetPortDialogPort(DialogRef dialog);
#if !OPAQUE_TOOLBOX_STRUCTS && !ACCESSOR_CALLS_ARE_FUNCTIONS
  #ifdef __cplusplus
    inline DEFINE_API(void) SetPortDialogPort(DialogRef dialog) { MacSetPort((GrafPtr)dialog); }
  #else
    #define SetPortDialogPort(dialog) (MacSetPort((GrafPtr)dialog))
  #endif
#endif



/*
 *  GetDialogPort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( CGrafPtr )
GetDialogPort(DialogRef dialog);
#if !OPAQUE_TOOLBOX_STRUCTS && !ACCESSOR_CALLS_ARE_FUNCTIONS
  #ifdef __cplusplus
    inline DEFINE_API(CGrafPtr ) GetDialogPort(DialogRef dialog) { return (CGrafPtr)dialog; }
  #else
    #define GetDialogPort(dialog) ((CGrafPtr)dialog)
  #endif
#endif



/*
 *  GetDialogFromWindow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( DialogRef )
GetDialogFromWindow(WindowRef window);
#if !OPAQUE_TOOLBOX_STRUCTS && !ACCESSOR_CALLS_ARE_FUNCTIONS
  #ifdef __cplusplus
    inline DEFINE_API(DialogRef ) GetDialogFromWindow(WindowRef window) { return (DialogRef)window; }
  #else
    #define GetDialogFromWindow(window) ((DialogRef)window)
  #endif
#endif




#if CALL_NOT_IN_CARBON
/*
 *  CouldDialog()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
CouldDialog(SInt16 dialogID)                                  ONEWORDINLINE(0xA979);


/*
 *  FreeDialog()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
FreeDialog(SInt16 dialogID)                                   ONEWORDINLINE(0xA97A);


/*
 *  CouldAlert()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
CouldAlert(SInt16 alertID)                                    ONEWORDINLINE(0xA989);


/*
 *  FreeAlert()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
FreeAlert(SInt16 alertID)                                     ONEWORDINLINE(0xA98A);




#endif  /* CALL_NOT_IN_CARBON */

#if OLDROUTINENAMES
#define DisposDialog(theDialog) DisposeDialog(theDialog)
#define UpdtDialog(theDialog, updateRgn) UpdateDialog(theDialog, updateRgn)
#define GetDItem(theDialog, itemNo, itemType, item, box) GetDialogItem(theDialog, itemNo, itemType, item, box)
#define SetDItem(theDialog, itemNo, itemType, item, box) SetDialogItem(theDialog, itemNo, itemType, item, box)
#define HideDItem(theDialog, itemNo) HideDialogItem(theDialog, itemNo)
#define ShowDItem(theDialog, itemNo) ShowDialogItem(theDialog, itemNo)
#define SelIText(theDialog, itemNo, strtSel, endSel) SelectDialogItemText(theDialog, itemNo, strtSel, endSel)
#define GetIText(item, text) GetDialogItemText(item, text)
#define SetIText(item, text) SetDialogItemText(item, text)
#define FindDItem(theDialog, thePt) FindDialogItem(theDialog, thePt)
#define NewCDialog(dStorage, boundsRect, title, visible, procID, behind, goAwayFlag, refCon, items) \
NewColorDialog(dStorage, boundsRect, title, visible, procID, behind, goAwayFlag, refCon, items)
#define GetAlrtStage() GetAlertStage()
#define ResetAlrtStage() ResetAlertStage()
#define DlgCut(theDialog) DialogCut(theDialog)
#define DlgPaste(theDialog) DialogPaste(theDialog)
#define DlgCopy(theDialog) DialogCopy(theDialog)
#define DlgDelete(theDialog) DialogDelete(theDialog)
#define SetDAFont(fontNum) SetDialogFont(fontNum)
#define SetGrafPortOfDialog(dialog) SetPortDialogPort(dialog)
#if CGLUESUPPORTED
#define newcdialog(dStorage, boundsRect, title, visible, procID, behind, goAwayFlag, refCon, items) \
newcolordialog(dStorage, boundsRect, title, visible, procID, behind, goAwayFlag, refCon, items)
#define getitext(item, text) getdialogitemtext(item, text)
#define setitext(item, text) setdialogitemtext(item, text)
#define findditem(theDialog, thePt) finddialogitem(theDialog, thePt)
#endif
#endif  /* OLDROUTINENAMES */






#if TARGET_OS_MAC
#endif  /* TARGET_OS_MAC */

#if !TARGET_OS_MAC
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

#endif /* __DIALOGS__ */

