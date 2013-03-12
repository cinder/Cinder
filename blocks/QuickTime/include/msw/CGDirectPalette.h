/*
     File:       CGDirectPalette.h
 
     Contains:   CoreGraphics direct palette
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2000-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef CGDIRECTPALETTE_H_
#define CGDIRECTPALETTE_H_

#ifndef __CGDIRECTDISPLAY__
#include <CGDirectDisplay.h>
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

typedef float                           CGPaletteBlendFraction;
/*
 * Convenient device color representation
 *
 * Values should be in the range from 0.0 to 1.0, where 0.0 is black, and 1.0
 * is full on for each channel.
 */
struct CGDeviceColor {
  float               red;
  float               green;
  float               blue;
};
typedef struct CGDeviceColor            CGDeviceColor;
struct CGDeviceByteColor {
  CGByteValue         red;
  CGByteValue         green;
  CGByteValue         blue;
};
typedef struct CGDeviceByteColor        CGDeviceByteColor;
/*
 * Create a new palette object representing the default 8 bit color palette.
 * Release the palette using CGPaletteRelease().
 */
/*
 *  CGPaletteCreateDefaultColorPalette()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDirectPaletteRef )
CGPaletteCreateDefaultColorPalette(void);


/*
 * Create a copy of the display's current palette, if any.
 * Returns NULL if the current display mode does not support a palette.
 * Release the palette using CGPaletteRelease().
 */
/*
 *  CGPaletteCreateWithDisplay()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDirectPaletteRef )
CGPaletteCreateWithDisplay(CGDirectDisplayID display);


/*
 * Create a new palette with a capacity as specified.  Entries are initialized from
 * the default color palette.  Release the palette using CGPaletteRelease().
 */
/*
 *  CGPaletteCreateWithCapacity()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDirectPaletteRef )
CGPaletteCreateWithCapacity(CGTableCount capacity);


/*
 * Create a new palette with a capacity and contents as specified.
 * Release the palette using CGPaletteRelease().
 */
/*
 *  CGPaletteCreateWithSamples()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDirectPaletteRef )
CGPaletteCreateWithSamples(
  CGDeviceColor *  sampleTable,
  CGTableCount     sampleCount);


/*
 * Convenience function:
 * Create a new palette with a capacity and contents as specified.
 * Release the palette using CGPaletteRelease().
 */
/*
 *  CGPaletteCreateWithByteSamples()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDirectPaletteRef )
CGPaletteCreateWithByteSamples(
  CGDeviceByteColor *  sampleTable,
  CGTableCount         sampleCount);


/*
 * Release a palette
 */
/*
 *  CGPaletteRelease()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGPaletteRelease(CGDirectPaletteRef palette);


/*
 * Get the color value at the specified index
 */
/*
 *  CGPaletteGetColorAtIndex()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDeviceColor )
CGPaletteGetColorAtIndex(
  CGDirectPaletteRef   palette,
  CGTableCount         index);


/*
 * Get the index for the specified color value
 * The index returned is for a palette color with the
 * lowest RMS error to the specified color.
 */
/*
 *  CGPaletteGetIndexForColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGTableCount )
CGPaletteGetIndexForColor(
  CGDirectPaletteRef   palette,
  CGDeviceColor        color);


/*
 * Get the number of samples in the palette
 */
/*
 *  CGPaletteGetNumberOfSamples()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGTableCount )
CGPaletteGetNumberOfSamples(CGDirectPaletteRef palette);



/*
 * Set the color value at the specified index
 */
/*
 *  CGPaletteSetColorAtIndex()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGPaletteSetColorAtIndex(
  CGDirectPaletteRef   palette,
  CGDeviceColor        color,
  CGTableCount         index);


/*
 * Copy a palette
 */
/*
 *  CGPaletteCreateCopy()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDirectPaletteRef )
CGPaletteCreateCopy(CGDirectPaletteRef palette);


/*
 * Compare two palettes
 */
/*
 *  CGPaletteIsEqualToPalette()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
CGPaletteIsEqualToPalette(
  CGDirectPaletteRef   palette1,
  CGDirectPaletteRef   palette2);


/*
 * Create a new palette blended with a fraction of a device color.
 * Free the resulting palette with CGPaletteRelease()
 */
/*
 *  CGPaletteCreateFromPaletteBlendedWithColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGDirectPaletteRef )
CGPaletteCreateFromPaletteBlendedWithColor(
  CGDirectPaletteRef       palette,
  CGPaletteBlendFraction   fraction,
  CGDeviceColor            color);



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

#endif /* CGDIRECTPALETTE_H_ */

