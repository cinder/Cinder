/*
     File:       HTMLRendering.h
 
     Contains:   HTML Rendering Library Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1999-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __HTMLRENDERING__
#define __HTMLRENDERING__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __FILES__
#include <Files.h>
#endif

#ifndef __CODEFRAGMENTS__
#include <CodeFragments.h>
#endif

#ifndef __CFDATA__
#include <CFData.h>
#endif

#ifndef __CFSTRING__
#include <CFString.h>
#endif

#ifndef __CFURL__
#include <CFURL.h>
#endif

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

#ifndef __EVENTS__
#include <Events.h>
#endif

#ifndef __CONTROLS__
#include <Controls.h>
#endif

#ifndef __HITOOLBOX__
#include <HIToolbox.h>
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

typedef struct OpaqueHRReference*       HRReference;
/*
 *  HRGetHTMLRenderingLibVersion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HRGetHTMLRenderingLibVersion(NumVersion * returnVers);


#if TARGET_RT_MAC_CFM
#ifdef __cplusplus
    inline pascal Boolean HRHTMLRenderingLibAvailable() { return ((HRGetHTMLRenderingLibVersion != (void*)kUnresolvedCFragSymbolAddress) ); }
#else
    #define HRHTMLRenderingLibAvailable()   ((HRGetHTMLRenderingLibVersion != (void*)kUnresolvedCFragSymbolAddress) )
#endif
#elif TARGET_RT_MAC_MACHO
#ifdef __cplusplus
    inline pascal Boolean HRHTMLRenderingLibAvailable() { return true; }
#else
    #define HRHTMLRenderingLibAvailable()   (true)
#endif
#endif  /*  */

enum {
  kHRRendererHTML32Type         = FOUR_CHAR_CODE('ht32') /* HTML 3.2 */
};


/*
 *  HRNewReference()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HRNewReference(
  HRReference *  hrRef,
  OSType         rendererType,
  GrafPtr        grafPtr);


/*
 *  HRDisposeReference()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HRDisposeReference(HRReference hrRef);



/*
 *  HRFreeMemory()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt32 )
HRFreeMemory(Size inBytesNeeded);



/* System level notifications */
/*
 *  HRScreenConfigurationChanged()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
HRScreenConfigurationChanged(void);


/*
 *  HRIsHREvent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
HRIsHREvent(const EventRecord * eventRecord);



/* Drawing */
/*
 *  HRSetGrafPtr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HRSetGrafPtr(
  HRReference   hrRef,
  GrafPtr       grafPtr);


/*
 *  HRActivate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HRActivate(HRReference hrRef);


/*
 *  HRDeactivate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HRDeactivate(HRReference hrRef);


/*
 *  HRDraw()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HRDraw(
  HRReference   hrRef,
  RgnHandle     updateRgnH);


/*
 *  HRSetRenderingRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HRSetRenderingRect(
  HRReference   hrRef,
  const Rect *  renderingRect);


/*
 *  HRGetRenderedImageSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HRGetRenderedImageSize(
  HRReference   hrRef,
  Point *       renderingSize);


/*
 *  HRScrollToLocation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HRScrollToLocation(
  HRReference   hrRef,
  Point *       location);


/*
 *  HRForceQuickdraw()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HRForceQuickdraw(
  HRReference   hrRef,
  Boolean       forceQuickdraw);


typedef SInt16 HRScrollbarState;
enum {
  eHRScrollbarOn                = 0,
  eHRScrollbarOff               = 1,
  eHRScrollbarAuto              = 2
};

/*
 *  HRSetScrollbarState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HRSetScrollbarState(
  HRReference        hrRef,
  HRScrollbarState   hScrollbarState,
  HRScrollbarState   vScrollbarState);


/*
 *  HRSetDrawBorder()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HRSetDrawBorder(
  HRReference   hrRef,
  Boolean       drawBorder);


/*
 *  HRSetGrowboxCutout()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HRSetGrowboxCutout(
  HRReference   hrRef,
  Boolean       allowCutout);


/* Navigation */
/*
 *  HRGoToFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HRGoToFile(
  HRReference     hrRef,
  const FSSpec *  fsspec,
  Boolean         addToHistory,
  Boolean         forceRefresh);


/*
 *  HRGoToURL()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HRGoToURL(
  HRReference   hrRef,
  const char *  url,
  Boolean       addToHistory,
  Boolean       forceRefresh);


/*
 *  HRGoToAnchor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HRGoToAnchor(
  HRReference   hrRef,
  const char *  anchorName);


/*
 *  HRGoToPtr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HRGoToPtr(
  HRReference   hrRef,
  char *        buffer,
  UInt32        bufferSize,
  Boolean       addToHistory,
  Boolean       forceRefresh);


/* Accessors */
/* either file url or url of <base> tag */
/*
 *  HRGetRootURL()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HRGetRootURL(
  HRReference   hrRef,
  Handle        rootURLH);


/* url of <base> tag */
/*
 *  HRGetBaseURL()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HRGetBaseURL(
  HRReference   hrRef,
  Handle        baseURLH);


/* file url */
/*
 *  HRGetHTMLURL()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HRGetHTMLURL(
  HRReference   hrRef,
  Handle        HTMLURLH);


/*
 *  HRGetTitle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HRGetTitle(
  HRReference   hrRef,
  StringPtr     title);


/*
 *  HRGetHTMLFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HRGetHTMLFile(
  HRReference   hrRef,
  FSSpec *      fsspec);



/* Utilities */
/*
 *  HRUtilCreateFullURL()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HRUtilCreateFullURL(
  const char *  rootURL,
  const char *  linkURL,
  Handle        fullURLH);


/*
 *  HRUtilGetFSSpecFromURL()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HRUtilGetFSSpecFromURL(
  const char *  rootURL,
  const char *  linkURL,
  FSSpec *      destSpec);


/* urlHandle should be valid on input */
/*
 *  HRUtilGetURLFromFSSpec()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HRUtilGetURLFromFSSpec(
  const FSSpec *  fsspec,
  Handle          urlHandle);



/*
    Visited links

    If you register a function here, it will be called to determine
    whether or not the given URL has been visited. It should return
    true if the URL has been visited.
    
    In addition to the URLs that the application may add to the list
    of visited links, it should also add URLs that the user clicks
    on. These URLs can be caught by the "add URL to history" callback
    below.
 */
typedef CALLBACK_API( Boolean , HRWasURLVisitedProcPtr )(const char *url, void *refCon);
typedef STACK_UPP_TYPE(HRWasURLVisitedProcPtr)                  HRWasURLVisitedUPP;
/*
 *  HRRegisterWasURLVisitedUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
HRRegisterWasURLVisitedUPP(
  HRWasURLVisitedUPP   inWasURLVisitedUPP,
  HRReference          hrRef,
  void *               inRefCon);


/*
 *  HRUnregisterWasURLVisitedUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
HRUnregisterWasURLVisitedUPP(HRReference hrRef);



/*
    New URL

    If you register a function here, it will be called every time
    the renderer is going to display a new URL. A few examples of how
    you might use this include...
    
        (a) maintaining a history of URLs
        (b) maintainging a list of visited links
        (c) setting a window title based on the new URL
*/
typedef CALLBACK_API( OSStatus , HRNewURLProcPtr )(const char *url, const char *targetFrame, Boolean addToHistory, void *refCon);
typedef STACK_UPP_TYPE(HRNewURLProcPtr)                         HRNewURLUPP;
/*
 *  HRRegisterNewURLUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
HRRegisterNewURLUPP(
  HRNewURLUPP   inNewURLUPP,
  HRReference   hrRef,
  void *        inRefCon);


/*
 *  HRUnregisterNewURLUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
HRUnregisterNewURLUPP(HRReference hrRef);






/*
    URL to FSSpec function

    If you register a function here, it will be called every time
    the renderer is going to locate a file. The function will be
    passed an enum indicating the type of file being asked for.
 */
typedef UInt16 URLSourceType;
enum {
  kHRLookingForHTMLSource       = 1,
  kHRLookingForImage            = 2,
  kHRLookingForEmbedded         = 3,
  kHRLookingForImageMap         = 4,
  kHRLookingForFrame            = 5
};

typedef CALLBACK_API( OSStatus , HRURLToFSSpecProcPtr )(const char *rootURL, const char *linkURL, FSSpec *fsspec, URLSourceType urlSourceType, void *refCon);
typedef STACK_UPP_TYPE(HRURLToFSSpecProcPtr)                    HRURLToFSSpecUPP;
/*
 *  HRRegisterURLToFSSpecUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
HRRegisterURLToFSSpecUPP(
  HRURLToFSSpecUPP   inURLToFSSpecUPP,
  HRReference        hrRef,
  void *             inRefCon);


/*
 *  HRUnregisterURLToFSSpecUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in HTMLRenderingLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
HRUnregisterURLToFSSpecUPP(HRReference hrRef);



/*
 *  NewHRWasURLVisitedUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( HRWasURLVisitedUPP )
NewHRWasURLVisitedUPP(HRWasURLVisitedProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppHRWasURLVisitedProcInfo = 0x000003D0 };  /* pascal 1_byte Func(4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(HRWasURLVisitedUPP) NewHRWasURLVisitedUPP(HRWasURLVisitedProcPtr userRoutine) { return (HRWasURLVisitedUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppHRWasURLVisitedProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewHRWasURLVisitedUPP(userRoutine) (HRWasURLVisitedUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppHRWasURLVisitedProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewHRWasCFURLVisitedUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
/*
 *  NewHRNewURLUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( HRNewURLUPP )
NewHRNewURLUPP(HRNewURLProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppHRNewURLProcInfo = 0x000037F0 };  /* pascal 4_bytes Func(4_bytes, 4_bytes, 1_byte, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(HRNewURLUPP) NewHRNewURLUPP(HRNewURLProcPtr userRoutine) { return (HRNewURLUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppHRNewURLProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewHRNewURLUPP(userRoutine) (HRNewURLUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppHRNewURLProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewHRNewCFURLUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
/*
 *  NewHRURLToFSSpecUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( HRURLToFSSpecUPP )
NewHRURLToFSSpecUPP(HRURLToFSSpecProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppHRURLToFSSpecProcInfo = 0x0000EFF0 };  /* pascal 4_bytes Func(4_bytes, 4_bytes, 4_bytes, 2_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(HRURLToFSSpecUPP) NewHRURLToFSSpecUPP(HRURLToFSSpecProcPtr userRoutine) { return (HRURLToFSSpecUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppHRURLToFSSpecProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewHRURLToFSSpecUPP(userRoutine) (HRURLToFSSpecUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppHRURLToFSSpecProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewHRURLToFSRefUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
/*
 *  DisposeHRWasURLVisitedUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeHRWasURLVisitedUPP(HRWasURLVisitedUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeHRWasURLVisitedUPP(HRWasURLVisitedUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeHRWasURLVisitedUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeHRWasCFURLVisitedUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
/*
 *  DisposeHRNewURLUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeHRNewURLUPP(HRNewURLUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeHRNewURLUPP(HRNewURLUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeHRNewURLUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeHRNewCFURLUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
/*
 *  DisposeHRURLToFSSpecUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeHRURLToFSSpecUPP(HRURLToFSSpecUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeHRURLToFSSpecUPP(HRURLToFSSpecUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeHRURLToFSSpecUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeHRURLToFSRefUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
/*
 *  InvokeHRWasURLVisitedUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
InvokeHRWasURLVisitedUPP(
  const char *        url,
  void *              refCon,
  HRWasURLVisitedUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeHRWasURLVisitedUPP(const char * url, void * refCon, HRWasURLVisitedUPP userUPP) { return (Boolean)CALL_TWO_PARAMETER_UPP(userUPP, uppHRWasURLVisitedProcInfo, url, refCon); }
  #else
    #define InvokeHRWasURLVisitedUPP(url, refCon, userUPP) (Boolean)CALL_TWO_PARAMETER_UPP((userUPP), uppHRWasURLVisitedProcInfo, (url), (refCon))
  #endif
#endif

/*
 *  InvokeHRWasCFURLVisitedUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
/*
 *  InvokeHRNewURLUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
InvokeHRNewURLUPP(
  const char *  url,
  const char *  targetFrame,
  Boolean       addToHistory,
  void *        refCon,
  HRNewURLUPP   userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSStatus) InvokeHRNewURLUPP(const char * url, const char * targetFrame, Boolean addToHistory, void * refCon, HRNewURLUPP userUPP) { return (OSStatus)CALL_FOUR_PARAMETER_UPP(userUPP, uppHRNewURLProcInfo, url, targetFrame, addToHistory, refCon); }
  #else
    #define InvokeHRNewURLUPP(url, targetFrame, addToHistory, refCon, userUPP) (OSStatus)CALL_FOUR_PARAMETER_UPP((userUPP), uppHRNewURLProcInfo, (url), (targetFrame), (addToHistory), (refCon))
  #endif
#endif

/*
 *  InvokeHRNewCFURLUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
/*
 *  InvokeHRURLToFSSpecUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
InvokeHRURLToFSSpecUPP(
  const char *      rootURL,
  const char *      linkURL,
  FSSpec *          fsspec,
  URLSourceType     urlSourceType,
  void *            refCon,
  HRURLToFSSpecUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSStatus) InvokeHRURLToFSSpecUPP(const char * rootURL, const char * linkURL, FSSpec * fsspec, URLSourceType urlSourceType, void * refCon, HRURLToFSSpecUPP userUPP) { return (OSStatus)CALL_FIVE_PARAMETER_UPP(userUPP, uppHRURLToFSSpecProcInfo, rootURL, linkURL, fsspec, urlSourceType, refCon); }
  #else
    #define InvokeHRURLToFSSpecUPP(rootURL, linkURL, fsspec, urlSourceType, refCon, userUPP) (OSStatus)CALL_FIVE_PARAMETER_UPP((userUPP), uppHRURLToFSSpecProcInfo, (rootURL), (linkURL), (fsspec), (urlSourceType), (refCon))
  #endif
#endif

/*
 *  InvokeHRURLToFSRefUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewHRWasURLVisitedProc(userRoutine)                 NewHRWasURLVisitedUPP(userRoutine)
    #define NewHRNewURLProc(userRoutine)                        NewHRNewURLUPP(userRoutine)
    #define NewHRURLToFSSpecProc(userRoutine)                   NewHRURLToFSSpecUPP(userRoutine)
    #define CallHRWasURLVisitedProc(userRoutine, url, refCon)   InvokeHRWasURLVisitedUPP(url, refCon, userRoutine)
    #define CallHRNewURLProc(userRoutine, url, targetFrame, addToHistory, refCon) InvokeHRNewURLUPP(url, targetFrame, addToHistory, refCon, userRoutine)
    #define CallHRURLToFSSpecProc(userRoutine, rootURL, linkURL, fsspec, urlSourceType, refCon) InvokeHRURLToFSSpecUPP(rootURL, linkURL, fsspec, urlSourceType, refCon, userRoutine)
#endif /* CALL_NOT_IN_CARBON */


#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* __HTMLRENDERING__ */

