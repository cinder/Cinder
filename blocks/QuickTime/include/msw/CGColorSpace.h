/*
     File:       CGColorSpace.h
 
     Contains:   CoreGraphics color space
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2000-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/

#ifndef CGCOLORSPACE_H_
#define CGCOLORSPACE_H_

#ifndef __CGBASE__
#include <CGBase.h>
#endif

#ifndef __CGDATAPROVIDER__
#include <CGDataProvider.h>
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

#if PRAGMA_ENUM_ALWAYSINT
    #if defined(__fourbyteints__) && !__fourbyteints__ 
        #define __CGCOLORSPACE__RESTORE_TWOBYTEINTS
        #pragma fourbyteints on
    #endif
    #pragma enumsalwaysint on
#elif PRAGMA_ENUM_OPTIONS
    #pragma option enum=int
#elif PRAGMA_ENUM_PACK
    #if __option(pack_enums)
        #define __CGCOLORSPACE__RESTORE_PACKED_ENUMS
        #pragma options(!pack_enums)
    #endif
#endif

typedef struct CGColorSpace*            CGColorSpaceRef;
enum CGColorRenderingIntent {
  kCGRenderingIntentDefault     = 0,
  kCGRenderingIntentAbsoluteColorimetric = 1,
  kCGRenderingIntentRelativeColorimetric = 2,
  kCGRenderingIntentPerceptual  = 3,
  kCGRenderingIntentSaturation  = 4
};
typedef enum CGColorRenderingIntent CGColorRenderingIntent;


/** Device-dependent color spaces.  **/
/* Create a DeviceGray colorspace. */
/*
 *  CGColorSpaceCreateDeviceGray()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGColorSpaceRef )
CGColorSpaceCreateDeviceGray(void);


/* Create a DeviceRGB colorspace. */
/*
 *  CGColorSpaceCreateDeviceRGB()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGColorSpaceRef )
CGColorSpaceCreateDeviceRGB(void);


/* Create a DeviceCMYK colorspace. */
/*
 *  CGColorSpaceCreateDeviceCMYK()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGColorSpaceRef )
CGColorSpaceCreateDeviceCMYK(void);


/** Device-independent color spaces. **/
/* Create a calibrated gray colorspace.  `whitePoint' is an array of 3
 * numbers specifying the tristimulus value, in the CIE 1931 XYZ-space, of
 * the diffuse white point.  `blackPoint' is an array of 3 numbers
 * specifying the tristimulus value, in CIE 1931 XYZ-space, of the diffuse
 * black point. `gamma' defines the gamma for the gray component. */
/*
 *  CGColorSpaceCreateCalibratedGray()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGColorSpaceRef )
CGColorSpaceCreateCalibratedGray(
  const float   whitePoint[3],
  const float   blackPoint[3],
  float         gamma);


/* Create a calibrated RGB colorspace.  `whitePoint' is an array of 3
 * numbers specifying the tristimulus value, in the CIE 1931 XYZ-space, of
 * the diffuse white point.  `blackPoint' is an array of 3 numbers
 * specifying the tristimulus value, in CIE 1931 XYZ-space, of the diffuse
 * black point. `gamma' is an array of 3 numbers specifying the gamma for
 * the red, green, and blue components of the color space. `matrix' is an
 * array of 9 numbers specifying the linear interpretation of the
 * gamma-modified RGB values of the colorspace with respect to the final
 * XYZ representation. */
/*
 *  CGColorSpaceCreateCalibratedRGB()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGColorSpaceRef )
CGColorSpaceCreateCalibratedRGB(
  const float   whitePoint[3],
  const float   blackPoint[3],
  const float   gamma[3],
  const float   matrix[9]);


/* Create an L*a*b* colorspace.  `whitePoint' is an array of 3 numbers
 * specifying the tristimulus value, in the CIE 1931 XYZ-space, of the
 * diffuse white point.  `blackPoint' is an array of 3 numbers specifying
 * the tristimulus value, in CIE 1931 XYZ-space, of the diffuse black
 * point. `range' is an array of four numbers specifying the range of valid
 * values for the a* and b* components of the color space. */
/*
 *  CGColorSpaceCreateLab()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGColorSpaceRef )
CGColorSpaceCreateLab(
  const float   whitePoint[3],
  const float   blackPoint[3],
  const float   range[4]);


/* Create an ICC-based colorspace.  `nComponents' specifies the number of
 * color components in the color space defined by the ICC profile data.
 * This must match the number of components actually in the ICC profile,
 * and must be 1, 3, or 4.  `range' is an array of 2*nComponents numbers
 * specifying the minimum and maximum valid values of the corresponding
 * color components, so that for color component k, range[2*k] <= c[k] <=
 * range[2*k+1], where c[k] is the k'th color component.  `profile' is a
 * data provider specifying the ICC profile.  `alternate' specifies an
 * alternate colorspace to be used in case the ICC profile is not
 * supported.  It must have `nComponents' color components. If `alternate'
 * is NULL, then the color space used will be DeviceGray, DeviceRGB, or
 * DeviceCMYK, depending on whether `nComponents' is 1, 3, or 4,
 * respectively. */
/*
 *  CGColorSpaceCreateICCBased()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGColorSpaceRef )
CGColorSpaceCreateICCBased(
  size_t              nComponents,
  const float *       range,
  CGDataProviderRef   profile,
  CGColorSpaceRef     alternateSpace);


/** Special colorspaces. **/
/* Create an indexed colorspace.  A sample value in an indexed color space
 * is treated as an index into the color table of the color space.  `base'
 * specifies the base color space in which the values in the color table
 * are to be interpreted. `lastIndex' is an integer which specifies the
 * maximum valid index value; it must be less than or equal to 255.
 * `colorTable' is an array of m * (lastIndex + 1) bytes, where m is
 * the number of color components in the base color space.  Each byte
 * is an unsigned integer in the range 0 to 255 that is scaled to the
 * range of the corresponding color component in the base color space. */
/*
 *  CGColorSpaceCreateIndexed()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGColorSpaceRef )
CGColorSpaceCreateIndexed(
  CGColorSpaceRef        baseSpace,
  size_t                 lastIndex,
  const unsigned char *  colorTable);


/* Create a pattern colorspace. `baseSpace' is the underlying colorspace of
 * the pattern colorspace.  For colored patterns, `baseSpace' should be
 * NULL; for uncolored patterns, `baseSpace' specifies the colorspace of
 * colors which will be painted through the pattern. */
/*
 *  CGColorSpaceCreatePattern()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( CGColorSpaceRef )
CGColorSpaceCreatePattern(CGColorSpaceRef baseSpace);


/* Create a CGColorSpace using `platformColorSpaceReference', a pointer to
 * a platform-specific color space reference. For MacOS X,
 * `platformColorSpaceReference' should be a pointer to a CMProfileRef. */
/*
 *  CGColorSpaceCreateWithPlatformColorSpace()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( CGColorSpaceRef )
CGColorSpaceCreateWithPlatformColorSpace(void * platformColorSpaceReference);



/** Colorspace information. **/
/* Return the number of color components supported by the colorspace `cs'. */
/*
 *  CGColorSpaceGetNumberOfComponents()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( size_t )
CGColorSpaceGetNumberOfComponents(CGColorSpaceRef cs);


/** Retaining & releasing colorspaces. **/
/* Increment the retain count of `cs' and return it.  All colorspaces are
 * created with an initial retain count of 1. */
/*
 *  CGColorSpaceRetain()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGColorSpaceRef )
CGColorSpaceRetain(CGColorSpaceRef cs);


/* Decrement the retain count of `cs'.  If the retain count reaches 0, then
 * release it and any associated resources. */
/*
 *  CGColorSpaceRelease()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGColorSpaceRelease(CGColorSpaceRef cs);



#if PRAGMA_ENUM_ALWAYSINT
    #pragma enumsalwaysint reset
    #ifdef __CGCOLORSPACE__RESTORE_TWOBYTEINTS
        #pragma fourbyteints off
    #endif
#elif PRAGMA_ENUM_OPTIONS
    #pragma option enum=reset
#elif defined(__CGCOLORSPACE__RESTORE_PACKED_ENUMS)
    #pragma options(pack_enums)
#endif

#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* CGCOLORSPACE_H_ */

