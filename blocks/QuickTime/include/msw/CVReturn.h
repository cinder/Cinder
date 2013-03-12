/*
 *  CVReturn.h
 *  CoreVideo
 *
 *  Copyright (c) 2004 Apple Computer, Inc. All rights reserved.
 *
 */
 
 /*! @header CVReturn.h
	@copyright 2004 Apple Computer, Inc. All rights reserved.
	@availability Mac OS X 10.4 or later
    @discussion Here you can find all the CoreVideo specific error codes. 
		   
*/

#if !defined(__COREVIDEO_CVRETURN_H__)
#define __COREVIDEO_CVRETURN_H__ 1

#include <TargetConditionals.h>

#if TARGET_OS_MAC
#include <QuartzCore/CVBase.h>
#else
#include <CVBase.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif
/*!
    @enum	CVReturn
    @abstract   CoreVideo specific error codes

    @constant   kCVReturnSuccess Function executed successfully without errors.
    @constant   kCVReturnFirst Placeholder to mark the beginning of the range of CVReturn codes.
    @constant   kCVReturnLast Placeholder to mark the end of the range of CVReturn codes.
 
    @constant   kCVReturnInvalidArgument At least one of the arguments passed in is not valid. Either out of range or the wrong type.
    @constant   kCVReturnAllocationFailed The allocation for a buffer or buffer pool failed. Most likely because of lack of resources.

    @constant   kCVReturnInvalidDisplay A CVDisplayLink cannot be created for the given DisplayRef.
    @constant   kCVReturnDisplayLinkAlreadyRunning The CVDisplayLink is already started and running.
    @constant   kCVReturnDisplayLinkNotRunning The CVDisplayLink has not been started.
    @constant   kCVReturnDisplayLinkCallbacksNotSet The render and display callbacks or the output callback is not set. You have to set either the render/display pair or the single output callback.

    @constant   kCVReturnInvalidPixelFormat The requested pixelformat is not supported for the CVBuffer type.
    @constant   kCVReturnInvalidSize The requested size (most likely too big) is not supported for the CVBuffer type.
    @constant   kCVReturnInvalidPixelBufferAttributes A CVBuffer cannot be created with the given attributes.
    @constant   kCVReturnPixelBufferNotOpenGLCompatible The Buffer cannot be used with OpenGL as either its size, pixelformat or attributes are not supported by OpenGL.

    @constant   kCVReturnPoolAllocationFailed The allocation for the buffer pool failed. Most likely because of lack of resources. Check if your parameters are in range.
    @constant   kCVReturnInvalidPoolAttributes A CVBufferPool cannot be created with the given attributes.
*/

enum _CVReturn
{
    kCVReturnSuccess                         = 0,
    
    kCVReturnFirst                           = -6660,
    
    kCVReturnError                           = kCVReturnFirst,
    kCVReturnInvalidArgument                 = -6661,
    kCVReturnAllocationFailed                = -6662,
    
    // DisplayLink related errors
    kCVReturnInvalidDisplay                  = -6670,
    kCVReturnDisplayLinkAlreadyRunning       = -6671,
    kCVReturnDisplayLinkNotRunning           = -6672,
    kCVReturnDisplayLinkCallbacksNotSet      = -6673,
    
    // Buffer related errors
    kCVReturnInvalidPixelFormat		     = -6680,
    kCVReturnInvalidSize                     = -6681,
    kCVReturnInvalidPixelBufferAttributes    = -6682,    
    kCVReturnPixelBufferNotOpenGLCompatible  = -6683,
    
    // Buffer Pool related errors
    kCVReturnPoolAllocationFailed            = -6690,
    kCVReturnInvalidPoolAttributes           = -6691,
    
    kCVReturnLast			     = -6699
    
};
typedef int32_t CVReturn;

#if defined(__cplusplus)
}
#endif

#endif
