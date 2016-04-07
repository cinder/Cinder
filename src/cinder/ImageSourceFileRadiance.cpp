/*
 Copyright (c) 2014, The Cinder Project, All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Significant portions copyright Igor Kravtchenko.

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

#include "cinder/ImageSourceFileRadiance.h"
#include "cinder/Stream.h"

namespace cinder {

ImageSourceRef ImageSourceFileRadiance::create( DataSourceRef dataSourceRef, ImageSource::Options options )
{
	return ImageSourceRef( new ImageSourceFileRadiance( dataSourceRef, options ) );
}

void ImageSourceFileRadiance::load( ImageTargetRef target )
{
	// get a pointer to the ImageSource function appropriate for handling our data configuration
	ImageSource::RowFunc func = setupRowFunc( target );
	//int number_passes = png_set_interlace_handling( mPngPtr );
	for( int32_t row = 0; row < mHeight; ++row ) {
		((*this).*func)( target, row, mRgbData.get() + ( row * mWidth * 3 ) );
	}
}

void ImageSourceFileRadiance::registerSelf()
{
	ImageIoRegistrar::SourceCreationFunc sourceFunc = ImageSourceFileRadiance::create;
	ImageIoRegistrar::registerSourceType( "hdr", sourceFunc, 1 );
}

ImageSourceFileRadiance::ImageSourceFileRadiance( DataSourceRef dataSourceRef, ImageSource::Options options )
{
	IStreamRef stream = dataSourceRef->createStream();
	
	loadStream( stream );
}

namespace {
typedef unsigned char RgbePixel[4];

const int MINELEN =	8;				// minimum scanline length for encoding
const int MAXELEN = 0x7fff;			// maximum scanline length for encoding

enum { R, G, B, E };

void workOnRgbeScanline( RgbePixel *scan, int len, float *cols );
bool decrunchScanline( RgbePixel *scanline, int len, IStreamCinder *stream );
bool oldStyleDecrunch( RgbePixel *scanline, int len, IStreamCinder *stream );
}

void ImageSourceFileRadiance::loadStream( IStreamRef stream )
{
	setDataType( ImageIo::FLOAT32 );
	setColorModel( ImageIo::CM_RGB );
	setChannelOrder( ImageIo::RGB );

	char str[200];
	stream->readData( str, 10 );
	if( memcmp( str, "#?RADIANCE", 10 ) )
		throw ImageSourceFileRadianceException( "Invalid header" );

	stream->seekRelative( 1 );

	char cmd[200];
	int i = 0;
	char c = 0, oldc;
	while( true ) {
		oldc = c;
		stream->read( &c );
		if( c == 0xa && oldc == 0xa )
			break;
		cmd[i++] = c;
	}

	char resolution[200];
	i = 0;
	while( true ) {
		stream->read( &c );
		resolution[i++] = c;
		if( c == 0xa )
			break;
	}

	int width, height;
#if defined( CINDER_UWP )
	if( ! sscanf_s( resolution, "-Y %d +X %d", &height, &width ) )
#else
	if( ! sscanf( resolution, "-Y %d +X %d", &height, &width ) )
#endif
		throw ImageSourceFileRadianceException( "Unable to parse size" );
	setSize( width, height );

	mRgbData = std::unique_ptr<float[]>( new float[width * height * 3] );
	std::unique_ptr<RgbePixel[]> scanline( new RgbePixel[width] );

	// convert image
	float *cols = mRgbData.get();
	for( int y = height - 1; y >= 0; y-- ) {
		if( ! decrunchScanline( scanline.get(), width, stream.get() ) )
			break;
		workOnRgbeScanline( scanline.get(), width, cols );
		cols += width * 3;
	}
}

namespace {
// this should be optimized
float convertComponent( int expo, int val )
{
	float v = val / 256.0f;
	float d = powf( 2.0f, expo );
	return v * d;
}

void workOnRgbeScanline( RgbePixel *scan, int len, float *cols )
{
	while( len-- > 0 ) {
		int expo = scan[0][E] - 128;
		cols[0] = convertComponent( expo, scan[0][R] );
		cols[1] = convertComponent( expo, scan[0][G] );
		cols[2] = convertComponent( expo, scan[0][B] );
		cols += 3;
		scan++;
	}
}

bool decrunchScanline( RgbePixel *scanline, int len, IStreamCinder *stream )
{
	char i;

	if( len < MINELEN || len > MAXELEN )
		return oldStyleDecrunch(scanline, len, stream );

	stream->read( &i );
	if( i != 2 ) { // old style
		stream->seekRelative( -1 );
		return oldStyleDecrunch( scanline, len, stream );
	}

	stream->read( &scanline[0][G] );
	stream->read( &scanline[0][B] );
	stream->read( &i );

	if( scanline[0][G] != 2 || scanline[0][B] & 128 ) {
		scanline[0][R] = 2;
		scanline[0][E] = i;
		return oldStyleDecrunch( scanline + 1, len - 1, stream );
	}

	// read each component
	for( i = 0; i < 4; i++ ) {
	    for( int j = 0; j < len; ) {
			unsigned char code;
			stream->read( &code );
			if( code > 128 ) { // run
			    code &= 127;
			    unsigned char val;
				stream->read( &val );
			    while( code-- )
					scanline[j++][i] = val;
			}
			else {	// non-run
				while( code-- )
					stream->read( &scanline[j++][i] );
			}
		}
    }

	return ! stream->isEof();
}

bool oldStyleDecrunch( RgbePixel *scanline, int len, IStreamCinder *stream )
{
	int i;
	int rshift = 0;
	
	while( len > 0 ) {
		stream->read( &scanline[0][R] );
		stream->read( &scanline[0][G] );
		stream->read( &scanline[0][B] );
		stream->read( &scanline[0][E] );
		if( stream->isEof() )
			return false;

		if( scanline[0][R] == 1 &&
			scanline[0][G] == 1 &&
			scanline[0][B] == 1) {
			for( i = scanline[0][E] << rshift; i > 0; i-- ) {
				memcpy( &scanline[0][0], &scanline[-1][0], 4 );
				scanline++;
				len--;
			}
			rshift += 8;
		}
		else {
			scanline++;
			len--;
			rshift = 0;
		}
	}
	
	return true;
}
} // anonymous namespace

} // namespace cinder