/*
     File:       NSL.h
 
     Contains:   Interface to API for using the NSL User Interface
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1997-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __NSL__
#define __NSL__

#ifndef __NSLCORE__
#include <NSLCore.h>
#endif

#ifndef __EVENTS__
#include <Events.h>
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

typedef UInt32 NSLDialogOptionFlags;
enum {
  kNSLDefaultNSLDlogOptions     = 0x00000000, /* use defaults for all the options */
  kNSLNoURLTEField              = 0x00000001, /* don't show url text field for manual entry */
  kNSLAddServiceTypes           = 0x00000002, /* add the service type if a user enters an incomplete URL */
  kNSLClientHandlesRecents      = 0x00000004 /* Stops NSLStandardGetURL from adding the selection to the recent items folder */
};


struct NSLDialogOptions {
  UInt16              version;
  NSLDialogOptionFlags  dialogOptionFlags;    /* option flags for affecting the dialog's behavior */
  Str255              windowTitle;
  Str255              actionButtonLabel;      /* label of the default button (or null string for default) */
  Str255              cancelButtonLabel;      /* label of the cancel button (or null string for default) */
  Str255              message;                /* custom message prompt (or null string for default) */
};
typedef struct NSLDialogOptions         NSLDialogOptions;
typedef CALLBACK_API( Boolean , NSLURLFilterProcPtr )(char *url, Str255 displayString);
/* you can provide for calls to NSLStandardGetURL*/
typedef CALLBACK_API( void , NSLEventProcPtr )(EventRecord *newEvent, void *userContext);
typedef STACK_UPP_TYPE(NSLURLFilterProcPtr)                     NSLURLFilterUPP;
typedef STACK_UPP_TYPE(NSLEventProcPtr)                         NSLEventUPP;
/*
 *  NewNSLURLFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( NSLURLFilterUPP )
NewNSLURLFilterUPP(NSLURLFilterProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppNSLURLFilterProcInfo = 0x000003D0 };  /* pascal 1_byte Func(4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(NSLURLFilterUPP) NewNSLURLFilterUPP(NSLURLFilterProcPtr userRoutine) { return (NSLURLFilterUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppNSLURLFilterProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewNSLURLFilterUPP(userRoutine) (NSLURLFilterUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppNSLURLFilterProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewNSLEventUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( NSLEventUPP )
NewNSLEventUPP(NSLEventProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppNSLEventProcInfo = 0x000003C0 };  /* pascal no_return_value Func(4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(NSLEventUPP) NewNSLEventUPP(NSLEventProcPtr userRoutine) { return (NSLEventUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppNSLEventProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewNSLEventUPP(userRoutine) (NSLEventUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppNSLEventProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeNSLURLFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeNSLURLFilterUPP(NSLURLFilterUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeNSLURLFilterUPP(NSLURLFilterUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeNSLURLFilterUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeNSLEventUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeNSLEventUPP(NSLEventUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeNSLEventUPP(NSLEventUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeNSLEventUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeNSLURLFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
InvokeNSLURLFilterUPP(
  char *           url,
  Str255           displayString,
  NSLURLFilterUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeNSLURLFilterUPP(char * url, Str255 displayString, NSLURLFilterUPP userUPP) { return (Boolean)CALL_TWO_PARAMETER_UPP(userUPP, uppNSLURLFilterProcInfo, url, displayString); }
  #else
    #define InvokeNSLURLFilterUPP(url, displayString, userUPP) (Boolean)CALL_TWO_PARAMETER_UPP((userUPP), uppNSLURLFilterProcInfo, (url), (displayString))
  #endif
#endif

/*
 *  InvokeNSLEventUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeNSLEventUPP(
  EventRecord *  newEvent,
  void *         userContext,
  NSLEventUPP    userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeNSLEventUPP(EventRecord * newEvent, void * userContext, NSLEventUPP userUPP) { CALL_TWO_PARAMETER_UPP(userUPP, uppNSLEventProcInfo, newEvent, userContext); }
  #else
    #define InvokeNSLEventUPP(newEvent, userContext, userUPP) CALL_TWO_PARAMETER_UPP((userUPP), uppNSLEventProcInfo, (newEvent), (userContext))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewNSLURLFilterProc(userRoutine)                    NewNSLURLFilterUPP(userRoutine)
    #define NewNSLEventProc(userRoutine)                        NewNSLEventUPP(userRoutine)
    #define CallNSLURLFilterProc(userRoutine, url, displayString) InvokeNSLURLFilterUPP(url, displayString, userRoutine)
    #define CallNSLEventProc(userRoutine, newEvent, userContext) InvokeNSLEventUPP(newEvent, userContext, userRoutine)
#endif /* CALL_NOT_IN_CARBON */


/* <--- function returns OSStatus of the operation.  noErr will be returned if valid, kNSLUserCanceled will be returned if the user cancels */
/* ---> dialogOptions */
/* ---> eventProc */
/* ---> eventProcContextPtr */
/* ---> filterProc */
/* ---> serviceTypeList */
/* <--- userSelectedURL */
/* NSLDialogOptions* dialogOptions */
/*
   dialogOptions is a user defined structure defining the look, feel and operation of NSLStandardGetURL dialog
   default behavior can be achieved by passing in a pointer to a structure that has been filled out by a previous
   call to NSLGetDefaultDialogOptions or by passing in NULL.
*/
/* NSLEventUPP eventProc */
/*
   the eventProc is a callback NSLURLFilterUPP that will
   get called with Events that the dialog doesn't handle.  If you pass in nil,
   you won't get update events while the NSLStandardGetURL dialog is open.
*/
/* void* eventProcContextPtr */
/* you can provide a pointer to some contextual data that you want to have sent to your eventProc filter*/
/* NSLURLFilterProcPtr filterProc */
/*
   the filter param is a callback NSLURLFilterUPP that
   will get called (if not nil) for each url that is going to be displayed in
   the dialog's result list.  A result of false will not include the url for the
   user to select from.  You also have the option of handling the way the url looks
   in the dialog listing by copying the preferred name into the displayString
   parameter.  (If left alone, NSLStandardGetURL dialog will strip the service type
   portion off the url).
*/
/* char* serviceTypeList */
/*
   the serviceTypeList parameter is a null terminated string that will 
   directly affect the contents of the services popup in the dialog.
   The structure of this string is a set of tuples as follows:
   Name of ServiceType as to be represented in the popup followed by
   a comma delimted list of service descriptors (ie http,https) that will
   used in the search of that type.  Each comma delimited tuple is delimited
   by semi-colons.
*/
/*
   For example:
   If you want to search for services of type http (web), https (secure web),
   and ftp, you could pass in the string "Web Servers,http,https;FTP Servers,ftp".
   This would result in a popup list with two items ("Web Servers" and "FTP Servers")
   and searches performed on them will provide results of type http and https for the
   first, and ftp for the second.
*/

/*
   Results list Icons:
   NSLStandardGetURL provides icons in its listings for the following types:
   "http", "https", "ftp", "afp", "lpr", "LaserWriter", "AFPServer"
   any other types will get a generic icon.  However, you can provide icons
   if you wish by including an '#ics8' resource id at the end of your comma
   delimited list.  The dialog will then use that icon if found in its results
   list.  This icon will be used for all types in a tuple.
   For example:
   The param "Web Servers,http,https;Telnet Servers,telnet;NFS Servers,nfs,129"
   would result in lists of http and https services to be shown with their default
   icons, telnet servers would be shown with the default misc. icon and nfs
   servers would be shown with your icon at resource id 129.
*/

/* char** url */
/*
   pass in the address of a char* and it will point to the resulting url.  If the user
   cancels (the function returns false), the pointer will be set to nil.  If the function
   returns true (user selected a url), then you must call NSLFreeURL on the pointer when
   you are done with it.
*/
/*
   Call this to have the user select a url based service from a dialog.
   Function takes on input an optional filter proc, a serviceTypeList, and an address to a Ptr.
   Function sets the value of the Ptr to a newly created c-style null terminated string
   containing the user's choice of URL.
*/

/*
 *  NSLStandardGetURL()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in NSLPPCLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
NSLStandardGetURL(
  NSLDialogOptions *  dialogOptions,             /* can be NULL */
  NSLEventUPP         eventProc,                 /* can be NULL */
  void *              eventProcContextPtr,       /* can be NULL */
  NSLURLFilterUPP     filterProc,                /* can be NULL */
  char *              serviceTypeList,
  char **             userSelectedURL);


/*
 *  NSLGetDefaultDialogOptions()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in NSLPPCLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
NSLGetDefaultDialogOptions(NSLDialogOptions * dialogOptions);


/* <--- function returns null (useful for setting variable at same time as freeing it */
/* ---> url is memory created by a call to NSLStandardGetURL */
/*
 *  NSLFreeURL()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in NSLPPCLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( char * )
NSLFreeURL(char * url);


/* <--- function returns kNSLErrNullPtrError, file mgr errors, or resource mgr errors */
/* ---> folderSelectorType is one of the well-known folders defined in Folders.h */
/* ---> url is any valid url */
/* ---> userFriendlyName is used for the file name and the display name (in the UI) */
/*
 *  NSLSaveURLAliasToFolder()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in NSLPPCLib 1.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
NSLSaveURLAliasToFolder(
  OSType        folderSelectorType,
  const char *  url,
  const char *  userFriendlyName);





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

#endif /* __NSL__ */

