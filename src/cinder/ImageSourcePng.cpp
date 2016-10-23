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

#include "cinder/ImageSourcePng.h"
#include "cinder/Log.h"
#include <png.h>

using namespace std;

namespace cinder {

struct ci_png_info
{
	ci::IStreamRef		srcStreamRef;
};

extern "C" {

static void ci_PNG_stream_reader( png_structp mPngPtr, png_bytep data, png_size_t length )
{
	try {
		((ci_png_info*)png_get_io_ptr(mPngPtr))->srcStreamRef->readData( data, (size_t)length );
	}
	catch( std::exception &exc ) {
		CI_LOG_W( "failed to read png, what: " << exc.what() );
		longjmp( png_jmpbuf(mPngPtr), 1 );
	}
}

static void ci_png_warning( png_structp /*mPngPtr*/, png_const_charp /*message*/ )
{
//    fli_png_info_struct *info = mPngPtr ? (fli_png_info_struct*)png_get_io_ptr(mPngPtr) : NULL;
//    if ( !info || info->verbose )
//        wxLogWarning( wxString::FromAscii(message) );
}

static void ci_png_error( png_structp mPngPtr, png_const_charp message )
{
    ci_png_warning(NULL, message);
    longjmp( png_jmpbuf(mPngPtr), 1 );
}

} // extern "C"

///////////////////////////////////////////////////////////////////////////////
// Registrar
void ImageSourcePng::registerSelf()
{
	ImageIoRegistrar::SourceCreationFunc sourceFunc = ImageSourcePng::createSourceRef;
	ImageIoRegistrar::registerSourceType( "png", sourceFunc, 1 );
}

///////////////////////////////////////////////////////////////////////////////
// ImageSourcePng
ImageSourcePngRef ImageSourcePng::createRef( DataSourceRef dataSourceRef, ImageSource::Options options )
{
	return ImageSourcePngRef( new ImageSourcePng( dataSourceRef, options ) );
}

ImageSourcePng::ImageSourcePng( DataSourceRef dataSourceRef, ImageSource::Options /*options*/ )
	: ImageSource(), mInfoPtr( 0 ), mPngPtr( 0 )
{
	mPngPtr = png_create_read_struct( PNG_LIBPNG_VER_STRING, (png_voidp)NULL, NULL, NULL );
	if( ! mPngPtr ) {
		throw ImageSourcePngException( "Could not create png struct." );
	}

	mCiInfoPtr = shared_ptr<ci_png_info>( new ci_png_info );
	mCiInfoPtr->srcStreamRef = dataSourceRef->createStream();

	png_set_read_fn( mPngPtr, reinterpret_cast<void*>( mCiInfoPtr.get() ), ci_PNG_stream_reader );
	mInfoPtr = png_create_info_struct( mPngPtr );

	if( ! mInfoPtr ) {
		png_destroy_read_struct( &mPngPtr, (png_infopp)NULL, (png_infopp)NULL );
		mPngPtr = 0;
		throw ImageSourcePngException( "Could not destroy png read struct." );
	}
	
	if( ! loadHeader() )
		throw ImageSourcePngException( "Could not load png header." );
}

// part of this being separated allows for us to play nicely with the setjmp of libpng
bool ImageSourcePng::loadHeader()
{
	bool success = true;

	if( setjmp( png_jmpbuf(mPngPtr) ) ) {
		success = false;
	}
	else {
		png_read_info( mPngPtr, mInfoPtr );

		png_uint_32 width, height;
		int bitDepth, colorType, interlaceType, compressionType, filterMethod;

		if( ! png_get_IHDR( mPngPtr, mInfoPtr, &width, &height, &bitDepth, &colorType, &interlaceType, &compressionType, &filterMethod ) ) {
			png_destroy_read_struct( &mPngPtr, &mInfoPtr, (png_infopp)NULL );
			mPngPtr = 0;
			return false;
		}

		setSize( width, height );
		setDataType( ( bitDepth == 16 ) ? ImageIo::UINT16 : ImageIo::UINT8 );
		
	#ifdef CINDER_LITTLE_ENDIAN
		png_set_swap( mPngPtr );
	#endif

		switch( colorType ) {
			case PNG_COLOR_TYPE_GRAY:
				setColorModel( ImageIo::CM_GRAY );
				setChannelOrder( ImageIo::Y );			
			break;
			case PNG_COLOR_TYPE_GRAY_ALPHA:
				setColorModel( ImageIo::CM_GRAY );
				setChannelOrder( ImageIo::YA );
			break;
			case PNG_COLOR_TYPE_RGB:
			case PNG_COLOR_TYPE_PALETTE:
				setColorModel( ImageIo::CM_RGB );
				setChannelOrder( ImageIo::RGB );
			break;
			case PNG_COLOR_TYPE_RGB_ALPHA:
				setColorModel( ImageIo::CM_RGB );
				setChannelOrder( ImageIo::RGBA );
			break;
			default:
				throw ImageSourcePngException( "Unexpected png color type." );
		}	

		png_set_expand_gray_1_2_4_to_8( mPngPtr );
		png_set_palette_to_rgb( mPngPtr );
		png_set_tRNS_to_alpha( mPngPtr );
		
		png_read_update_info( mPngPtr, mInfoPtr );
	}
	
	return success;
}

ImageSourcePng::~ImageSourcePng()
{
	if( mPngPtr )
		png_destroy_read_struct( &mPngPtr, &mInfoPtr, NULL );
}

void ImageSourcePng::load( ImageTargetRef target )
{
	bool success = true;
	if( setjmp( png_jmpbuf(mPngPtr) ) ) {
		png_destroy_read_struct( &mPngPtr, &mInfoPtr, (png_infopp)NULL );
		mPngPtr = 0;
		success = false;
	}
	else {
		// get a pointer to the ImageSource function appropriate for handling our data configuration
		ImageSource::RowFunc func = setupRowFunc( target );
		//int number_passes = png_set_interlace_handling( mPngPtr );
		unique_ptr<png_byte[]> row_pointer( new png_byte[png_get_rowbytes( mPngPtr, mInfoPtr )] );
		for( int32_t row = 0; row < mHeight; ++row ) {
			png_read_row( mPngPtr, row_pointer.get(), NULL );
			((*this).*func)( target, row, row_pointer.get() );
		}
	}
	
	if( ! success )
		throw ImageSourcePngException( "Failure during load." );
}

} // namespace cinder