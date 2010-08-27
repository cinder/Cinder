/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#include "cinder/gl/gl.h"
#include "cinder/qtime/QuickTime.h"
#include "cinder/qtime/QuickTimeUtils.h"

#if defined( CINDER_MSW )
	#pragma push_macro( "__STDC_CONSTANT_MACROS" )
	#pragma push_macro( "_STDINT_H" )
		#undef __STDC_CONSTANT_MACROS
		#define _STDINT_H
		#include <CVPixelBuffer.h>
		#include <ImageCompression.h>
		#include <Movies.h>
		#include <QuickTimeComponents.h>
	#pragma pop_macro( "_STDINT_H" )
	#pragma pop_macro( "__STDC_CONSTANT_MACROS" )
#else
	#include <QuickTime/QuickTime.h>
	#include <ApplicationServices/ApplicationServices.h>
	#include <QuickTime/ImageCompression.h>
	#include <CoreVideo/CoreVideo.h>
#endif

using namespace std;

namespace cinder { namespace qtime {

#if ! defined( __LP64__ )

bool dictionarySetValue( CFMutableDictionaryRef dict, CFStringRef key, SInt32 value )
{
	bool         setNumber = false;
    CFNumberRef  number    = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &value);
	
    if( number != NULL ) {
		CFDictionarySetValue( dict, key, number );
		CFRelease( number );
		setNumber = true;
	}
	
    return setNumber;
}

bool dictionarySetPixelBufferPixelFormatType( bool alpha, CFMutableDictionaryRef dict )
{
	bool setPixelBufferOptions = false;
	setPixelBufferOptions = dictionarySetValue( dict, kCVPixelBufferPixelFormatTypeKey, ( alpha ) ? k32BGRAPixelFormat : k24RGBPixelFormat );
	return  setPixelBufferOptions;
}

bool dictionarySetPixelBufferSize( const unsigned int width, const unsigned int height, CFMutableDictionaryRef dict )
{
	bool setSize = false;

	setSize = dictionarySetValue( dict, kCVPixelBufferWidthKey, width );
	setSize = setSize && dictionarySetValue( dict, kCVPixelBufferHeightKey, height );
	
	return setSize;
}

bool dictionarySetPixelBufferBytesPerRowAlignment( CFMutableDictionaryRef dict )
{
	bool setAlignment = false;
	setAlignment = dictionarySetValue( dict, kCVPixelBufferBytesPerRowAlignmentKey, 1 );
	return setAlignment;
}

void dictionarySetPixelBufferOpenGLCompatibility( CFMutableDictionaryRef dict )
{
	CFDictionarySetValue(dict, kCVPixelBufferOpenGLCompatibilityKey, kCFBooleanTrue);
}

bool dictionarySetPixelBufferOptions( unsigned int width, unsigned int height, bool alpha, CFMutableDictionaryRef *pixelBufferOptions )
{
	bool setPixelBufferOptions = false;
    CFMutableDictionaryRef  pixelBufferDict = CFDictionaryCreateMutable( kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks );

    if ( pixelBufferDict != NULL ) {
		if ( dictionarySetPixelBufferPixelFormatType( alpha, pixelBufferDict ) ) {
			if ( dictionarySetPixelBufferSize( width, height, pixelBufferDict ) ) {
				if ( dictionarySetPixelBufferBytesPerRowAlignment( pixelBufferDict ) ) {
					dictionarySetPixelBufferOpenGLCompatibility( pixelBufferDict );
					*pixelBufferOptions = pixelBufferDict;
					setPixelBufferOptions = true;
				}
			}
		}
	}
	
	return setPixelBufferOptions;
}

CFMutableDictionaryRef initQTVisualContextOptions( int width, int height, bool alpha )
{
	CFMutableDictionaryRef  visualContextOptions = NULL;
    CFMutableDictionaryRef  pixelBufferOptions   = NULL;
												   
	bool  setPixelBufferOptions = dictionarySetPixelBufferOptions( width, height, alpha, &pixelBufferOptions );
	
	if( pixelBufferOptions != NULL ) {
		if( setPixelBufferOptions ) {
			visualContextOptions = CFDictionaryCreateMutable( kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks );
			if( visualContextOptions != NULL ) {
				CFDictionarySetValue( visualContextOptions, kQTVisualContextPixelBufferAttributesKey, pixelBufferOptions );
			}
		}
			
		CFRelease( pixelBufferOptions );
	}
	
	return  visualContextOptions;
}

::ItemCount openMovieBaseProperties( QTNewMoviePropertyElement movieProps[10] )
{
	ItemCount moviePropCount = 0;
	Boolean boolTrue = true;

	movieProps[moviePropCount].propClass = kQTPropertyClass_MovieInstantiation;
	movieProps[moviePropCount].propID = kQTMovieInstantiationPropertyID_DontAskUnresolvedDataRefs;
	movieProps[moviePropCount].propValueSize = sizeof(boolTrue);
	movieProps[moviePropCount].propValueAddress = &boolTrue;
	movieProps[moviePropCount].propStatus = 0;
	moviePropCount++;

	movieProps[moviePropCount].propClass = kQTPropertyClass_NewMovieProperty;
	movieProps[moviePropCount].propID = kQTNewMoviePropertyID_Active;
	movieProps[moviePropCount].propValueSize = sizeof(boolTrue);
	movieProps[moviePropCount].propValueAddress = &boolTrue;
	movieProps[moviePropCount].propStatus = 0;
	moviePropCount++;
	 
	movieProps[moviePropCount].propClass = kQTPropertyClass_NewMovieProperty;
	movieProps[moviePropCount].propID = kQTNewMoviePropertyID_DontInteractWithUser;
	movieProps[moviePropCount].propValueSize = sizeof(boolTrue);
	movieProps[moviePropCount].propValueAddress = &boolTrue;
	movieProps[moviePropCount].propStatus = 0;
	moviePropCount++;

#if 0
	movieProps[moviePropCount].propClass = kQTPropertyClass_Context;
	movieProps[moviePropCount].propID = kQTContextPropertyID_VisualContext;
	movieProps[moviePropCount].propValueSize = sizeof(visualContext);
	movieProps[moviePropCount].propValueAddress = &visualContext;
	movieProps[moviePropCount].propStatus = 0;
	moviePropCount++;
	 
	movieProps[moviePropCount].propClass = kQTPropertyClass_Context;
	movieProps[moviePropCount].propID = kQTContextPropertyID_AudioContext;
	movieProps[moviePropCount].propValueSize = sizeof(audioContext);
	movieProps[moviePropCount].propValueAddress = &audioContext;
	movieProps[moviePropCount].propStatus = 0;
	moviePropCount++;
#endif

	return moviePropCount;
}

::Movie openMovieFromUrl( const Url &url )
{
	::Movie result;
	::QTNewMoviePropertyElement movieProps[10];
	ItemCount moviePropCount = 0;

	moviePropCount = openMovieBaseProperties( movieProps );

	shared_ptr<const __CFString> urlStringCF = shared_ptr<const __CFString>( ::CFStringCreateWithCString( kCFAllocatorDefault, url.str().c_str(), kCFStringEncodingUTF8 ), ::CFRelease );
	CFURLRef urlCFBase = ::CFURLCreateWithString( kCFAllocatorDefault, urlStringCF.get(), NULL );
	shared_ptr<const __CFURL> urlCF = shared_ptr<const __CFURL>( urlCFBase, ::CFRelease );
	// Store the movie properties in the array
	movieProps[moviePropCount].propClass = kQTPropertyClass_DataLocation;
	movieProps[moviePropCount].propID = kQTDataLocationPropertyID_CFURL;
	movieProps[moviePropCount].propValueSize = sizeof(CFURLRef);
	movieProps[moviePropCount].propValueAddress = (void*)&urlCFBase;
	movieProps[moviePropCount].propStatus = 0;
	moviePropCount++;
	 
	OSStatus err;
	if( (err = ::NewMovieFromProperties( moviePropCount, movieProps, 0, NULL, &result ) ) != noErr ) {
		throw QuickTimeFileInvalidExc();
	}

	return result;
}

::Movie openMovieFromPath( const std::string &path )
{
	::Movie result;
	QTNewMoviePropertyElement movieProps[10];
	ItemCount moviePropCount = 0;

	moviePropCount = openMovieBaseProperties( movieProps );

	::CFStringRef basePathCF = ::CFStringCreateWithCString( kCFAllocatorDefault, path.c_str(), kCFStringEncodingUTF8 );
	shared_ptr<const __CFString> pathCF = shared_ptr<const __CFString>( basePathCF, ::CFRelease );
	// Store the movie properties in the array
	movieProps[moviePropCount].propClass = kQTPropertyClass_DataLocation;
	movieProps[moviePropCount].propID = kQTDataLocationPropertyID_CFStringNativePath;
	movieProps[moviePropCount].propValueSize = sizeof(CFStringRef);
	movieProps[moviePropCount].propValueAddress = (void*)&basePathCF;
	movieProps[moviePropCount].propStatus = 0;
	moviePropCount++;

	OSStatus err;
	if( (err = ::NewMovieFromProperties( moviePropCount, movieProps, 0, NULL, &result ) ) != noErr ) {
		throw QuickTimePathInvalidExc();
	}
		
	return result;
}

Handle createPointerReferenceHandle( void *data, Size dataSize )
{
	Handle dataRef = NULL;
	PointerDataRefRecord ptrDataRefRec;
	OSErr err;

	ptrDataRefRec.data = data;
	ptrDataRefRec.dataLength = dataSize;

	// create a data reference handle for our data
	err = ::PtrToHand( &ptrDataRefRec, &dataRef, sizeof(PointerDataRefRecord) );
	if( err != noErr )
		return 0;

	return dataRef;
}

OSStatus ptrDataRefAddFileNameExtension( ComponentInstance dataRefHandler, const string &fileName )
{
	OSStatus osErr = noErr;
	unsigned char myChar = 0;
	Handle fileNameHandle = NULL;

	// create a handle with our file name string
	// if we were passed an empty string, then we need to add this null string (a single 0 byte) to the handle
	if( fileName.empty() )
		osErr = ::PtrToHand( &myChar, &fileNameHandle, sizeof(myChar) );
	else {
		shared_ptr<char> tempStr( new char[fileName.size()+1], checked_array_deleter<char>() );
		memcpy( &tempStr.get()[1], &fileName[0], fileName.size() );
		tempStr.get()[0] = fileName.size();
		osErr = ::PtrToHand( &tempStr.get()[0], &fileNameHandle, tempStr.get()[0] + 1 );
		if( osErr != noErr ) goto bail;
	}
	if( osErr != noErr) goto bail;

	// set the data ref extension for the data ref handler
	osErr = ::DataHSetDataRefExtension( dataRefHandler, fileNameHandle, kDataRefExtensionFileName );

 bail:
    if( fileNameHandle )         // no longer need this
        ::DisposeHandle( fileNameHandle );

    return osErr;

}


OSStatus ptrDataRefAddMIMETypeExtension( ComponentInstance dataRefHandler, const std::string &mimeType )
{
    OSStatus osErr = noErr;
    Handle mimeTypeHndl = NULL;

    if( mimeType.empty() )
        return paramErr;

	shared_ptr<char> tempStr( new char[mimeType.size()+1], checked_array_deleter<char>() );
	memcpy( &tempStr.get()[1], &mimeType[0], mimeType.size() );
	tempStr.get()[0] = mimeType.size();
    osErr = ::PtrToHand( &tempStr.get()[0], &mimeTypeHndl, tempStr.get()[0] + 1 );
    if( osErr != noErr ) goto bail;

    // set the data ref extension for the data ref handler
    osErr = ::DataHSetDataRefExtension( dataRefHandler, mimeTypeHndl, kDataRefExtensionMIMEType );

 bail:
    if( mimeTypeHndl )         // no longer need this
        ::DisposeHandle( mimeTypeHndl );

    return osErr;
}

Handle createPointerDataRefWithExtensions( void *data, size_t dataSize, const string &fileName, const string &mimeType )
{
	OSStatus err = noErr;
	Handle dataRef = NULL;
	ComponentInstance dataRefHandler = NULL;

	// First create a data reference handle for our data
	dataRef = createPointerReferenceHandle( data, dataSize );
	if( ! dataRef )
		goto bail;

	//  Get a data handler for our data reference
	err = OpenADataHandler(
			dataRef,                    /* data reference */
			PointerDataHandlerSubType,  /* data ref. type */
			NULL,                       /* anchor data ref. */
			(OSType)0,                  /* anchor data ref. type */
			NULL,                       /* time base for data handler */
			kDataHCanRead,              /* flag for data handler usage */
			&dataRefHandler);           /* returns the data handler */
	if( err ) goto bail;

	// We can add the filename to the data ref to help
	// importer finding process. Find uses the extension.
	// If we add a filetype or mimetype we must add a
	// filename -- even if it is an empty string
	if( ( ! fileName.empty() ) || ( ! mimeType.empty() ) ) {
		err = ptrDataRefAddFileNameExtension( dataRefHandler, fileName );
		if( err ) goto bail;
	}

	// to add MIME type information, add a classic atom followed by
	// a Pascal string holding the MIME type
	if ( ! mimeType.empty() ) {
		err = ptrDataRefAddMIMETypeExtension( dataRefHandler, mimeType );
		if( err ) goto bail;
	}

	// dispose old data ref handle because it does not contain our new changes
	::DisposeHandle( dataRef );
	dataRef = NULL;

	// re-acquire data reference from the data handler to get the new changes
	err = ::DataHGetDataRef( dataRefHandler, &dataRef );
	if( err ) goto bail;

	::CloseComponent( dataRefHandler );

	return dataRef;
 bail:
    if( dataRefHandler ) {
		::CloseComponent(dataRefHandler);
    }

    if( dataRef ) {         // make sure and dispose the data reference handle once we are done with it
        ::DisposeHandle( dataRef );
    }

    return NULL;
}

// Refer to http://developer.apple.com/technotes/tn/tn1195.html Technical Note TN1195 "Tagging Handle and Pointer Data References in QuickTime"
::Movie openMovieFromMemory( const void *data, size_t dataSize, const string &fileNameHint, const string &mimeTypeHint )
{
	Handle dataRefHandle = createPointerDataRefWithExtensions( const_cast<void*>( data ), dataSize, fileNameHint, mimeTypeHint );

	::Movie result;
/*	QTNewMoviePropertyElement movieProps[10];
	ItemCount moviePropCount = 0;
	DataReferenceRecord dataRef = {0};
	
	moviePropCount = openMovieBaseProperties( movieProps );

	// Store the movie properties in the array
	movieProps[moviePropCount].propClass = kQTPropertyClass_DataLocation;
	movieProps[moviePropCount].propID = kQTDataLocationPropertyID_QTDataHandler;
	movieProps[moviePropCount].propValueSize = sizeof(aQTDataRef);
	movieProps[moviePropCount].propValueAddress = (void*)&dataRefHandle;
	movieProps[moviePropCount].propStatus = 0;
	moviePropCount++;

	OSErr err;
	if( (err = NewMovieFromProperties(moviePropCount, movieProps, 0, NULL, &result) ) != noErr ) {
		throw QuickTimeErrorLoadingExc();
	}*/

	short myResID = 0;
	OSErr err = NewMovieFromDataRef( &result, newMovieActive | newMovieAsyncOK, &myResID, dataRefHandle, 'ptr ' );
	if( err )
		throw QuickTimeErrorLoadingExc();
	
	return result;
}

static void CVPixelBufferDealloc( void *refcon )
{
	::CVBufferRelease( (CVPixelBufferRef)(refcon) );
}

Surface8u convertCVPixelBufferToSurface( CVPixelBufferRef pixelBufferRef )
{
	CVPixelBufferLockBaseAddress( pixelBufferRef, 0 );
	uint8_t *ptr = reinterpret_cast<uint8_t*>( CVPixelBufferGetBaseAddress( pixelBufferRef ) );
	int32_t rowBytes = CVPixelBufferGetBytesPerRow( pixelBufferRef );
	OSType type = CVPixelBufferGetPixelFormatType( pixelBufferRef );
	size_t width = CVPixelBufferGetWidth( pixelBufferRef );
	size_t height = CVPixelBufferGetHeight( pixelBufferRef );
	SurfaceChannelOrder sco;
	if( type == k24RGBPixelFormat )
		sco = SurfaceChannelOrder::RGB;
	else if( type == k32ARGBPixelFormat )
		sco = SurfaceChannelOrder::ARGB;
	else if( type == k24BGRPixelFormat )
		sco = SurfaceChannelOrder::BGR;
	else if( type == k32BGRAPixelFormat )
		sco = SurfaceChannelOrder::BGRA;
	Surface result( ptr, width, height, rowBytes, sco );
	result.setDeallocator( CVPixelBufferDealloc, pixelBufferRef );
	return result;
}

#endif // ! defined( __LP64__ )

///////////////////////////////////////////////////////////////////////////////////////////////
// ImageTargetCgImage
ImageTargetCvPixelBufferRef ImageTargetCvPixelBuffer::createRef( ImageSourceRef imageSource, bool convertToYpCbCr )
{
	return ImageTargetCvPixelBufferRef( new ImageTargetCvPixelBuffer( imageSource, convertToYpCbCr ) );
}

ImageTargetCvPixelBuffer::ImageTargetCvPixelBuffer( ImageSourceRef imageSource, bool convertToYpCbCr )
	: ImageTarget(), mPixelBufferRef( 0 ), mConvertToYpCbCr( convertToYpCbCr )
{
	setSize( (size_t)imageSource->getWidth(), (size_t)imageSource->getHeight() );
	
	//http://developer.apple.com/mac/library/qa/qa2006/qa1501.html
	
	// if we're converting to YCbCr, we'll load all of the data as RGB in terms of ci::ImageIo
	// but we run color space conversion over it later in the finalize method
	OSType formatType;
	if( ! mConvertToYpCbCr ) {
		switch( imageSource->getDataType() ) {
			// for now all we support is 8 bit RGB(A)
			case ImageIo::UINT16:
			case ImageIo::FLOAT32:
			case ImageIo::UINT8:
				setDataType( ImageIo::UINT8 );
				if( imageSource->hasAlpha () ) {
					formatType = k32ARGBPixelFormat;
					setChannelOrder( ImageIo::ARGB );
				}
				else {
					formatType = k24RGBPixelFormat;
					setChannelOrder( ImageIo::RGB );
				}
				setColorModel( ImageIo::CM_RGB );
			break;
			default:
				throw ImageIoException();
		}
	}
	else {
		formatType = 'v408';/*k4444YpCbCrA8PixelFormat;*/
		setDataType( ImageIo::UINT8 );
		setChannelOrder( ImageIo::RGBA );
		setColorModel( ImageIo::CM_RGB );
	}

	if( ::CVPixelBufferCreate( kCFAllocatorDefault, imageSource->getWidth(), imageSource->getHeight(), 
				formatType, NULL, &mPixelBufferRef ) != kCVReturnSuccess )
		throw ImageIoException();
	
	if( ::CVPixelBufferLockBaseAddress( mPixelBufferRef, 0 ) != kCVReturnSuccess )
		throw ImageIoException();
	mData = reinterpret_cast<uint8_t*>( ::CVPixelBufferGetBaseAddress( mPixelBufferRef ) );
	mRowBytes = ::CVPixelBufferGetBytesPerRow( mPixelBufferRef );
}

ImageTargetCvPixelBuffer::~ImageTargetCvPixelBuffer()
{
	if( mPixelBufferRef ) {
		::CVPixelBufferUnlockBaseAddress( mPixelBufferRef, 0 );
		::CVPixelBufferRelease( mPixelBufferRef );
	}
}

void* ImageTargetCvPixelBuffer::getRowPointer( int32_t row )
{
	return &mData[row * mRowBytes];
}

void ImageTargetCvPixelBuffer::finalize()
{
	switch( ::CVPixelBufferGetPixelFormatType( mPixelBufferRef ) ) {
		case 'v408':/*k4444YpCbCrA8PixelFormat:*/
			convertDataToAYpCbCr();
			::CVBufferSetAttachment( mPixelBufferRef, kCVImageBufferYCbCrMatrixKey, kCVImageBufferYCbCrMatrix_ITU_R_601_4, kCVAttachmentMode_ShouldPropagate );
		break;
		case 'v308':/*k444YpCbCr8CodecType:*/
			convertDataToYpCbCr();
			::CVBufferSetAttachment( mPixelBufferRef, kCVImageBufferYCbCrMatrixKey, kCVImageBufferYCbCrMatrix_ITU_R_601_4, kCVAttachmentMode_ShouldPropagate );
		break;
	}
}

// Assumes RGB order 8 bit unsinged input, results in Rec. 601 YpCbCr
void ImageTargetCvPixelBuffer::convertDataToYpCbCr()
{
	for( uint32_t y = 0; y < mHeight; ++y ) {
		uint8_t *data = reinterpret_cast<uint8_t*>( getRowPointer( y ) );
		for( uint32_t x = 0; x < mWidth; ++x ) {
			float r = data[x*3+0] / 255.0f;
			float g = data[x*3+1] / 255.0f;
			float b = data[x*3+2] / 255.0f;
			uint8_t yp = 16 + ( 65.481f * r + 128.553f * g + 24.966f * b );
			uint8_t cb = 128 + ( -37.797f * r + -74.203f * g + 112 * b );
			uint8_t cr = 128 + ( 112 * r + -93.786f * g + -18.214f * b );
			data[x*3+0] = yp;
			data[x*3+1] = cb;
			data[x*3+2] = cr;
		}
	}
}

// Assumes RGBA order 8 bit unsigned input, results in Rec. 601 YpCbCrA
void ImageTargetCvPixelBuffer::convertDataToAYpCbCr()
{
	for( uint32_t y = 0; y < mHeight; ++y ) {
		uint8_t *data = reinterpret_cast<uint8_t*>( getRowPointer( y ) );
		for( uint32_t x = 0; x < mWidth; ++x ) {
			float r = data[x*4+0] / 255.0f;
			float g = data[x*4+1] / 255.0f;
			float b = data[x*4+2] / 255.0f;
			uint8_t yp = 16 + ( 65.481f * r + 128.553f * g + 24.966f * b );
			uint8_t cb = 128 + ( -37.797f * r + -74.203f * g + 112 * b );
			uint8_t cr = 128 + ( 112 * r + -93.786f * g + -18.214f * b );
			data[x*4+0] = cb;
			data[x*4+1] = yp;
			data[x*4+2] = cr;
		}
	}
}

CVPixelBufferRef createCvPixelBuffer( ImageSourceRef imageSource, bool convertToYpCbCr )
{
	ImageTargetCvPixelBufferRef target = ImageTargetCvPixelBuffer::createRef( imageSource, convertToYpCbCr );
	imageSource->load( target );
	target->finalize();
	::CVPixelBufferRef result( target->getCvPixelBuffer() );
	::CVPixelBufferRetain( result );
	return result;
}

#if ! defined( __LP64__ )
///////////////////////////////////////////////////////////////////////////////////////////////
// ImageTargetGWorld
ImageTargetGWorldRef ImageTargetGWorld::createRef( ImageSourceRef imageSource )
{
	return ImageTargetGWorldRef( new ImageTargetGWorld( imageSource ) );
}

ImageTargetGWorld::ImageTargetGWorld( ImageSourceRef imageSource )
	: ImageTarget(), mGWorld( 0 ), mPixMap( 0 )
{
	setSize( (size_t)imageSource->getWidth(), (size_t)imageSource->getHeight() );
	
	OSType formatType;
	// for now all we support is 8 bit RGBA
	setDataType( ImageIo::UINT8 );
	formatType = k32ARGBPixelFormat;
	setChannelOrder( ImageIo::ARGB );
	setColorModel( ImageIo::CM_RGB );

	::Rect boundsRect;
	boundsRect.left = boundsRect.top = 0;
	boundsRect.right = (short)imageSource->getWidth();
	boundsRect.bottom = (short)imageSource->getHeight();
	if( ::QTNewGWorld( &mGWorld, formatType, &boundsRect, NULL, NULL, 0 ) != noErr )
		throw ImageIoException();
	
	mPixMap = ::GetGWorldPixMap( mGWorld );
    if( ! ::LockPixels( mPixMap ) ) {
		::DisposeGWorld( mGWorld );
        throw ImageIoException();
	}

#if defined( CINDER_MSW )
	mData = reinterpret_cast<uint8_t*>( (**mPixMap).baseAddr );
	mRowBytes = ( (**mPixMap).rowBytes ) & 0x3FFF;
#else
	mData = reinterpret_cast<uint8_t*>( ::GetPixBaseAddr( mPixMap ) );
	mRowBytes = ::GetPixRowBytes( mPixMap );
#endif
}

void ImageTargetGWorld::finalize()
{
	if( ::GetPixelsState( mPixMap ) & pixelsLocked )
		::UnlockPixels( mPixMap );
}

void* ImageTargetGWorld::getRowPointer( int32_t row )
{
	return &mData[row * mRowBytes];
}

GWorldPtr createGWorld( ImageSourceRef imageSource )
{
	ImageTargetGWorldRef target = ImageTargetGWorld::createRef( imageSource );
	imageSource->load( target );
	target->finalize();
	return target->getGWorld();
}

#endif // ! defined( __LP64__ )

} } // namespace cinder::qtime