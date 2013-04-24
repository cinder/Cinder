/*
     File:       HIMovieView.h
 
     Contains:   HIView-based movie playback
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2004-2007 by Apple Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __HIMOVIEVIEW__
#define __HIMOVIEVIEW__

#ifndef __HIVIEW__
#include <HIView.h>
#endif

#ifndef __MOVIES__
#include <Movies.h>
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

/* QuickTime is not available to 64-bit clients */

#if !__LP64__

#if TARGET_OS_MAC

/*
 *  kHIMovieViewClassID
 *  
 *  Summary:
 *    Class ID for HIMovieView
 */
#define kHIMovieViewClassID             CFSTR("com.apple.quicktime.HIMovieView")

/*
 *  Summary:
 *    HIMovieView Event class
 */
enum {

  /*
   * Events related to movie views.
   */
  kEventClassMovieView          = FOUR_CHAR_CODE('moov')
};


/*
 *  kEventClassMovieView / kEventMovieViewOptimalBoundsChanged
 *  
 *  Summary:
 *    Sent when the movie size changes.
 *  
 *  Parameters:
 *    
 *    --> kEventParamDirectObject (in, typeControlRef)
 *          The movie view whose size is changing.
 *    
 *    --> kEventParamControlOptimalBounds (in, typeHIRect)
 *          The new optimal bounds.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 */
enum {
  kEventMovieViewOptimalBoundsChanged = 1
};



/*
 *  Summary:
 *    HIMovieView attributes
 */
enum {

  /*
   * No attributes
   */
  kHIMovieViewNoAttributes      = 0L,

  /*
   * Movie controller bar is visible below visual content
   */
  kHIMovieViewControllerVisibleAttribute = (1L << 0),

  /*
   * Automatically call MCIdle() at appropriate times
   */
  kHIMovieViewAutoIdlingAttribute = (1L << 1),

  /*
   * Accepts keyboard focus
   */
  kHIMovieViewAcceptsFocusAttribute = (1L << 2),

  /*
   * Movie editing enabled
   */
  kHIMovieViewEditableAttribute = (1L << 3),

  /*
   * Handles editing HI commands such as cut, copy and paste
   */
  kHIMovieViewHandleEditingHIAttribute = (1L << 4),

  /*
   * Combination of kHIMovieViewControllerVisibleAttribute,
   * kHIMovieViewAutoIdlingAttribute, and
   * kHIMovieViewAcceptsFocusAttribute
   */
  kHIMovieViewStandardAttributes = (kHIMovieViewControllerVisibleAttribute | kHIMovieViewAutoIdlingAttribute | kHIMovieViewAcceptsFocusAttribute)
};

/*
 *  HIMovieViewCreate()
 *  
 *  Summary:
 *    Creates an HIMovieView object
 *  
 *  Discussion:
 *    If successful, the created view will have a single retain count.
 *  
 *  Parameters:
 *    
 *    inMovie:
 *      [in]  Initial movie to view, may be NULL
 *    
 *    inAttributes:
 *      [in]  Initial HIMovieView attributes
 *    
 *    outMovieView:
 *      [out] Points to variable to receive new HIMovieView
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
HIMovieViewCreate(
  Movie        inMovie,
  OptionBits   inAttributes,
  HIViewRef *  outMovieView);


/*
 *  HIMovieViewGetMovie()
 *  
 *  Summary:
 *    Returns the view's current movie.
 *  
 *  Parameters:
 *    
 *    inView:
 *      [in]  The HIMovieView
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( Movie )
HIMovieViewGetMovie(HIViewRef inView);


/*
 *  HIMovieViewSetMovie()
 *  
 *  Summary:
 *    Sets the view's current movie.
 *  
 *  Parameters:
 *    
 *    inView:
 *      [in]  The HIMovieView
 *    
 *    inMovie:
 *      [in]  The new movie to display
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
HIMovieViewSetMovie(
  HIViewRef   inView,
  Movie       inMovie);


/*
 *  HIMovieViewGetAttributes()
 *  
 *  Summary:
 *    Returns the view's current attributes.
 *  
 *  Parameters:
 *    
 *    inView:
 *      [in]  The HIMovieView
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OptionBits )
HIMovieViewGetAttributes(HIViewRef inView);


/*
 *  HIMovieViewChangeAttributes()
 *  
 *  Summary:
 *    Changes the views attributes.
 *  
 *  Discussion:
 *    Setting an attribute takes precedence over clearing the attribute.
 *  
 *  Parameters:
 *    
 *    inView:
 *      [in]  The HIMovieView
 *    
 *    inAttributesToSet:
 *      [in]  Attributes to set
 *    
 *    inAttributesToClear:
 *      [in]  Attributes to clear
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
HIMovieViewChangeAttributes(
  HIViewRef    inView,
  OptionBits   inAttributesToSet,
  OptionBits   inAttributesToClear);


/*
 *  HIMovieViewGetMovieController()
 *  
 *  Summary:
 *    Returns the view's current movie controller.
 *  
 *  Parameters:
 *    
 *    inView:
 *      [in]  The HIMovieView
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( MovieController )
HIMovieViewGetMovieController(HIViewRef inView);


/*
 *  HIMovieViewGetControllerBarSize()
 *  
 *  Summary:
 *    Returns the size of the visible movie controller bar.
 *  
 *  Parameters:
 *    
 *    inView:
 *      [in]  The HIMovieView
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( HISize )
HIMovieViewGetControllerBarSize(HIViewRef inView);


/*
 *  HIMovieViewPlay()
 *  
 *  Summary:
 *    Convenience routine to play the view's current movie.
 *  
 *  Discussion:
 *    If the movie is already playing, this function does nothing.
 *  
 *  Parameters:
 *    
 *    movieView:
 *      [in]  The movie view.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
HIMovieViewPlay(HIViewRef movieView);


/*
 *  HIMovieViewPause()
 *  
 *  Summary:
 *    Convenience routine to pause the view's current movie.
 *  
 *  Discussion:
 *    If the movie is already paused, this function does nothing.
 *  
 *  Parameters:
 *    
 *    movieView:
 *      [in]  The movie view.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
HIMovieViewPause(HIViewRef movieView);


#endif  /* TARGET_OS_MAC */


#endif // !__LP64__


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

#endif /* __HIMOVIEVIEW__ */

