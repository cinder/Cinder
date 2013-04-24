/*
 *  CVImageBuffer.h
 *  CoreVideo
 *
 *  Copyright (c) 2004 Apple Computer, Inc. All rights reserved.
 *
 */
 
 /*! @header CVImageBuffer.h
	@copyright 2004 Apple Computer, Inc. All rights reserved.
	@availability Mac OS X 10.4 or later
    @discussion CVImageBufferRef types are abstract and define various attachments and convenience
		calls for retreiving image related bits of data.
		   
*/

#if !defined(__COREVIDEO_CVIMAGEBUFFER_H__)
#define __COREVIDEO_CVIMAGEBUFFER_H__ 1

#include <TargetConditionals.h>

#if TARGET_OS_MAC
#include <ApplicationServices/ApplicationServices.h>
#include <QuartzCore/CVBuffer.h>
#else
#pragma warning (disable: 4068)		// ignore unknown pragmas
#include <CVBuffer.h>
#include <CGGeometry.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#pragma mark CVImageBufferRef attachment keys

#if TARGET_OS_MAC
CV_EXPORT const CFStringRef kCVImageBufferCGColorSpaceKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;				// CGColorSpaceRef

CV_EXPORT const CFStringRef kCVImageBufferCleanApertureKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;				// CFDictionary containing the following four keys
CV_EXPORT const CFStringRef	kCVImageBufferCleanApertureWidthKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;			// CFNumber
CV_EXPORT const CFStringRef	kCVImageBufferCleanApertureHeightKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;			// CFNumber
CV_EXPORT const CFStringRef	kCVImageBufferCleanApertureHorizontalOffsetKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;		// CFNumber
CV_EXPORT const CFStringRef	kCVImageBufferCleanApertureVerticalOffsetKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;		// CFNumber
CV_EXPORT const CFStringRef kCVImageBufferPreferredCleanApertureKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;			// CFDictionary containing same keys as kCVImageBufferCleanApertureKey

CV_EXPORT const CFStringRef kCVImageBufferFieldCountKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;				// CFNumber
CV_EXPORT const CFStringRef kCVImageBufferFieldDetailKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;				// CFString with one of the following four values
CV_EXPORT const CFStringRef	kCVImageBufferFieldDetailTemporalTopFirst AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;		// CFString
CV_EXPORT const CFStringRef	kCVImageBufferFieldDetailTemporalBottomFirst AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;		// CFString
CV_EXPORT const CFStringRef	kCVImageBufferFieldDetailSpatialFirstLineEarly AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;		// CFString
CV_EXPORT const CFStringRef	kCVImageBufferFieldDetailSpatialFirstLineLate AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;		// CFString

CV_EXPORT const CFStringRef kCVImageBufferPixelAspectRatioKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;				// CFDictionary with the following two keys
CV_EXPORT const CFStringRef	kCVImageBufferPixelAspectRatioHorizontalSpacingKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;	// CFNumber
CV_EXPORT const CFStringRef	kCVImageBufferPixelAspectRatioVerticalSpacingKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;	// CFNumber

CV_EXPORT const CFStringRef kCVImageBufferDisplayDimensionsKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;				// CFDictionary with the following two keys
CV_EXPORT const CFStringRef	kCVImageBufferDisplayWidthKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;				// CFNumber
CV_EXPORT const CFStringRef	kCVImageBufferDisplayHeightKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;				// CFNumber

CV_EXPORT const CFStringRef kCVImageBufferGammaLevelKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;				// CFNumber describing the gamma level
CV_EXPORT const CFStringRef kCVImageBufferYCbCrMatrixKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;				// CFString describing the color matrix for YCbCr->RGB. This key can be one of the following values:
CV_EXPORT const CFStringRef	kCVImageBufferYCbCrMatrix_ITU_R_709_2 AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;			// CFString
CV_EXPORT const CFStringRef	kCVImageBufferYCbCrMatrix_ITU_R_601_4 AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;			// CFString
CV_EXPORT const CFStringRef	kCVImageBufferYCbCrMatrix_SMPTE_240M_1995 AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;		// CFString

#else
#define kCVImageBufferCGColorSpaceKey			CFSTR("CGColorSpace")

#define kCVImageBufferCleanApertureKey			CFSTR("CVCleanAperture")
#define kCVImageBufferCleanApertureWidthKey		    CFSTR("Width")
#define kCVImageBufferCleanApertureHeightKey		    CFSTR("Height")
#define kCVImageBufferCleanApertureHorizontalOffsetKey	    CFSTR("HorizontalOffset")
#define kCVImageBufferCleanApertureVerticalOffsetKey	    CFSTR("VerticalOffset")
#define kCVImageBufferPreferredCleanApertureKey		CFSTR("CVPreferredCleanAperture")

#define kCVImageBufferFieldCountKey			CFSTR("CVFieldCount")
#define kCVImageBufferFieldDetailKey			CFSTR("CVFieldDetail")
#define kCVImageBufferFieldDetailTemporalTopFirst	    CFSTR("TemporalTopFirst")
#define kCVImageBufferFieldDetailTemporalBottomFirst	    CFSTR("TemporalBottomFirst")
#define kCVImageBufferFieldDetailSpatialFirstLineEarly	    CFSTR("SpatialFirstLineEarly")
#define kCVImageBufferFieldDetailSpatialFirstLineLate	    CFSTR("SpatialFirstLineLate")

#define kCVImageBufferPixelAspectRatioKey		CFSTR("CVPixelAspectRatio")
#define kCVImageBufferPixelAspectRatioHorizontalSpacingKey	CFSTR("HorizontalSpacing")
#define kCVImageBufferPixelAspectRatioVerticalSpacingKey	CFSTR("VerticalSpacing")

#define kCVImageBufferDisplayDimensionsKey		CFSTR("CVDisplayDimensions")
#define kCVImageBufferDisplayWidthKey			    CFSTR("Width")
#define kCVImageBufferDisplayHeightKey			    CFSTR("Height")


#define kCVImageBufferGammaLevelKey			CFSTR("CVImageBufferGammaLevel")
#define kCVImageBufferYCbCrMatrixKey			CFSTR("CVImageBufferYCbCrMatrix")
#define kCVImageBufferYCbCrMatrix_ITU_R_709_2			CFSTR("CVImageBufferYCbCrMatrix_ITU_R_709_2")
#define kCVImageBufferYCbCrMatrix_ITU_R_601_4			CFSTR("CVImageBufferYCbCrMatrix_ITU_R_601_4")
#define kCVImageBufferYCbCrMatrix_SMPTE_240M_1995		CFSTR("CVImageBufferYCbCrMatrix_SMPTE_240M_1995")

#endif

#pragma mark CVImageBufferRef

/*!
    @typedef	CVImageBufferRef
    @abstract   Base type for all CoreVideo image buffers

*/
typedef CVBufferRef CVImageBufferRef;

/*!
    @function   CVImageBufferGetEncodedSize
    @abstract   Returns the full encoded dimensions of a CVImageBuffer.  For example, for an NTSC DV frame this would be 720x480
    @discussion Note: When creating a CIImage from a CVImageBuffer, this is the call you should use for retrieving the image size.
    @param      imageBuffer A CVImageBuffer that you wish to retrieve the encoded size from.
    @result     A CGSize returning the full encoded size of the buffer
		Returns zero size if called with a non-CVImageBufferRef type or NULL.
*/
CV_EXPORT CGSize CVImageBufferGetEncodedSize(CVImageBufferRef imageBuffer) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVImageBufferGetDisplaySize
    @abstract   Returns the nominal output display size (in square pixels) of a CVImageBuffer.  
                For example, for an NTSC DV frame this would be 640x480
    @param      imageBuffer A CVImageBuffer that you wish to retrieve the display size from.
    @result     A CGSize returning the nominal display size of the buffer
		Returns zero size if called with a non-CVImageBufferRef type or NULL.
*/
CV_EXPORT CGSize CVImageBufferGetDisplaySize(CVImageBufferRef imageBuffer) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVImageBufferGetCleanRect
    @abstract   Returns the source rectangle of a CVImageBuffer that represents the clean aperture
		of the buffer in encoded pixels.    For example, an NTSC DV frame would return a CGRect with an
		origin of 8,0 and a size of 704,480.		
		Note that the origin of this rect always the lower left	corner.   This is the same coordinate system as
		used by CoreImage.
    @param      imageBuffer A CVImageBuffer that you wish to retrieve the display size from.
    @result     A CGSize returning the nominal display size of the buffer
		Returns zero rect if called with a non-CVImageBufferRef type or NULL.
*/
CV_EXPORT CGRect CVImageBufferGetCleanRect(CVImageBufferRef imageBuffer) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

#if TARGET_OS_MAC
/*!
    @function   CVImageBufferGetColorSpace
    @abstract   Returns the color space of a CVImageBuffer.
    @param      imageBuffer A CVImageBuffer that you wish to retrieve the color space from.
    @result     A CGColorSpaceRef representing the color space of the buffer.
		Returns NULL if called with a non-CVImageBufferRef type or NULL.
*/
CV_EXPORT CGColorSpaceRef CVImageBufferGetColorSpace(CVImageBufferRef imageBuffer) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;
#endif

#if defined(__cplusplus)
}
#endif

#endif
