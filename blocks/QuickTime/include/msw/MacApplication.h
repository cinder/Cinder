/*
     File:       MacApplication.h
 
     Contains:   Application-level APIs
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2000-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __MACAPPLICATION__
#define __MACAPPLICATION__

#ifndef __CGCONTEXT__
#include <CGContext.h>
#endif

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

#ifndef __MENUS__
#include <Menus.h>
#endif

#ifndef __HIOBJECT__
#include <HIObject.h>
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


/*
 *  Summary:
 *    Controls the presentation of system-provided user interface
 *    elements.
 *  
 *  Discussion:
 *    Note that the system UI mode is a per-process state. Switching
 *    from a process that is suppressing system UI to another process
 *    that is not will cause system UI elements to become visible
 *    automatically. Switching back to the first process will suppress
 *    system UI elements again.
 */
enum {

  /*
   * In this mode, all standard system UI elements are visible.
   */
  kUIModeNormal                 = 0,

  /*
   * In this mode, system UI elements which cover the "content area" of
   * the screen (the area other than the menubar) are hidden. However,
   * these elements may automatically show themselves in response to
   * mouse movements or other user activity; specifically, the Dock
   * will still show itself automatically when the mouse moves into the
   * Dock's auto-show region.
   */
  kUIModeContentSuppressed      = 1,

  /*
   * In this mode, system UI elements which cover the "content area" of
   * the screen (the area other than the menubar) are hidden. Unlike
   * kUIModeContentSuppressed, most UI elements will not automatically
   * show themselves in this mode.
   */
  kUIModeContentHidden          = 2,

  /*
   * In this mode, all system UI elements, including the menubar, are
   * hidden. Most system UI elements will not automatically show
   * themselves in this mode. The application may request that the
   * menubar automatically show itself while in this mode by passing
   * the kUIOptionAutoShowMenuBar flag to SetSystemUIMode.
   */
  kUIModeAllHidden              = 3
};

typedef UInt32                          SystemUIMode;

/*
 *  Summary:
 *    Controls optional behavior of system-provided user interface
 *    elements.
 */
enum {

  /*
   * Requests that the menubar automatically show itself when the user
   * moves the mouse into the screen area that would ordinarily be
   * occupied by the menubar. Only valid with kUIModeAllHidden.
   */
  kUIOptionAutoShowMenuBar      = 1 << 0,

  /*
   * Disables all items in the Apple menu. Valid for all modes.
   */
  kUIOptionDisableAppleMenu     = 1 << 2,

  /*
   * The active application may not be changed while this process is
   * active. Currently disables the Command-Tab and Command-Shift-Tab
   * key sequences to switch the active process, and the global window
   * rotation key sequence selected by the user in the Keyboard
   * preference pane. SetFrontProcess may still be used to explicitly
   * switch the active process. Only valid with modes other than
   * kUIModeNormal.
   */
  kUIOptionDisableProcessSwitch = 1 << 3,

  /*
   * The Force Quit window may not be displayed while this process is
   * active. Currently disables the Command-Option-Escape key sequence
   * to open the Force Quit window and the Force Quit menu item in the
   * Apple menu. Only valid with modes other than kUIModeNormal.
   */
  kUIOptionDisableForceQuit     = 1 << 4,

  /*
   * The current login session may not be terminated while this process
   * is active. Currently disables the Power key and the Restart, Shut
   * Down, and Log Out menu items in the Apple menu. Only valid with
   * modes other than kUIModeNormal.
   */
  kUIOptionDisableSessionTerminate = 1 << 5
};

typedef OptionBits                      SystemUIOptions;
/*
 *  SetSystemUIMode()
 *  
 *  Summary:
 *    Sets the presentation mode for system-provided user interface
 *    elements.
 *  
 *  Discussion:
 *    The presentation mode of an application determines which
 *    system-provided user interface elements are visible on thes
 *    screen. When the frontmost application changes its presentation
 *    mode, a kEventAppSystemUIModeChanged Carbon event is sent to all
 *    applications that have registered for the event. This event is
 *    also sent when an application is activated; it contains the newly
 *    active application's presentation mode.
 *  
 *  Parameters:
 *    
 *    inMode:
 *      The new mode.
 *    
 *    inOptions:
 *      Options controlling how the new mode behaves.
 *  
 *  Result:
 *    An operating system result code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
SetSystemUIMode(
  SystemUIMode      inMode,
  SystemUIOptions   inOptions);


/*
 *  GetSystemUIMode()
 *  
 *  Summary:
 *    Returns the current presentation mode of the application.
 *  
 *  Parameters:
 *    
 *    outMode:
 *      On exit, contains the current mode. You may pass NULL if you
 *      don't need this information.
 *    
 *    outOptions:
 *      On exit, contains the current options for the mode. You may
 *      pass NULL if you don't need this information.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
GetSystemUIMode(
  SystemUIMode *     outMode,          /* can be NULL */
  SystemUIOptions *  outOptions);      /* can be NULL */


/*
 *  HIApplicationGetCurrent()
 *  
 *  Discussion:
 *    Returns the HIObjectRef of the currently running application
 *    object. This HIObject's EventTargetRef is what will be returned
 *    from GetApplicationEventTarget.
 *  
 *  Result:
 *    The HIObjectRef of the currently running application object.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( HIObjectRef )
HIApplicationGetCurrent(void);


/*
 *  SetApplicationDockTileImage()
 *  
 *  Discussion:
 *    Sets the image for the tile in the dock that represents your
 *    application while it is running. If you set the image, it will
 *    NOT revert back to its original image when your application
 *    terminates. You need to manually restore it before quitting.
 *  
 *  Parameters:
 *    
 *    inImage:
 *      The image you wish to have as your tile image.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
SetApplicationDockTileImage(CGImageRef inImage);


/*
 *  OverlayApplicationDockTileImage()
 *  
 *  Discussion:
 *    Takes the image passed in and composites it on top of the current
 *    image of your application's dock tile. You might do this to put a
 *    standard badge over your application's icon to indicate something
 *    to the user.
 *  
 *  Parameters:
 *    
 *    inImage:
 *      The image you wish to overlay onto your tile image.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
OverlayApplicationDockTileImage(CGImageRef inImage);


/*
 *  RestoreApplicationDockTileImage()
 *  
 *  Discussion:
 *    Restores the tile for your appliation in the dock to its normal
 *    image (your application icon). You would use this if some overlay
 *    or change of the application icon needed to be removed.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
RestoreApplicationDockTileImage(void);


/*
 *  BeginCGContextForApplicationDockTile()
 *  
 *  Discussion:
 *    Creates and returns a CGContextRef. You can use this context to
 *    draw into your application's dock tile with Quartz. You **MUST**
 *    call EndCGContextForApplicationDockTile and NOT CGEndContext when
 *    using this API, as it locks your application's tile in the dock.
 *    If you call CGEndContext, the dock will never know you are done
 *    with the tile.
 *  
 *  Result:
 *    An Quartz (Core Graphics) context reference.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGContextRef )
BeginCGContextForApplicationDockTile(void);


/*
 *  EndCGContextForApplicationDockTile()
 *  
 *  Discussion:
 *    Ends the CG context for your application tile and frees the lock
 *    on the application dock tile. You **MUST** call this routine and
 *    NOT CGEndContext when using BeginCGContextForApplicationDockTile,
 *    as it locks your application's tile in the dock. If you call
 *    CGEndContext, the dock will never know you are done with the tile.
 *  
 *  Parameters:
 *    
 *    inContext:
 *      The context to end. The context is invalid after this call and
 *      should no longer be used.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
EndCGContextForApplicationDockTile(CGContextRef inContext);




/*
 *  BeginQDContextForApplicationDockTile()
 *  
 *  Discussion:
 *    Creates and returns a CGrafPtr for your application's tile in the
 *    dock. You can use this port to draw into your application's dock
 *    tile with Quickdraw. You **MUST** call
 *    EndQDContextForApplicationDockTile and NOT DisposePort when using
 *    this API, as it locks your application's tile in the dock. If you
 *    call DisposePort, the dock will never know you are done with the
 *    tile.
 *  
 *  Result:
 *    A Quickdraw port reference.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGrafPtr )
BeginQDContextForApplicationDockTile(void);


/*
 *  EndQDContextForApplicationDockTile()
 *  
 *  Discussion:
 *    Disposes the Quickdraw port for your application tile and frees
 *    the lock on the application dock tile. You **MUST** call this
 *    routine and NOT DisposePort when using
 *    BeginQDContextForApplicationDockTile, else the dock will never
 *    know you are done with the tile.
 *  
 *  Parameters:
 *    
 *    inContext:
 *      The context to end. The context is invalid after this call and
 *      should no longer be used.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
EndQDContextForApplicationDockTile(CGrafPtr inContext);



/*
 *  SetApplicationDockTileMenu()
 *  
 *  Summary:
 *    Sets the menu that is displayed by the application's dock tile.
 *  
 *  Discussion:
 *    The Carbon Window Manager and the Dock will always automatically
 *    display a menu containing a list of the application's document
 *    windows. If the application wants to add other additional menu
 *    items, it can use the SetApplicationDockTileMenu API to provide
 *    those items. The items in the specified menu will be combined
 *    with the window title items. This API increments the refcount of
 *    the specified menu. Before the menu is actually displayed, it
 *    will receive kEventMenuPopulate, kEventMenuOpening, and
 *    kEventMenuEnableItems Carbon events, so any event handlers for
 *    these events may update the menu appropriately for the current
 *    state of the application. The application should set a command ID
 *    for each menu item in the dock tile menu, and when that item is
 *    chosen, a kEventCommandProcess Carbon event containing the item's
 *    command ID will be sent to the user focus target.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu to display, or NULL to remove the current dock tile
 *      menu.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( OSStatus )
SetApplicationDockTileMenu(MenuRef inMenu);


/*
 *  GetApplicationDockTileMenu()
 *  
 *  Summary:
 *    Returns the menu that is displayed by the application's dock tile.
 *  
 *  Result:
 *    The application's dock tile menu, or NULL if none.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( MenuRef )
GetApplicationDockTileMenu(void);



/*
 *  CreateCGImageFromPixMaps()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
CreateCGImageFromPixMaps(
  PixMapHandle   inImage,
  PixMapHandle   inMask,
  CGImageRef *   outImage);


/*
 *  GetApplicationTextEncoding()
 *  
 *  Summary:
 *    Returns the application's primary text encoding.
 *  
 *  Discussion:
 *    The application text encoding is used when you create a
 *    CFStringRef from text stored in Resource Manager resources, which
 *    typically uses one of the Mac encodings such as MacRoman or
 *    MacJapanese. The encoding is determined by: (a) if your app is
 *    bundled, the encoding of the .lproj directory chosen by CFBundle,
 *    (b) else if your plist has a CFBundleDevelopmentRegionKey, the
 *    encoding specified by that key, (c) else if your app has a 'vers'
 *    resource, the encoding for the region field in the 'vers', (d)
 *    else the current localization of the operating system.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( TextEncoding )
GetApplicationTextEncoding(void);


/*
 *  GetApplicationScript()
 *  
 *  Summary:
 *    Returns the application script.
 *  
 *  Discussion:
 *    The application script is used when you need a ScriptCode to pass
 *    to some other API, such as UseThemeFont.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( ScriptCode )
GetApplicationScript(void);




#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* __MACAPPLICATION__ */

