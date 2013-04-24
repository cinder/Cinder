/*
     File:       Navigation.h
 
     Contains:   Navigation Services Interfaces
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1996-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __NAVIGATION__
#define __NAVIGATION__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __MACERRORS__
#include <MacErrors.h>
#endif

#ifndef __CODEFRAGMENTS__
#include <CodeFragments.h>
#endif

#ifndef __TRANSLATION__
#include <Translation.h>
#endif

#ifndef __MACWINDOWS__
#include <MacWindows.h>
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

typedef UInt32 NavAskSaveChangesAction;
enum {
                                        /* input action codes for NavAskSaveChanges() */
  kNavSaveChangesClosingDocument = 1,
  kNavSaveChangesQuittingApplication = 2,
  kNavSaveChangesOther          = 0
};


typedef UInt32 NavAskSaveChangesResult;
enum {
                                        /* result codes for NavAskSaveChanges() */
  kNavAskSaveChangesSave        = 1,
  kNavAskSaveChangesCancel      = 2,
  kNavAskSaveChangesDontSave    = 3
};


typedef UInt32 NavAskDiscardChangesResult;
enum {
                                        /* result codes for NavAskDiscardChanges() */
  kNavAskDiscardChanges         = 1,
  kNavAskDiscardChangesCancel   = 2
};


typedef SInt16 NavFilterModes;
enum {
                                        /* which elements are being filtered for objects: */
  kNavFilteringBrowserList      = 0,
  kNavFilteringFavorites        = 1,
  kNavFilteringRecents          = 2,
  kNavFilteringShortCutVolumes  = 3,
  kNavFilteringLocationPopup    = 4     /* for v1.1 or greater */
};


enum {
  kNavFileOrFolderVersion       = 1
};

struct NavFileOrFolderInfo {
  UInt16              version;
  Boolean             isFolder;
  Boolean             visible;
  UInt32              creationDate;
  UInt32              modificationDate;
  union {
    struct {
      Boolean             locked;             /* file is locked */
      Boolean             resourceOpen;       /* resource fork is opened */
      Boolean             dataOpen;           /* data fork is opened */
      Boolean             reserved1;
      UInt32              dataSize;           /* size of the data fork */
      UInt32              resourceSize;       /* size of the resource fork */
      FInfo               finderInfo;         /* more file info: */
      FXInfo              finderXInfo;
    }                       fileInfo;
    struct {
      Boolean             shareable;
      Boolean             sharePoint;
      Boolean             mounted;
      Boolean             readable;
      Boolean             writeable;
      Boolean             reserved2;
      UInt32              numberOfFiles;
      DInfo               finderDInfo;
      DXInfo              finderDXInfo;
      OSType              folderType;         /* package type, For struct version >= 1 */
      OSType              folderCreator;      /* package creator, For struct version >= 1 */
      char                reserved3[206];
    }                       folderInfo;

  }                       fileAndFolder;
};
typedef struct NavFileOrFolderInfo      NavFileOrFolderInfo;
union NavEventDataInfo {
  EventRecord *       event;                  /* for event processing */
  void *              param;                  /* points to event specific data */
};
typedef union NavEventDataInfo          NavEventDataInfo;
struct NavEventData {
  NavEventDataInfo    eventDataParms;         /* the event data */
  SInt16              itemHit;                /* the dialog item number, for v1.1 or greater */
};
typedef struct NavEventData             NavEventData;

/*
 *  NavDialogRef
 *  
 *  Summary:
 *    Opaque Navigation Services dialog identifier
 *  
 *  Discussion:
 *    A NavDialogRef is an opaque reference to an instance of a
 *    Navigation Services dialog. A new NavDialogRef is returned from
 *    any of the NavCreate*Dialog functions and is later disposed with
 *    the NavDialogDispose function. NavDialogRef is the new name for
 *    the NavContext type, and thus when a client's event proc is
 *    called, the value of the NavCBRec.context field is the same as
 *    the NavDialogRef returned from the corresponding
 *    NavCreate*Dialog. A NavDialogRef is distinct from, and is not
 *    interchangable with, a Dialog Manager DialogRef.
 */
typedef struct __NavDialog*             NavDialogRef;
#if CALL_NOT_IN_CARBON
/* NavContext is the old name for NavDialogRef */

typedef NavDialogRef                    NavContext;
#endif  /* CALL_NOT_IN_CARBON */


/*
 *  NavUserAction
 *  
 *  Summary:
 *    Indicates an action taken by the user
 *  
 *  Discussion:
 *    When the user clicks a button at the bottom of a Nav Services
 *    dialog (or makes an equivalent mouse or key gesture), a
 *    kNavCBUserAction event is sent to the client's event proc
 *    indicating which action was taken. Often, the action also
 *    dismisses the dialog. User action events are only generated when
 *    using dialogs created from a NavCreate*Dialog function. In the
 *    special case of a modeless GetFile dialog (supported only on Mac
 *    OS X), the user can option-click on the open button to keep the
 *    dialog from being dismissed, but the kNavCBUserAction event is
 *    sent so the client can get the reply record and open the selected
 *    files.
 */
typedef UInt32 NavUserAction;
enum {

  /*
   * No action taken. The dialog is still running or was terminated
   * programmatically.
   */
  kNavUserActionNone            = 0,

  /*
   * The user cancelled the dialog.
   */
  kNavUserActionCancel          = 1,

  /*
   * The user clicked the Open button in the GetFile dialog.
   */
  kNavUserActionOpen            = 2,

  /*
   * The user clicked the Save button in the PutFile dialog.
   */
  kNavUserActionSaveAs          = 3,

  /*
   * The user clicked the Choose button in the ChooseFile,
   * ChooseFolder, ChooseVolume or ChooseObject dialogs.
   */
  kNavUserActionChoose          = 4,

  /*
   * The user clicked the New Folder button in the New Folder dialog.
   */
  kNavUserActionNewFolder       = 5,

  /*
   * The user clicked the Save button in an AskSaveChanges dialog.
   */
  kNavUserActionSaveChanges     = 6,

  /*
   * The user clicked the Don't Save button in an AskSaveChanges dialog.
   */
  kNavUserActionDontSaveChanges = 7,

  /*
   * The user clicked the Discard button in the AskDiscardChanges
   * dialog.
   */
  kNavUserActionDiscardChanges  = 8,

  /*
   * The user clicked the Review Unsaved button in the
   * AskReviewDocuments dialog (used only on Mac OS X).
   */
  kNavUserActionReviewDocuments = 9,

  /*
   * The user clicked the Discard Changes button in the
   * AskReviewDocuments dialog (used only on Mac OS X).
   */
  kNavUserActionDiscardDocuments = 10
};



enum {
  kNavCBRecVersion              = 1
};


/*
 *  NavCBRec
 *  
 *  Summary:
 *    A structure passed to event and preview callbacks
 *  
 *  Discussion:
 *    The NavCBRec structure is passed to the client's event proc or
 *    custom preview proc. It provides information that is specific to
 *    each event type. New for Carbon: the userAction field.
 */
struct NavCBRec {

  /*
   * The version of the struct (currently 1)
   */
  UInt16              version;

  /*
   * The NavDialogRef this callback with which this call is associated
   */
  NavDialogRef        context;

  /*
   * The dialog's window
   */
  WindowRef           window;

  /*
   * The custom control area rectangle (window coordinates)
   */
  Rect                customRect;

  /*
   * The custom preview area rectangle (window coordinates)
   */
  Rect                previewRect;

  /*
   * The event-specific data, including the EventRecord, if any
   */
  NavEventData        eventData;

  /*
   * The action taken by the user that generated a kNavCBUserAction
   * event (Carbon dialogs only)
   */
  NavUserAction       userAction;

  /*
   * Reserved for future use
   */
  char                reserved[218];
};
typedef struct NavCBRec                 NavCBRec;
typedef NavCBRec *                      NavCBRecPtr;

/*
 *  NavEventCallbackMessage
 *  
 *  Summary:
 *    Identifies the message type being sent to the client's event proc
 */
typedef SInt32 NavEventCallbackMessage;
enum {

  /*
   * An OS event has occurred. A pointer to the EventRecord is in the
   * eventData.eventDataParms.event field of the NavCBRec.
   */
  kNavCBEvent                   = 0,

  /*
   * Negotiate for custom control space. Client can set change the
   * customRect field in the NavCBRec to create space for a custom
   * area. Nav Services will continue to send the kNavCBCustomize
   * message until the client leaves the customRect field unchanged.
   */
  kNavCBCustomize               = 1,

  /*
   * This message is sent after custom area negotiation, just before
   * the dialog is made visible. Add your custom controls when you
   * receive this message.
   */
  kNavCBStart                   = 2,

  /*
   * This is the last message sent, after the dialog has been hidden.
   */
  kNavCBTerminate               = 3,

  /*
   * Sent when the dialog has been resized. Check the customRect and or
   * previewRect values to see if any relayout is needed. Nav Services
   * automatically moves controls in the custom area.
   */
  kNavCBAdjustRect              = 4,

  /*
   * The target folder of the dialog has changed. The
   * NavCBRec.eventData.eventDataParms.param field is an AEDesc*
   * containing an descriptor of the new location (ususally an FSSpec
   * or an FSRef).
   */
  kNavCBNewLocation             = 5,

  /*
   * The target folder has changed to the user's desktop folder.
   */
  kNavCBShowDesktop             = 6,

  /*
   * The user has selected or deselected a file or folder. The
   * NavCBRec.eventData.eventDataParms.param field is an AEDescList*
   * identifying the currently selected items.
   */
  kNavCBSelectEntry             = 7,

  /*
   * The value of the Show/Format popup menu has changed. The
   * NavCBRec.eventData.eventDataParms.param is a NavMenuItemSpec*
   * identifying the menu item selected. If the dialog was created
   * using the Carbon-only NavCreate*Dialog APIs, then the menuType
   * field of the NavMenuItemSpec is set to the index into the client's
   * CFArray of popupExtension strings (see NavDialogCreationOptions).
   */
  kNavCBPopupMenuSelect         = 8,

  /*
   * Sent when the user has accepted (Open, Save, etc.).
   */
  kNavCBAccept                  = 9,

  /*
   * Sent when the user has cancelled the dialog.
   */
  kNavCBCancel                  = 10,

  /*
   * The custom preview area state has changed. The
   * NavCBRec.eventData.eventDataParms.param is a Boolean* set to true
   * if the preview area is visible or false if it is not.
   */
  kNavCBAdjustPreview           = 11,

  /*
   * The user has taken one of the actions described in the
   * NavUserAction definition. The action may or may not dismiss the
   * dialog. The NavCBRec.userAction field indicates which action was
   * taken (Carbon dialogs only).
   */
  kNavCBUserAction              = 12,

  /*
   * The user has opened a folder or chosen a file. The client can
   * block navigation or dismissal by setting the appropriate action
   * state with the kNavCtlSetActionState NavCustomControl selector.
   */
  kNavCBOpenSelection           = (long)0x80000000
};


typedef void *                          NavCallBackUserData;
/* for events and customization: */
typedef CALLBACK_API( void , NavEventProcPtr )(NavEventCallbackMessage callBackSelector, NavCBRecPtr callBackParms, void *callBackUD);
/* for preview support: */
typedef CALLBACK_API( Boolean , NavPreviewProcPtr )(NavCBRecPtr callBackParms, void *callBackUD);
/* filtering callback information: */
typedef CALLBACK_API( Boolean , NavObjectFilterProcPtr )(AEDesc *theItem, void *info, void *callBackUD, NavFilterModes filterMode);
typedef STACK_UPP_TYPE(NavEventProcPtr)                         NavEventUPP;
typedef STACK_UPP_TYPE(NavPreviewProcPtr)                       NavPreviewUPP;
typedef STACK_UPP_TYPE(NavObjectFilterProcPtr)                  NavObjectFilterUPP;
/*
 *  NewNavEventUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( NavEventUPP )
NewNavEventUPP(NavEventProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppNavEventProcInfo = 0x00000FC0 };  /* pascal no_return_value Func(4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(NavEventUPP) NewNavEventUPP(NavEventProcPtr userRoutine) { return (NavEventUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppNavEventProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewNavEventUPP(userRoutine) (NavEventUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppNavEventProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewNavPreviewUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( NavPreviewUPP )
NewNavPreviewUPP(NavPreviewProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppNavPreviewProcInfo = 0x000003D0 };  /* pascal 1_byte Func(4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(NavPreviewUPP) NewNavPreviewUPP(NavPreviewProcPtr userRoutine) { return (NavPreviewUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppNavPreviewProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewNavPreviewUPP(userRoutine) (NavPreviewUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppNavPreviewProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewNavObjectFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( NavObjectFilterUPP )
NewNavObjectFilterUPP(NavObjectFilterProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppNavObjectFilterProcInfo = 0x00002FD0 };  /* pascal 1_byte Func(4_bytes, 4_bytes, 4_bytes, 2_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(NavObjectFilterUPP) NewNavObjectFilterUPP(NavObjectFilterProcPtr userRoutine) { return (NavObjectFilterUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppNavObjectFilterProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewNavObjectFilterUPP(userRoutine) (NavObjectFilterUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppNavObjectFilterProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeNavEventUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeNavEventUPP(NavEventUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeNavEventUPP(NavEventUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeNavEventUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeNavPreviewUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeNavPreviewUPP(NavPreviewUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeNavPreviewUPP(NavPreviewUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeNavPreviewUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeNavObjectFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeNavObjectFilterUPP(NavObjectFilterUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeNavObjectFilterUPP(NavObjectFilterUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeNavObjectFilterUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeNavEventUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeNavEventUPP(
  NavEventCallbackMessage  callBackSelector,
  NavCBRecPtr              callBackParms,
  void *                   callBackUD,
  NavEventUPP              userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeNavEventUPP(NavEventCallbackMessage callBackSelector, NavCBRecPtr callBackParms, void * callBackUD, NavEventUPP userUPP) { CALL_THREE_PARAMETER_UPP(userUPP, uppNavEventProcInfo, callBackSelector, callBackParms, callBackUD); }
  #else
    #define InvokeNavEventUPP(callBackSelector, callBackParms, callBackUD, userUPP) CALL_THREE_PARAMETER_UPP((userUPP), uppNavEventProcInfo, (callBackSelector), (callBackParms), (callBackUD))
  #endif
#endif

/*
 *  InvokeNavPreviewUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
InvokeNavPreviewUPP(
  NavCBRecPtr    callBackParms,
  void *         callBackUD,
  NavPreviewUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeNavPreviewUPP(NavCBRecPtr callBackParms, void * callBackUD, NavPreviewUPP userUPP) { return (Boolean)CALL_TWO_PARAMETER_UPP(userUPP, uppNavPreviewProcInfo, callBackParms, callBackUD); }
  #else
    #define InvokeNavPreviewUPP(callBackParms, callBackUD, userUPP) (Boolean)CALL_TWO_PARAMETER_UPP((userUPP), uppNavPreviewProcInfo, (callBackParms), (callBackUD))
  #endif
#endif

/*
 *  InvokeNavObjectFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
InvokeNavObjectFilterUPP(
  AEDesc *            theItem,
  void *              info,
  void *              callBackUD,
  NavFilterModes      filterMode,
  NavObjectFilterUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeNavObjectFilterUPP(AEDesc * theItem, void * info, void * callBackUD, NavFilterModes filterMode, NavObjectFilterUPP userUPP) { return (Boolean)CALL_FOUR_PARAMETER_UPP(userUPP, uppNavObjectFilterProcInfo, theItem, info, callBackUD, filterMode); }
  #else
    #define InvokeNavObjectFilterUPP(theItem, info, callBackUD, filterMode, userUPP) (Boolean)CALL_FOUR_PARAMETER_UPP((userUPP), uppNavObjectFilterProcInfo, (theItem), (info), (callBackUD), (filterMode))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewNavEventProc(userRoutine)                        NewNavEventUPP(userRoutine)
    #define NewNavPreviewProc(userRoutine)                      NewNavPreviewUPP(userRoutine)
    #define NewNavObjectFilterProc(userRoutine)                 NewNavObjectFilterUPP(userRoutine)
    #define CallNavEventProc(userRoutine, callBackSelector, callBackParms, callBackUD) InvokeNavEventUPP(callBackSelector, callBackParms, callBackUD, userRoutine)
    #define CallNavPreviewProc(userRoutine, callBackParms, callBackUD) InvokeNavPreviewUPP(callBackParms, callBackUD, userRoutine)
    #define CallNavObjectFilterProc(userRoutine, theItem, info, callBackUD, filterMode) InvokeNavObjectFilterUPP(theItem, info, callBackUD, filterMode, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

typedef SInt32 NavCustomControlMessage;
enum {
  kNavCtlShowDesktop            = 0,    /*    show desktop,           parms = nil */
  kNavCtlSortBy                 = 1,    /*    sort key field,       parms->NavSortKeyField */
  kNavCtlSortOrder              = 2,    /*    sort order,              parms->NavSortOrder */
  kNavCtlScrollHome             = 3,    /*    scroll list home,       parms = nil */
  kNavCtlScrollEnd              = 4,    /*    scroll list end,      parms = nil */
  kNavCtlPageUp                 = 5,    /*    page list up,          parms = nil */
  kNavCtlPageDown               = 6,    /*    page list down,          parms = nil */
  kNavCtlGetLocation            = 7,    /*    get current location,   parms<-AEDesc* */
  kNavCtlSetLocation            = 8,    /*    set current location,   parms->AEDesc* */
  kNavCtlGetSelection           = 9,    /*    get current selection,     parms<-AEDescList* */
  kNavCtlSetSelection           = 10,   /*    set current selection,     parms->AEDescList* */
  kNavCtlShowSelection          = 11,   /*    make selection visible,       parms = nil */
  kNavCtlOpenSelection          = 12,   /*    open view of selection,       parms = nil */
  kNavCtlEjectVolume            = 13,   /*    eject volume,          parms->vRefNum */
  kNavCtlNewFolder              = 14,   /*    create a new folder,     parms->StringPtr */
  kNavCtlCancel                 = 15,   /*    cancel dialog,          parms = nil */
  kNavCtlAccept                 = 16,   /*    accept dialog default,     parms = nil */
  kNavCtlIsPreviewShowing       = 17,   /*    query preview status,   parms<-Boolean */
  kNavCtlAddControl             = 18,   /*  add one control to dialog,    parms->ControlHandle */
  kNavCtlAddControlList         = 19,   /*    add control list to dialog,    parms->Handle (DITL rsrc) */
  kNavCtlGetFirstControlID      = 20,   /*    get 1st control ID,         parms<-UInt16 */
  kNavCtlSelectCustomType       = 21,   /*    select a custom menu item  parms->NavMenuItemSpec* */
  kNavCtlSelectAllType          = 22,   /*  select an "All" menu item parms->SInt16 */
  kNavCtlGetEditFileName        = 23,   /*    get save dlog's file name  parms<-StringPtr */
  kNavCtlSetEditFileName        = 24,   /*    set save dlog's file name  parms->StringPtr */
  kNavCtlSelectEditFileName     = 25,   /*    select save dlog file name parms->ControlEditTextSelectionRec*, v1.1 or greater */
  kNavCtlBrowserSelectAll       = 26,   /*  re-scan the browser list  parms = nil, v2.0 or greater */
  kNavCtlGotoParent             = 27,   /*  navigate to parent         parms = nil, v2.0 or greater */
  kNavCtlSetActionState         = 28,   /*  restrict navigation      parms->NavActionState (flags), v2.0 or greater */
  kNavCtlBrowserRedraw          = 29,   /*  rescan browser list      parms = nil, v2.0 or greater */
  kNavCtlTerminate              = 30    /*  terminate/dismiss dialog  parms = nil, v2.0 or greater */
};

typedef UInt32 NavActionState;
enum {
  kNavNormalState               = 0x00000000, /* normal/default state */
  kNavDontOpenState             = 0x00000001, /* disallow opening files/folders */
  kNavDontSaveState             = 0x00000002, /* disallow saving files */
  kNavDontChooseState           = 0x00000004, /* disallow choosing objects */
  kNavDontNewFolderState        = 0x00000010 /* disallow creating new folders */
};

typedef UInt16 NavPopupMenuItem;
enum {
  kNavAllKnownFiles             = 0,
  kNavAllReadableFiles          = 1,
  kNavAllFiles                  = 2
};

typedef UInt16 NavSortKeyField;
enum {
  kNavSortNameField             = 0,
  kNavSortDateField             = 1
};


typedef UInt16 NavSortOrder;
enum {
  kNavSortAscending             = 0,
  kNavSortDescending            = 1
};


typedef UInt32 NavDialogOptionFlags;
enum {
  kNavDefaultNavDlogOptions     = 0x000000E4, /* use defaults for all the options */
  kNavNoTypePopup               = 0x00000001, /* don't show file type/extension popup on Open/Save */
  kNavDontAutoTranslate         = 0x00000002, /* don't automatically translate on Open */
  kNavDontAddTranslateItems     = 0x00000004, /* don't add translation choices on Open/Save */
  kNavAllFilesInPopup           = 0x00000010, /* "All Files" menu item in the type popup on Open */
  kNavAllowStationery           = 0x00000020, /* allow saving of stationery files */
  kNavAllowPreviews             = 0x00000040, /* allow preview to show */
  kNavAllowMultipleFiles        = 0x00000080, /* allow multiple items to be selected */
  kNavAllowInvisibleFiles       = 0x00000100, /* allow invisible items to be shown */
  kNavDontResolveAliases        = 0x00000200, /* don't resolve aliases */
  kNavSelectDefaultLocation     = 0x00000400, /* make the default location the browser selection */
  kNavSelectAllReadableItem     = 0x00000800, /* make the dialog select "All Readable Documents" on open */
  kNavSupportPackages           = 0x00001000, /* recognize file system packages, v2.0 or greater */
  kNavAllowOpenPackages         = 0x00002000, /* allow opening of packages, v2.0 or greater */
  kNavDontAddRecents            = 0x00004000, /* don't add chosen objects to the recents list, v2.0 or greater */
  kNavDontUseCustomFrame        = 0x00008000, /* don't draw the custom area bevel frame, v2.0 or greater */
  kNavDontConfirmReplacement    = 0x00010000, /* don't show the "Replace File?" alert on save conflict, v3.0 or greater */
  kNavPreserveSaveFileExtension = 0x00020000 /* extension in default file name is preserved and initially hidden, v3.1 or greater */
};


typedef UInt32 NavTranslationOptions;
enum {
  kNavTranslateInPlace          = 0,    /*    translate in place, replacing translation source file (default for Save) */
  kNavTranslateCopy             = 1     /*    translate to a copy of the source file (default for Open) */
};


enum {
  kNavMenuItemSpecVersion       = 0
};

struct NavMenuItemSpec {
  UInt16              version;
  OSType              menuCreator;
  OSType              menuType;
  Str255              menuItemName;
  char                reserved[245];
};
typedef struct NavMenuItemSpec          NavMenuItemSpec;
typedef NavMenuItemSpec *               NavMenuItemSpecArrayPtr;
typedef NavMenuItemSpecArrayPtr *       NavMenuItemSpecArrayHandle;
typedef NavMenuItemSpecArrayPtr         NavMenuItemSpecPtr;
typedef NavMenuItemSpecArrayHandle      NavMenuItemSpecHandle;
enum {
  kNavGenericSignature          = FOUR_CHAR_CODE('****')
};

struct NavTypeList {
  OSType              componentSignature;
  short               reserved;
  short               osTypeCount;
  OSType              osType[1];
};
typedef struct NavTypeList              NavTypeList;
typedef NavTypeList *                   NavTypeListPtr;
typedef NavTypeListPtr *                NavTypeListHandle;
enum {
  kNavDialogOptionsVersion      = 0
};

struct NavDialogOptions {
  UInt16              version;
  NavDialogOptionFlags  dialogOptionFlags;    /* option flags for affecting the dialog's behavior */
  Point               location;               /* top-left location of the dialog, or {-1,-1} for default position */
  Str255              clientName;
  Str255              windowTitle;
  Str255              actionButtonLabel;      /* label of the default button (or null string for default) */
  Str255              cancelButtonLabel;      /* label of the cancel button (or null string for default) */
  Str255              savedFileName;          /* default name for text box in NavPutFile (or null string for default) */
  Str255              message;                /* custom message prompt (or null string for default) */
  UInt32              preferenceKey;          /* a key for to managing preferences for using multiple utility dialogs */
  NavMenuItemSpecArrayHandle  popupExtension; /* extended popup menu items, an array of NavMenuItemSpecs */
  char                reserved[494];
};
typedef struct NavDialogOptions         NavDialogOptions;
enum {
  kNavReplyRecordVersion        = 2
};


/*
 *  NavReplyRecord
 *  
 *  Summary:
 *    A structure describing the results of a Nav Services dialog
 *  
 *  Discussion:
 *    A reply record is the result of a Nav Services file dialog. Using
 *    the older API, which is always modal, the client passes the
 *    address of a reply record when invoking the dialog. In the Carbon
 *    API, dialogs may also be window modal or modeless, so the client
 *    requests the reply record by calling NavDialogGetReply when a
 *    kNavCBUserAction event is received. Either way, a reply record
 *    should be disposed of using NavDisposeReply.
 */
struct NavReplyRecord {

  /*
   * The version of the structure. The first public version of the
   * structure was version 0. Fields added after version 0, starting
   * with the saveFileName field, are noted below.
   */
  UInt16              version;

  /*
   * True if the reply contains a non-null selection
   */
  Boolean             validRecord;

  /*
   * True if this reply is from a PutFile dialog and the file to be
   * saved already exists and needs to be replaced. The user has
   * already been warned unless the kNavDontConfirmReplacement option
   * flag is used.
   */
  Boolean             replacing;

  /*
   * True if this reply is from a PutFile dialog and the user wants to
   * save the file as stationery.
   */
  Boolean             isStationery;

  /*
   * True if translation was performed on the file(s) to be opened or
   * if transtlation will be needed on the file to be saved.
   */
  Boolean             translationNeeded;

  /*
   * For GetFile or Choose dialogs, a list of items chosen by the user.
   * For the older NavPutFile dialog, a list containing one item: an
   * FSSpec of the file to be saved. ** IMPORTANT NOTE *** For the new
   * Carbon-only PutFile dialog created with NavCreatePutFileDialog,
   * the selection is a list containing one item: the DIRECTORY where
   * the file is to be saved. The file name is obtained from the
   * saveFileName field. When using the original modal API, each
   * descriptor will contain an FSSpec (typeFSS). When using the new
   * Carbon-only dialogs created via the NavCreate*Dialog functions,
   * each descriptor could contain either an FSSpec (typeFSS, used on
   * Mac OS 8 or 9) or an FSRef (typeFSRef, used on Mac OS X). This
   * divergence is caused by the need to use FSRef (for Unicode/HFS+
   * support) on Mac OS X, while being unable to provide FSRefs on Mac
   * OS 8.6.
   */
  AEDescList          selection;

  /*
   * For NavPutFile: the script system associated with the name of the
   * file to be saved.
   */
  ScriptCode          keyScript;

  /*
   * A handle to an array of type FileTranslationSpec. Each array entry
   * corresponds to an item in the selection and describes the
   * translation that was performed (GetFile) or needs to be performed
   * (PutFile) on that item.
   */
  FileTranslationSpecArrayHandle  fileTranslation;

  /*
   * Reserved for private use.
   */
  UInt32              reserved1;

  /*
   * Carbon PutFile dialog only: the name of the file to be saved. This
   * field contains the true file name to saved, even if the extension
   * will be hidden from the user. This field was added in structure
   * version 1.
   */
  CFStringRef         saveFileName;

  /*
   * The extension on the name of the saved file should be hidden. Once
   * the file has been saved, the client should call NavCompleteSave.
   * NavCompleteSave will take care of hiding the extension on the
   * file. However, the client needs to know that the extension is
   * hidden so that it can display the document name correctly in the
   * UI, such as in window titles and menus. This field is only used if
   * the client has r equested extension preservation using the
   * kNavPreserveSaveFileExtension dialog option flag. This field was
   * added in structure version 2.
   */
  Boolean             saveFileExtensionHidden;

  /*
   * Reserved for future use.
   */
  UInt8               reserved2;

  /*
   * Reserved for future use.
   */
  char                reserved[225];
};
typedef struct NavReplyRecord           NavReplyRecord;
/*
 *  NavLoad()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in NavigationLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
NavLoad(void);


/*
 *  NavUnload()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in NavigationLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
NavUnload(void);


/*
 *  NavLibraryVersion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in NavigationLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( UInt32 )
NavLibraryVersion(void);


/*
 *  NavGetDefaultDialogOptions()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in NavigationLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
NavGetDefaultDialogOptions(NavDialogOptions * dialogOptions);



/*
 *  NavGetFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in NavigationLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
NavGetFile(
  AEDesc *             defaultLocation,       /* can be NULL */
  NavReplyRecord *     reply,
  NavDialogOptions *   dialogOptions,         /* can be NULL */
  NavEventUPP          eventProc,             /* can be NULL */
  NavPreviewUPP        previewProc,           /* can be NULL */
  NavObjectFilterUPP   filterProc,            /* can be NULL */
  NavTypeListHandle    typeList,              /* can be NULL */
  void *               callBackUD);           /* can be NULL */


/*
 *  NavPutFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in NavigationLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
NavPutFile(
  AEDesc *            defaultLocation,       /* can be NULL */
  NavReplyRecord *    reply,
  NavDialogOptions *  dialogOptions,         /* can be NULL */
  NavEventUPP         eventProc,             /* can be NULL */
  OSType              fileType,
  OSType              fileCreator,
  void *              callBackUD);           /* can be NULL */


/*
 *  NavAskSaveChanges()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in NavigationLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
NavAskSaveChanges(
  NavDialogOptions *         dialogOptions,
  NavAskSaveChangesAction    action,
  NavAskSaveChangesResult *  reply,
  NavEventUPP                eventProc,           /* can be NULL */
  void *                     callBackUD);         /* can be NULL */


/*
 *  NavCustomAskSaveChanges()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in NavigationLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
NavCustomAskSaveChanges(
  NavDialogOptions *         dialogOptions,
  NavAskSaveChangesResult *  reply,
  NavEventUPP                eventProc,           /* can be NULL */
  void *                     callBackUD);         /* can be NULL */


/*
 *  NavAskDiscardChanges()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in NavigationLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
NavAskDiscardChanges(
  NavDialogOptions *            dialogOptions,
  NavAskDiscardChangesResult *  reply,
  NavEventUPP                   eventProc,           /* can be NULL */
  void *                        callBackUD);         /* can be NULL */


/*
 *  NavChooseFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in NavigationLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
NavChooseFile(
  AEDesc *             defaultLocation,       /* can be NULL */
  NavReplyRecord *     reply,
  NavDialogOptions *   dialogOptions,         /* can be NULL */
  NavEventUPP          eventProc,             /* can be NULL */
  NavPreviewUPP        previewProc,           /* can be NULL */
  NavObjectFilterUPP   filterProc,            /* can be NULL */
  NavTypeListHandle    typeList,              /* can be NULL */
  void *               callBackUD);           /* can be NULL */


/*
 *  NavChooseFolder()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in NavigationLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
NavChooseFolder(
  AEDesc *             defaultLocation,       /* can be NULL */
  NavReplyRecord *     reply,
  NavDialogOptions *   dialogOptions,         /* can be NULL */
  NavEventUPP          eventProc,             /* can be NULL */
  NavObjectFilterUPP   filterProc,            /* can be NULL */
  void *               callBackUD);           /* can be NULL */


/*
 *  NavChooseVolume()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in NavigationLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
NavChooseVolume(
  AEDesc *             defaultSelection,       /* can be NULL */
  NavReplyRecord *     reply,
  NavDialogOptions *   dialogOptions,          /* can be NULL */
  NavEventUPP          eventProc,              /* can be NULL */
  NavObjectFilterUPP   filterProc,             /* can be NULL */
  void *               callBackUD);            /* can be NULL */


/*
 *  NavChooseObject()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in NavigationLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
NavChooseObject(
  AEDesc *             defaultLocation,       /* can be NULL */
  NavReplyRecord *     reply,
  NavDialogOptions *   dialogOptions,         /* can be NULL */
  NavEventUPP          eventProc,             /* can be NULL */
  NavObjectFilterUPP   filterProc,            /* can be NULL */
  void *               callBackUD);           /* can be NULL */


/*
 *  NavNewFolder()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in NavigationLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
NavNewFolder(
  AEDesc *            defaultLocation,       /* can be NULL */
  NavReplyRecord *    reply,
  NavDialogOptions *  dialogOptions,         /* can be NULL */
  NavEventUPP         eventProc,             /* can be NULL */
  void *              callBackUD);           /* can be NULL */


/*
 *  NavTranslateFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in NavigationLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
NavTranslateFile(
  NavReplyRecord *        reply,
  NavTranslationOptions   howToTranslate);


/*
 *  NavCompleteSave()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in NavigationLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
NavCompleteSave(
  NavReplyRecord *        reply,
  NavTranslationOptions   howToTranslate);


/*
 *  NavCustomControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in NavigationLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
NavCustomControl(
  NavDialogRef              dialog,
  NavCustomControlMessage   selector,
  void *                    parms);


/*
 *  NavCreatePreview()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in NavigationLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
NavCreatePreview(
  AEDesc *      theObject,
  OSType        previewDataType,
  const void *  previewData,
  Size          previewDataSize);


/*
 *  NavDisposeReply()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in NavigationLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
NavDisposeReply(NavReplyRecord * reply);


/*
 *  NavServicesCanRun()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in NavigationLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API( Boolean )
NavServicesCanRun(void);



#if TARGET_RT_MAC_CFM
#ifdef __cplusplus
    inline pascal Boolean NavServicesAvailable() { return ((NavLibraryVersion != (void*)kUnresolvedCFragSymbolAddress) && NavServicesCanRun()); }
#else
    #define NavServicesAvailable()  ((NavLibraryVersion != (void*)kUnresolvedCFragSymbolAddress) && NavServicesCanRun())
#endif
#elif TARGET_RT_MAC_MACHO
/* Navigation is always available on OS X */
#ifdef __cplusplus
    inline pascal Boolean NavServicesAvailable() { return true; }
#else
    #define NavServicesAvailable()  (true)
#endif
#else
/* NavServicesAvailable() is implemented in Navigation.o for classic 68K clients*/
#if CALL_NOT_IN_CARBON
/*
 *  NavServicesAvailable()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Boolean )
NavServicesAvailable(void);


#endif  /* CALL_NOT_IN_CARBON */

#endif  /*  */


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

#endif /* __NAVIGATION__ */

