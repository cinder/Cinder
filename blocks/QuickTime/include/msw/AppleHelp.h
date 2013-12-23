/*
     File:       AppleHelp.h
 
     Contains:   Apple Help
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2000-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __APPLEHELP__
#define __APPLEHELP__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __FILES__
#include <Files.h>
#endif

#ifndef __CFSTRING__
#include <CFString.h>
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

/* AppleHelp Error Codes */
enum {
  kAHInternalErr                = -10790,
  kAHInternetConfigPrefErr      = -10791
};


typedef SInt16 AHTOCType;
enum {
  kAHTOCTypeUser                = 0,
  kAHTOCTypeDeveloper           = 1
};

/*
 *  AHSearch()
 *  
 *  Discussion:
 *    Delivers a request to perform the specified search to the Help
 *    Viewer application.
 *  
 *  Parameters:
 *    
 *    bookname:
 *      Optionally, the AppleTitle of the Help book to be searched. If
 *      NULL, all installed Help books are searched.
 *    
 *    query:
 *      The query to be made. This string can, if desired, have boolean
 *      operators or be a natural language phrase.
 *  
 *  Result:
 *    An operating system result code that indicates whether the
 *    request was successfully sent to the Help Viewer application.
 *    Possible values: noErr, paramErr, kAHInternalErr,
 *    kAHInternetConfigPrefErr.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AHSearch(
  CFStringRef   bookname,
  CFStringRef   query);


/*
 *  AHGotoMainTOC()
 *  
 *  Discussion:
 *    Delivers a request to load the main table of contents of
 *    installed help books to the Help Viewer application.
 *  
 *  Parameters:
 *    
 *    toctype:
 *      The type of table of contents to be loaded: user or developer.
 *  
 *  Result:
 *    An operating system result code that indicates whether the
 *    request was successfully sent to the Help Viewer application.
 *    Possible values: noErr, paramErr, kAHInternalErr,
 *    kAHInternetConfigPrefErr.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AHGotoMainTOC(AHTOCType toctype);


/*
 *  AHGotoPage()
 *  
 *  Discussion:
 *    Delivers a request to load a specific text/html file to the Help
 *    Viewer application.
 *  
 *  Parameters:
 *    
 *    bookname:
 *      Optionally, the AppleTitle of an installed Help book. If NULL,
 *      the path parameter must be a full file: URL to the file to be
 *      opened.
 *    
 *    path:
 *      Optionally, one of two types of paths: 1) a URL-style path to a
 *      file that is relative to the main folder of the book supplied
 *      in the bookname parameter, or 2) if bookname is NULL, a full
 *      file: URL to the file to be opened. If this parameter is NULL,
 *      then bookname must not be NULL, and is used to open the Help
 *      Viewer to the main page of Help content for the specified book.
 *    
 *    anchor:
 *      Optionally, the name of anchor tag to scroll to in the newly
 *      opened file. Can be NULL.
 *  
 *  Result:
 *    An operating system result code that indicates whether the
 *    request was successfully sent to the Help Viewer application.
 *    Possible values: noErr, paramErr, kAHInternalErr,
 *    kAHInternetConfigPrefErr.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AHGotoPage(
  CFStringRef   bookname,
  CFStringRef   path,
  CFStringRef   anchor);


/*
 *  AHLookupAnchor()
 *  
 *  Discussion:
 *    Delivers a request to perform an anchor lookup to the Help Viewer
 *    application. Note: anchor lookups will fail unless you have
 *    indexed your help content with anchor indexing turned on in the
 *    indexing tool's preferences panel.
 *  
 *  Parameters:
 *    
 *    bookname:
 *      Optionally, the AppleTitle of the Help book to searched. If
 *      NULL, the anchor lookup is performed using all installed Help
 *      books.
 *    
 *    anchor:
 *      The name of the anchor tag to look up.
 *  
 *  Result:
 *    An operating system result code that indicates whether the
 *    request was successfully sent to the Help Viewer application.
 *    Possible values: noErr, paramErr, kAHInternalErr,
 *    kAHInternetConfigPrefErr.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AHLookupAnchor(
  CFStringRef   bookname,
  CFStringRef   anchor);



/*
 *  AHRegisterHelpBook()
 *  
 *  Discussion:
 *    Registers a book of Help content such that the book will appear
 *    in the current user's main table of contents (Help Center) in the
 *    Help Viewer application. To be used when help books reside
 *    outside of the known help folders (i.e. help books that are kept
 *    inside of application bundles).
 *  
 *  Parameters:
 *    
 *    appBundleRef:
 *      An FSRef pointer to the bundle within which one or more Help
 *      books is stored. This is likely an FSRef to your application's
 *      main bundle.
 *  
 *  Result:
 *    An operating system result code that indicates whether all help
 *    books contained within the specified bundle were registered.
 *    Possible values: noErr, paramErr, kAHInternalErr, dirNFErr.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AHRegisterHelpBook(const FSRef * appBundleRef);



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

#endif /* __APPLEHELP__ */

