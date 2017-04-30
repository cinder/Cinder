/*
 Copyright (c) 2015, The Cinder Project, All rights reserved.
 
 This code is intended for use with the Cinder C++ library: http://libcinder.org
 
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

#include "cinder/Cinder.h"

// This path is used on iOS or Mac OS X 10.8+
#if defined( CINDER_COCOA_TOUCH ) || ( defined( CINDER_MAC ) && ( MAC_OS_X_VERSION_MIN_REQUIRED >= 1080 ) )

#include "cinder/gl/platform.h"

#include "cinder/qtime/QuickTimeImplAvf.h"
#include "cinder/qtime/AvfUtils.h"

#if defined( CINDER_COCOA )
	#include <CoreVideo/CoreVideo.h>
	#include <AVFoundation/AVFoundation.h>
#endif

using namespace std;

namespace cinder { namespace qtime {

bool setAudioSessionModes()
{
#if defined( CINDER_COCOA_TOUCH )
	NSError* error = nil;
	[[AVAudioSession sharedInstance] setMode:AVAudioSessionModeMoviePlayback error:&error];
	return [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayback error:&error];
	
#else
	return false;
	
#endif
}

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
#if defined( CINDER_COCOA_TOUCH )
	setPixelBufferOptions = dictionarySetValue( dict, kCVPixelBufferPixelFormatTypeKey, ( alpha ) ? kCVPixelFormatType_32BGRA : kCVPixelFormatType_24RGB );
#elif defined( CINDER_COCOA )
	setPixelBufferOptions = dictionarySetValue( dict, kCVPixelBufferPixelFormatTypeKey, ( alpha ) ? k32BGRAPixelFormat : k24RGBPixelFormat );
#endif
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
	
	/*
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
			dataRef,                    // data reference
			PointerDataHandlerSubType,  // data ref. type
			NULL,                       // anchor data ref.
			(OSType)0,                  // anchor data ref. type
			NULL,                       // time base for data handler
			kDataHCanRead,              // flag for data handler usage
			&dataRefHandler);           // returns the data handler
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
*/
	
void CVPixelBufferDealloc( void* refcon )
{
	::CVBufferRelease( (CVPixelBufferRef)(refcon) );
}

Surface8uRef convertCvPixelBufferToSurface( CVPixelBufferRef pixelBufferRef )
{
	::CVPixelBufferLockBaseAddress( pixelBufferRef, 0 );
	uint8_t *ptr = reinterpret_cast<uint8_t*>( ::CVPixelBufferGetBaseAddress( pixelBufferRef ) );
	int32_t rowBytes = (int32_t)::CVPixelBufferGetBytesPerRow( pixelBufferRef );
	OSType type = ::CVPixelBufferGetPixelFormatType( pixelBufferRef );
	size_t width = ::CVPixelBufferGetWidth( pixelBufferRef );
	size_t height = ::CVPixelBufferGetHeight( pixelBufferRef );
	::CVPixelBufferUnlockBaseAddress(pixelBufferRef, 0);
	
	SurfaceChannelOrder sco;
#if defined( CINDER_COCOA_TOUCH )
	if (type == kCVPixelFormatType_24RGB )
		sco = SurfaceChannelOrder::RGB;
	else if (type == kCVPixelFormatType_24BGR )
		sco = SurfaceChannelOrder::BGR;
	else if (type == kCVPixelFormatType_30RGB )
		sco = SurfaceChannelOrder::RGB;
	else if ( type == kCVPixelFormatType_32ARGB )
		sco = SurfaceChannelOrder::ARGB;
	else if (type == kCVPixelFormatType_32BGRA )
		sco = SurfaceChannelOrder::BGRA;
	else if (type == kCVPixelFormatType_32ABGR )
		sco = SurfaceChannelOrder::ABGR;
	else if (type == kCVPixelFormatType_32RGBA )
		sco = SurfaceChannelOrder::BGRA;
	
#elif defined( CINDER_COCOA )
	if( type == k24RGBPixelFormat )
		sco = SurfaceChannelOrder::RGB;
	else if( type == k32ARGBPixelFormat )
		sco = SurfaceChannelOrder::ARGB;
	else if( type == k24BGRPixelFormat )
		sco = SurfaceChannelOrder::BGR;
	else if( type == k32BGRAPixelFormat )
		sco = SurfaceChannelOrder::BGRA;
#endif
	
	return std::shared_ptr<Surface8u>( new Surface8u( ptr, (int32_t)width, (int32_t)height, rowBytes, sco ),
		[=]( Surface8u *s ) { delete s; CVPixelBufferDealloc( pixelBufferRef ); } );
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ImageTargetCgImage
ImageTargetCvPixelBufferRef ImageTargetCvPixelBuffer::createRef( ImageSourceRef imageSource, bool convertToYpCbCr )
{
	return ImageTargetCvPixelBufferRef( new ImageTargetCvPixelBuffer( imageSource, convertToYpCbCr ) );
}

ImageTargetCvPixelBufferRef ImageTargetCvPixelBuffer::createRef( ImageSourceRef imageSource, CVPixelBufferPoolRef pbPool )
{
	return ImageTargetCvPixelBufferRef( new ImageTargetCvPixelBuffer( imageSource, pbPool ) );
}

ImageTargetCvPixelBuffer::ImageTargetCvPixelBuffer( ImageSourceRef imageSource, CVPixelBufferPoolRef pbPool )
	: ImageTarget(), mPixelBufferRef( 0 )
{
	setSize( imageSource->getWidth(), imageSource->getHeight() );
	
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
#if defined( CINDER_COCOA_TOUCH )
					formatType = kCVPixelFormatType_32ARGB;
#elif defined( CINDER_COCOA )
					formatType = k32ARGBPixelFormat;
#endif
					setChannelOrder( ImageIo::ARGB );
				}
				else {
#if defined( CINDER_COCOA_TOUCH )
					formatType = kCVPixelFormatType_24RGB;
#elif defined( CINDER_COCOA )
					formatType = k24RGBPixelFormat;
#endif
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
	
	// TODO: Can we create the buffer from the pool????? Seems like no at first attempt --maybe a pixel buffer attributes mismatch?
//	CFMutableDictionaryRef attributes = CFDictionaryCreateMutable( kCFAllocatorDefault, 6, nil, nil );
//	dictionarySetPixelBufferOpenGLCompatibility( attributes );
//	CVReturn status = CVPixelBufferPoolCreatePixelBufferWithAuxAttributes(kCFAllocatorDefault, pbPool, attributes, &mPixelBufferRef);
//	if( attributes ) {
//		CFRelease( attributes );
//		attributes = NULL;
//	}
	CVReturn status = CVPixelBufferPoolCreatePixelBuffer( kCFAllocatorDefault, pbPool, &mPixelBufferRef );
	if( kCVReturnSuccess != status )
		throw ImageIoException();
	
	if( ::CVPixelBufferLockBaseAddress( mPixelBufferRef, 0 ) != kCVReturnSuccess )
		throw ImageIoException();
	mData = reinterpret_cast<uint8_t*>( ::CVPixelBufferGetBaseAddress( mPixelBufferRef ) );
	mRowBytes = ::CVPixelBufferGetBytesPerRow( mPixelBufferRef );
std::cout << "Total size: " << ::CVPixelBufferGetDataSize( mPixelBufferRef ) << " Planar: " << (int)::CVPixelBufferIsPlanar( mPixelBufferRef ) << std::endl;	
}


ImageTargetCvPixelBuffer::ImageTargetCvPixelBuffer( ImageSourceRef imageSource, bool convertToYpCbCr )
	: ImageTarget(), mPixelBufferRef( 0 ), mConvertToYpCbCr( convertToYpCbCr )
{
	setSize( imageSource->getWidth(), imageSource->getHeight() );
	
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
#if defined( CINDER_COCOA_TOUCH )
					formatType = kCVPixelFormatType_32ARGB;
#elif defined( CINDER_COCOA )
					formatType = k32ARGBPixelFormat;
#endif
					setChannelOrder( ImageIo::ARGB );
				}
				else {
#if defined( CINDER_COCOA_TOUCH )
					formatType = kCVPixelFormatType_24RGB;
#elif defined( CINDER_COCOA )
					formatType = k24RGBPixelFormat;
#endif
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
	for( int32_t y = 0; y < mHeight; ++y ) {
		uint8_t *data = reinterpret_cast<uint8_t*>( getRowPointer( y ) );
		for( int32_t x = 0; x < mWidth; ++x ) {
			float r = data[x*3+0] / 255.0f;
			float g = data[x*3+1] / 255.0f;
			float b = data[x*3+2] / 255.0f;
			uint8_t yp = 16 + (int)( 65.481f * r + 128.553f * g + 24.966f * b );
			uint8_t cb = 128 + (int)( -37.797f * r + -74.203f * g + 112 * b );
			uint8_t cr = 128 + (int)( 112 * r + -93.786f * g + -18.214f * b );
			data[x*3+0] = yp;
			data[x*3+1] = cb;
			data[x*3+2] = cr;
		}
	}
}

// Assumes RGBA order 8 bit unsigned input, results in Rec. 601 YpCbCrA
void ImageTargetCvPixelBuffer::convertDataToAYpCbCr()
{
	for( int32_t y = 0; y < mHeight; ++y ) {
		uint8_t *data = reinterpret_cast<uint8_t*>( getRowPointer( y ) );
		for( int32_t x = 0; x < mWidth; ++x ) {
			float r = data[x*4+0] / 255.0f;
			float g = data[x*4+1] / 255.0f;
			float b = data[x*4+2] / 255.0f;
			uint8_t yp = 16 + (int)( 65.481f * r + 128.553f * g + 24.966f * b );
			uint8_t cb = 128 + (int)( -37.797f * r + -74.203f * g + 112 * b );
			uint8_t cr = 128 + (int)( 112 * r + -93.786f * g + -18.214f * b );
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

CVPixelBufferRef createCvPixelBuffer( ImageSourceRef imageSource, CVPixelBufferPoolRef pbPool, bool convertToYpCbCr )
{
	ImageTargetCvPixelBufferRef target = ImageTargetCvPixelBuffer::createRef( imageSource, pbPool );
	imageSource->load( target );
	target->finalize();
	::CVPixelBufferRef result( target->getCvPixelBuffer() );
	::CVPixelBufferRetain( result );
	return result;
}

} } // namespace cinder::qtime

// // This path is used on iOS or Mac OS X 10.8+
#endif // defined( CINDER_COCOA_TOUCH ) || ( defined( CINDER_MAC ) && ( MAC_OS_X_VERSION_MIN_REQUIRED >= 1080 ) )
