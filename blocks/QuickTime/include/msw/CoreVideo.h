/*
 *  CoreVideo.h
 *  CoreVideo
 *
 *  Copyright (c) 2004 Apple Computer, Inc. All rights reserved.
 *
 */
/*!
    @header CoreVideo
	@copyright 2004 Apple Computer, Inc. All rights reserved.
	@availability Mac OS X 10.4 or later
    @abstract   Umbrella header for the CoreVideo framework
    @discussion This header includes all necesssary headers for the CoreVideo API
*/

#include <TargetConditionals.h>
#include <AvailabilityMacros.h>


#if TARGET_OS_MAC
#include <QuartzCore/CVReturn.h>
#include <QuartzCore/CVBase.h>
#include <QuartzCore/CVHostTime.h>
#include <QuartzCore/CVDisplayLink.h>
#include <QuartzCore/CVBuffer.h>
#include <QuartzCore/CVPixelBuffer.h>
#include <QuartzCore/CVPixelBufferPool.h>
#include <QuartzCore/CVOpenGLBuffer.h>
#include <QuartzCore/CVOpenGLBufferPool.h>
#include <QuartzCore/CVOpenGLTexture.h>
#include <QuartzCore/CVOpenGLTextureCache.h>
#include <QuartzCore/CVPixelFormatDescription.h>
#else
#include <CVReturn.h>
#include <CVBase.h>
#include <CVHostTime.h>
#include <CVBuffer.h>
#include <CVPixelBuffer.h>
#include <CVPixelBufferPool.h>
#include <CVPixelFormatDescription.h>
#include <CVDirect3DBuffer.h>
#include <CVDirect3DBufferPool.h>
#include <CVDirect3DTexture.h>
#include <CVDirect3DTextureCache.h>
#endif
