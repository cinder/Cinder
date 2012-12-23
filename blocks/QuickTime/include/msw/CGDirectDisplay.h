/*
     File:       CGDirectDisplay.h
 
     Contains:   CoreGraphics direct display
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2000-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef CGDIRECTDISPLAY_H_
#define CGDIRECTDISPLAY_H_

#ifndef __CGBASE__
#include <CGBase.h>
#endif

#ifndef __CGGEOMETRY__
#include <CGGeometry.h>
#endif

#ifndef __CGERROR__
#include <CGError.h>
#endif

#ifndef __CFARRAY__
#include <CFArray.h>
#endif

#ifndef __CFSTRING__
#include <CFString.h>
#endif

#ifndef __CFDICTIONARY__
#include <CFDictionary.h>
#endif

#include <stdint.h>


#if PRAGMA_ONCE
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if PRAGMA_IMPORT
#pragma import on
#endif

typedef struct _CGDirectDisplayID*      CGDirectDisplayID;
typedef struct _CGDirectPaletteRef*     CGDirectPaletteRef;
typedef uint32_t                        CGDisplayCount;
typedef uint32_t                        CGTableCount;
typedef int32_t                         CGDisplayCoord;
typedef uint8_t                         CGByteValue;
typedef uint32_t                        CGOpenGLDisplayMask;
typedef uint32_t                        CGBeamPosition;
typedef int32_t                         CGMouseDelta;
typedef double                          CGRefreshRate;
typedef CGError                         CGDisplayErr;
enum {
  CGDisplayNoErr                = kCGErrorSuccess
};

/* A NULL value points to the main display device as a programming convention */
#define kCGDirectMainDisplay            ((CGDirectDisplayID)NULL)

/*
 * Mechanisms used to find screen IDs
 * An array length (maxDisplays) and array of CGDirectDisplayIDs are passed in.
 * Up to maxDisplays of the array are filled in with the displays meeting the
 * specified criteria.  The actual number of displays filled in is returned in
 * dspyCnt.
 *
 * If the dspys array is NULL, maxDisplays is ignored, and *dspyCnt is filled
 * in with the number of displays meeting the function's requirements.
 */
/*
 *  CGGetDisplaysWithPoint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDisplayErr )
CGGetDisplaysWithPoint(
  CGPoint              point,
  CGDisplayCount       maxDisplays,
  CGDirectDisplayID *  dspys,
  CGDisplayCount *     dspyCnt);


/*
 *  CGGetDisplaysWithRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDisplayErr )
CGGetDisplaysWithRect(
  CGRect               rect,
  CGDisplayCount       maxDisplays,
  CGDirectDisplayID *  dspys,
  CGDisplayCount *     dspyCnt);


/*
 *  CGGetDisplaysWithOpenGLDisplayMask()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDisplayErr )
CGGetDisplaysWithOpenGLDisplayMask(
  CGOpenGLDisplayMask   mask,
  CGDisplayCount        maxDisplays,
  CGDirectDisplayID *   dspys,
  CGDisplayCount *      dspyCnt);


/*
 * Get lists of displays.  Use this to determine display IDs
 *
 * If the activeDspys array is NULL, maxDisplays is ignored, and *dspyCnt is filled
 * in with the number of displays meeting the function's requirements.
 *
 * The first display returned in the list is the main display,
 * the one with the menu bar.
 * When mirroring, this will be the largest display,
 * or if all are the same size, the one with the deepest pixel depth.
 */
/*
 *  CGGetActiveDisplayList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDisplayErr )
CGGetActiveDisplayList(
  CGDisplayCount       maxDisplays,
  CGDirectDisplayID *  activeDspys,
  CGDisplayCount *     dspyCnt);


/* Map a display to an OpenGL display mask; returns 0 on invalid display */
/*
 *  CGDisplayIDToOpenGLDisplayMask()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGOpenGLDisplayMask )
CGDisplayIDToOpenGLDisplayMask(CGDirectDisplayID display);


/* Return screen size and origin in global coords; Empty rect if display is invalid */
/*
 *  CGDisplayBounds()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGRect )
CGDisplayBounds(CGDirectDisplayID display);


/*
 *  CGDisplayPixelsWide()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( size_t )
CGDisplayPixelsWide(CGDirectDisplayID display);


/*
 *  CGDisplayPixelsHigh()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( size_t )
CGDisplayPixelsHigh(CGDirectDisplayID display);


/*
 * Display mode selection
 * Display modes are represented as CFDictionaries
 * All dictionaries and arrays returned via these mechanisms are
 * owned by the framework and should not be released.  The framework
 * will not release them out from under your application.
 *
 * Values associated with the following keys are CFNumber types.
 * With CFNumberGetValue(), use kCFNumberLongType for best results.
 */
/*
 * Keys used in mode dictionaries.  Source C strings shown won't change.
 * Some CFM environments cannot import data variables, and so
 * duplicate these CFStringRefs locally.
 */
/*
 *  kCGDisplayWidth
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
extern CFStringRef kCGDisplayWidth;
/*
 *  kCGDisplayHeight
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
extern CFStringRef kCGDisplayHeight;
/*
 *  kCGDisplayMode
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
extern CFStringRef kCGDisplayMode;
/*
 *  kCGDisplayBitsPerPixel
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
extern CFStringRef kCGDisplayBitsPerPixel;
/*
 *  kCGDisplayBitsPerSample
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
extern CFStringRef kCGDisplayBitsPerSample;
/*
 *  kCGDisplaySamplesPerPixel
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
extern CFStringRef kCGDisplaySamplesPerPixel;
/*
 *  kCGDisplayRefreshRate
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
extern CFStringRef kCGDisplayRefreshRate;
/*
 *  kCGDisplayModeUsableForDesktopGUI
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
extern CFStringRef kCGDisplayModeUsableForDesktopGUI;
/*
 *  kCGDisplayIOFlags
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
extern CFStringRef kCGDisplayIOFlags;
/*
 * Return a CFArray of CFDictionaries describing all display modes.
 * Returns NULL if the display is invalid.
 */
/*
 *  CGDisplayAvailableModes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CFArrayRef )
CGDisplayAvailableModes(CGDirectDisplayID display);


/*
 * Try to find a display mode of specified depth with dimensions equal or greater than
 * specified.
 * If no depth match is found, try for the next larger depth with dimensions equal or greater
 * than specified.  If no luck, then just return the current mode.
 *
 * exactmatch, if not NULL, is set to 'true' if an exact match in width, height, and depth is found,
 * and 'false' otherwise.
 * Returns NULL if display is invalid.
 */
/*
 *  CGDisplayBestModeForParameters()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CFDictionaryRef )
CGDisplayBestModeForParameters(
  CGDirectDisplayID   display,
  size_t              bitsPerPixel,
  size_t              width,
  size_t              height,
  boolean_t *         exactMatch);


/*
 *  CGDisplayBestModeForParametersAndRefreshRate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CFDictionaryRef )
CGDisplayBestModeForParametersAndRefreshRate(
  CGDirectDisplayID   display,
  size_t              bitsPerPixel,
  size_t              width,
  size_t              height,
  CGRefreshRate       refresh,
  boolean_t *         exactMatch);


/*
 * Return a CFDictionary describing the current display mode.
 * Returns NULL if display is invalid.
 */
/*
 *  CGDisplayCurrentMode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CFDictionaryRef )
CGDisplayCurrentMode(CGDirectDisplayID display);


/*
 * Switch display mode.  Note that after switching, 
 * display parameters and addresses may change.
 * The selected display mode persists for the life of the program, and automatically
 * reverts to the permanent setting made by Preferences when the program terminates.
 * The mode dictionary passed in must be a dictionary vended by other CGDirectDisplay
 * APIs such as CGDisplayBestModeForParameters() and CGDisplayAvailableModes().
 */
/*
 *  CGDisplaySwitchToMode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDisplayErr )
CGDisplaySwitchToMode(
  CGDirectDisplayID   display,
  CFDictionaryRef     mode);


/* Query parameters for current mode */
/*
 *  CGDisplayBitsPerPixel()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( size_t )
CGDisplayBitsPerPixel(CGDirectDisplayID display);


/*
 *  CGDisplayBitsPerSample()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( size_t )
CGDisplayBitsPerSample(CGDirectDisplayID display);


/*
 *  CGDisplaySamplesPerPixel()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( size_t )
CGDisplaySamplesPerPixel(CGDirectDisplayID display);


/*
 *  CGDisplayBytesPerRow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( size_t )
CGDisplayBytesPerRow(CGDirectDisplayID display);


/*
 * Set a display gamma/transfer function from a formula specifying
 * min and max values and a gamma for each channel.
 * Gamma values must be greater than 0.0.
 * To get an antigamma of 1.6, one would specify a value of (1.0 / 1.6)
 * Min values must be greater than or equal to 0.0 and less than 1.0.
 * Max values must be greater than 0.0 and less than or equal to 1.0.
 * Out of range values, or Max greater than or equal to Min result
 * in a kCGSRangeCheck error.
 *
 * Values are computed by sampling a function for a range of indices from 0 through 1:
 *  value = Min + ((Max - Min) * pow(index, Gamma))
 * The resulting values are converted to a machine specific format
 * and loaded into hardware.
 */
typedef float                           CGGammaValue;
/*
 *  CGSetDisplayTransferByFormula()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDisplayErr )
CGSetDisplayTransferByFormula(
  CGDirectDisplayID   display,
  CGGammaValue        redMin,
  CGGammaValue        redMax,
  CGGammaValue        redGamma,
  CGGammaValue        greenMin,
  CGGammaValue        greenMax,
  CGGammaValue        greenGamma,
  CGGammaValue        blueMin,
  CGGammaValue        blueMax,
  CGGammaValue        blueGamma);


/*
 *  CGGetDisplayTransferByFormula()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDisplayErr )
CGGetDisplayTransferByFormula(
  CGDirectDisplayID   display,
  CGGammaValue *      redMin,
  CGGammaValue *      redMax,
  CGGammaValue *      redGamma,
  CGGammaValue *      greenMin,
  CGGammaValue *      greenMax,
  CGGammaValue *      greenGamma,
  CGGammaValue *      blueMin,
  CGGammaValue *      blueMax,
  CGGammaValue *      blueGamma);


/*
 * Set a display gamma/transfer function using tables of data for each channel.
 * Values within each table should have values in the range of 0.0 through 1.0.
 * The same table may be passed in for red, green, and blue channels. 'tableSize'
 * indicates the number of entries in each table.
 * The tables are interpolated as needed to generate the number of samples needed
 * by hardware.
 */
/*
 *  CGSetDisplayTransferByTable()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDisplayErr )
CGSetDisplayTransferByTable(
  CGDirectDisplayID     display,
  CGTableCount          tableSize,
  const CGGammaValue *  redTable,
  const CGGammaValue *  greenTable,
  const CGGammaValue *  blueTable);


/*
 * Get transfer tables.  Capacity should contain the number of samples each
 * array can hold, and *sampleCount is filled in with the number of samples
 * actually copied in.
 */
/*
 *  CGGetDisplayTransferByTable()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDisplayErr )
CGGetDisplayTransferByTable(
  CGDirectDisplayID   display,
  CGTableCount        capacity,
  CGGammaValue *      redTable,
  CGGammaValue *      greenTable,
  CGGammaValue *      blueTable,
  CGTableCount *      sampleCount);


/* As a convenience, allow setting of the gamma table by byte values */
/*
 *  CGSetDisplayTransferByByteTable()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDisplayErr )
CGSetDisplayTransferByByteTable(
  CGDirectDisplayID    display,
  CGTableCount         tableSize,
  const CGByteValue *  redTable,
  const CGByteValue *  greenTable,
  const CGByteValue *  blueTable);


/* Restore gamma tables of system displays to the user's ColorSync specified values */
/*
 *  CGDisplayRestoreColorSyncSettings()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGDisplayRestoreColorSyncSettings(void);


/* Display capture and release */
/*
 *  CGDisplayIsCaptured()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( boolean_t )
CGDisplayIsCaptured(CGDirectDisplayID display);


/*
 *  CGDisplayCapture()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDisplayErr )
CGDisplayCapture(CGDirectDisplayID display);


/*
 *  CGDisplayRelease()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDisplayErr )
CGDisplayRelease(CGDirectDisplayID display);



/*
 * Capture all displays; this has the nice effect of providing an immersive
 * environment, and preventing other apps from trying to adjust themselves
 * to display changes only needed by your app.
 */
/*
 *  CGCaptureAllDisplays()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDisplayErr )
CGCaptureAllDisplays(void);


/*
 * Release all captured displays, and restore the display modes to the
 * user's preferences.  May be used in conjunction with CGDisplayCapture()
 * or CGCaptureAllDisplays().
 */
/*
 *  CGReleaseAllDisplays()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDisplayErr )
CGReleaseAllDisplays(void);



/*
 * Returns CoreGraphics raw shield window ID or NULL if not shielded
 * This value may be used with drawing surface APIs.
 */
/*
 *  CGShieldingWindowID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void * )
CGShieldingWindowID(CGDirectDisplayID display);


/*
 * Returns the window level used for the shield window.
 * This value may be used with Cocoa windows to position the
 * Cocoa window in the same window level as the shield window.
 */
/*
 *  CGShieldingWindowLevel()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( int32_t )
CGShieldingWindowLevel(void);


/*
 * Returns base address of display or NULL for an invalid display.
 * If the display has not been captured, the returned address may refer
 * to read-only memory.
 */
/*
 *  CGDisplayBaseAddress()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void * )
CGDisplayBaseAddress(CGDirectDisplayID display);


/*
 * return address for X,Y in screen coordinates;
 *  (0,0) represents the upper left corner of the display.
 * returns NULL for an invalid display or out of bounds coordinates
 * If the display has not been captured, the returned address may refer
 * to read-only memory.
 */
/*
 *  CGDisplayAddressForPosition()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void * )
CGDisplayAddressForPosition(
  CGDirectDisplayID   display,
  CGDisplayCoord      x,
  CGDisplayCoord      y);



/* Mouse Cursor controls */
/*
 *  CGDisplayHideCursor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDisplayErr )
CGDisplayHideCursor(CGDirectDisplayID display);


/* increments hide cursor count */
/*
 *  CGDisplayShowCursor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDisplayErr )
CGDisplayShowCursor(CGDirectDisplayID display);


/* decrements hide cursor count  */
/*
 * Move the cursor to the specified point relative to the display origin
 * (the upper left corner of the display).  Returns CGDisplayNoErr on success.
 * No events are generated as a result of this move.
 * Points that would lie outside the desktop are clipped to the desktop.
 */
/*
 *  CGDisplayMoveCursorToPoint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDisplayErr )
CGDisplayMoveCursorToPoint(
  CGDirectDisplayID   display,
  CGPoint             point);


/*
 * Report the mouse position change associated with the last mouse move event
 * recieved by this application.
 */
/*
 *  CGGetLastMouseDelta()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGGetLastMouseDelta(
  CGMouseDelta *  deltaX,
  CGMouseDelta *  deltaY);



/* Palette controls (8 bit pseudocolor only) */
/*
 * Returns TRUE if the current display mode supports palettes
 */
/*
 *  CGDisplayCanSetPalette()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( boolean_t )
CGDisplayCanSetPalette(CGDirectDisplayID display);


/*
 * Set a palette.  The current gamma function is applied to the palette
 * elements before being loaded into hardware.
 */
/*
 *  CGDisplaySetPalette()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDisplayErr )
CGDisplaySetPalette(
  CGDirectDisplayID    display,
  CGDirectPaletteRef   palette);


/*
 * Wait until the beam position is outside the range specified by upperScanLine and lowerScanLine.
 * Note that if upperScanLine and lowerScanLine encompass the entire display height,
 * the function returns an error.
 * lowerScanLine must be greater than or equal to upperScanLine.
 *
 * Some display systems may not conventional video vertical and horizontal sweep in painting.
 * These displays report a kCGDisplayRefreshRate of 0 in the CFDictionaryRef returned by
 * CGDisplayCurrentMode().  On such displays, this function returns at once.
 *
 * Some drivers may not implement support for this mechanism.
 * On such displays, this function returns at once.
 *
 * Returns CGDisplayNoErr on success, and an error if display or upperScanLine and
 * lowerScanLine are invalid.
 *
 * The app should set the values of upperScanLine and lowerScanLine to allow enough lead time
 * for the drawing operation to complete.  A common strategy is to wait for the beam to pass
 * the bottom of the drawing area, allowing almost a full vertical sweep period to perform drawing.
 * To do this, set upperScanLine to 0, and set lowerScanLine to the bottom of the bounding box:
 *  lowerScanLine = (CGBeamPosition)(cgrect.origin.y + cgrect.size.height);
 *
 * IOKit may implement this as a spin-loop on the beam position call used for CGDisplayBeamPosition().
 * On such system the function is CPU bound, and subject to all the usual scheduling pre-emption.
 * In particular, attempting to wait for the beam to hit a specific scanline may be an exercise in frustration.
 *
 * These functions are advisary in nature, and depend on IOKit and hardware specific drivers to implement
 * support. If you need extremely precise timing, or access to vertical blanking interrupts,
 * you should consider writing a device driver to tie into hardware-specific capabilities.
 */
/*
 *  CGDisplayWaitForBeamPositionOutsideLines()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDisplayErr )
CGDisplayWaitForBeamPositionOutsideLines(
  CGDirectDisplayID   display,
  CGBeamPosition      upperScanLine,
  CGBeamPosition      lowerScanLine);


/*
 * Returns the current beam position on the display.  If display is invalid,
 * or the display does not implement conventional video vertical and horizontal
 * sweep in painting, or the driver does not implement this functionality, 0 is returned.
 */
/*
 *  CGDisplayBeamPosition()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGBeamPosition )
CGDisplayBeamPosition(CGDirectDisplayID display);



#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* CGDIRECTDISPLAY_H_ */

