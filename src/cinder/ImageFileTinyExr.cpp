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

#include "cinder/ImageFileTinyExr.h"
#include "cinder/Log.h"

#include "tinyexr/tinyexr.h"

using namespace std;

namespace cinder {

// ----------------------------------------------------------------------------------------------------
// MARK: - ImageSourceFileTinyExr
// ----------------------------------------------------------------------------------------------------

ImageSourceRef ImageSourceFileTinyExr::create( DataSourceRef dataSourceRef, ImageSource::Options options )
{
	return ImageSourceRef( new ImageSourceFileTinyExr( dataSourceRef, options ) );
}

void ImageSourceFileTinyExr::registerSelf()
{
	ImageIoRegistrar::SourceCreationFunc sourceFunc = ImageSourceFileTinyExr::create;
	ImageIoRegistrar::registerSourceType( "exr", sourceFunc, 1 ); // lower is higher priority
}

ImageSourceFileTinyExr::ImageSourceFileTinyExr( DataSourceRef dataSource, ImageSource::Options options )
{
	mExrImage.reset( new EXRImage );
	const char *error;

	if( dataSource->isFilePath() ) {
		int status = LoadMultiChannelEXRFromFile( mExrImage.get(), dataSource->getFilePath().string().c_str(), &error );
		if( status != 0 )
			throw ImageIoExceptionFailedLoadTinyExr( string( "Failed to read .exr file at path: " ) + dataSource->getFilePath().string() + ", error message: " + error );
	}
	else {
		throw ImageIoExceptionFailedLoadTinyExr( "unimplemented" );
		// TODO: use LoadMultiChannelEXRFromMemory
	}

	// verify that the channels are all the same size; currently we don't support variably sized channels
	int pixelType = mExrImage->pixel_types[0];
	for( int c = 0; c < mExrImage->num_channels; ++c ) {
		if( pixelType != mExrImage->pixel_types[c] )
			throw ImageIoExceptionFailedLoadTinyExr( string( "Failed to read .exr file at path: " ) + dataSource->getFilePath().string() + ", heterogneous channel data types not supported" );
	}

pixelType = TINYEXR_PIXELTYPE_FLOAT;
	switch( pixelType ) {
		case TINYEXR_PIXELTYPE_HALF:
			setDataType( ImageIo::FLOAT16 );
		break;
		case TINYEXR_PIXELTYPE_FLOAT:
			setDataType( ImageIo::FLOAT32 );
		break;
		default:
			throw ImageIoExceptionFailedLoadTinyExr( string( "Failed to read .exr file at path: " ) + dataSource->getFilePath().string() + ", unknown data type" );
		break;
	}

	setSize( mExrImage->width, mExrImage->height );

	CI_LOG_I( "width: " << mExrImage->width << ", height: " << mExrImage->height << ", num_channels: " << mExrImage->num_channels );
	for( int i = 0; i < mExrImage->num_channels; i++ ) {
		CI_LOG_I( "channel: " << i << ", pixel type: " << mExrImage->pixel_types[i] << ", name: " << mExrImage->channel_names[i] );
	}

	switch( mExrImage->num_channels ) {
		case 3:
			setColorModel( ImageIo::CM_RGB );
			setChannelOrder( ImageIo::ChannelOrder::RGB );
		break;
		case 4:
			setColorModel( ImageIo::CM_RGB );
			setChannelOrder( ImageIo::ChannelOrder::RGBA );
		break;
		default:
			throw ImageIoExceptionFailedLoadTinyExr( "unsupported number of channels (" + to_string( mExrImage->num_channels ) + ")" );
	}
}

void ImageSourceFileTinyExr::load( ImageTargetRef target )
{
	ImageSource::RowFunc rowFunc = setupRowFunc( target );

	const size_t numChannels = mExrImage->num_channels;

	// TODO: need a more robust way to map these channels, I'm looking at the name in the debug output
	// - though, so far they all appear to be in this order
	const void *red = nullptr, *green = nullptr, *blue = nullptr, *alpha = nullptr;
	
	for( size_t c = 0; c < numChannels; ++c ) {
		if( strcmp( mExrImage->channel_names[c], "R" ) == 0 )
			red = mExrImage->images[c];
		else if( strcmp( mExrImage->channel_names[c], "G" ) == 0 )
			green = mExrImage->images[c];
		else if( strcmp( mExrImage->channel_names[c], "B" ) == 0 )
			blue = mExrImage->images[c];
		else if( strcmp( mExrImage->channel_names[c], "A" ) == 0 )
			alpha = mExrImage->images[c];
	}

	if( ( ! red ) || ( ! green ) || ( ! blue ) )
		throw ImageIoExceptionFailedLoadTinyExr( "Unable to locate channels for RGB" );
	
	// load one interleaved row at a time
	if( true/*getDataType() == ImageIo::FLOAT32*/ ) {
		vector<float> rowData( mWidth * mExrImage->num_channels, 0 );
		for( int32_t row = 0; row < mHeight; row++ ) {
			for( int32_t col = 0; col < mWidth; col++ ) {
				rowData.at( col * numChannels + 0 ) = ((float*)red)[row * mWidth + col];
				rowData.at( col * numChannels + 1 ) = ((float*)green)[row * mWidth + col];
				rowData.at( col * numChannels + 2 ) = ((float*)blue)[row * mWidth + col];
				if( alpha )
					rowData.at( col * numChannels + 3 ) = ((float*)alpha)[row * mWidth + col];
			}

std:cout << vec3( rowData[0], rowData[1], rowData[2] ) << std::endl;

			((*this).*rowFunc)( target, row, rowData.data() );
		}
	}
	else { // float16
		vector<uint16_t> rowData( mWidth * mExrImage->num_channels, 0 );
		for( int32_t row = 0; row < mHeight; row++ ) {
			for( int32_t col = 0; col < mWidth; col++ ) {
				rowData.at( col * numChannels + 0 ) = ((uint16_t*)red)[row * mWidth + col];
				rowData.at( col * numChannels + 1 ) = ((uint16_t*)green)[row * mWidth + col];
				rowData.at( col * numChannels + 2 ) = ((uint16_t*)blue)[row * mWidth + col];
				if( alpha )
					rowData.at( col * numChannels + 3 ) = ((uint16_t*)alpha)[row * mWidth + col];
			}

			((*this).*rowFunc)( target, row, rowData.data() );
		}
	}
}

// ----------------------------------------------------------------------------------------------------
// MARK: - ImageTargetFileTinyExr
// ----------------------------------------------------------------------------------------------------

void ImageTargetFileTinyExr::registerSelf()
{
	static bool sRegistered = false;
	const int32_t PRIORITY = 1;
	
	if( sRegistered )
		return;

	sRegistered = true;

	ImageIoRegistrar::TargetCreationFunc func = ImageTargetFileTinyExr::create;
	ImageIoRegistrar::registerTargetType( "exr", func, PRIORITY, "exr" );
}

ImageTargetRef ImageTargetFileTinyExr::create( DataTargetRef dataTarget, ImageSourceRef imageSource, ImageTarget::Options options, const std::string &extensionData )
{
	return ImageTargetRef( new ImageTargetFileTinyExr( dataTarget, imageSource, options, extensionData ) );
}

ImageTargetFileTinyExr::ImageTargetFileTinyExr( DataTargetRef dataTarget, ImageSourceRef imageSource, ImageTarget::Options options, const std::string &extensionData )
{
	if( ! dataTarget->providesFilePath() ) {
		throw ImageIoExceptionFailedWrite( "ImageTargetFileTinyExr only supports writing to files." );
	}

	mFilePath = dataTarget->getFilePath();

	setSize( imageSource->getWidth(), imageSource->getHeight() );
	ImageIo::ColorModel cm = options.isColorModelDefault() ? imageSource->getColorModel() : options.getColorModel();
	
	switch( cm ) {
		case ImageIo::ColorModel::CM_RGB:
			mNumComponents = ( imageSource->hasAlpha() ) ? 4 : 3;
			setColorModel( ImageIo::ColorModel::CM_RGB );
			setChannelOrder( ( mNumComponents == 3 ) ? ImageIo::ChannelOrder::BGR : ImageIo::ChannelOrder::ABGR );
		break;
		case ImageIo::ColorModel::CM_GRAY:
			mNumComponents = ( imageSource->hasAlpha() ) ? 2 : 1;
			setColorModel( ImageIo::ColorModel::CM_GRAY );
			setChannelOrder( ( mNumComponents == 2 ) ? ImageIo::ChannelOrder::YA : ImageIo::ChannelOrder::Y );
		break;
		default:
			throw ImageIoExceptionIllegalColorModel();
	}

	// TODO: consider supporting half float and uint types as well
	setDataType( ImageIo::DataType::FLOAT32 );
	mRowBytes = mNumComponents * imageSource->getWidth() * sizeof( float );

	mData.resize( mHeight * mRowBytes );

#if USE_PLANAR_CHANNELS
	mImagePlanar.reserve( mNumComponents );
	for( size_t i = 0; i < mNumComponents; i++ ) {
		mImagePlanar.emplace_back( mWidth, mHeight );
	}

#else
	mImagePlanar.resize( mNumComponents );
	for( auto &component : mImagePlanar ) {
		component.resize( mWidth * mHeight );
	}

#endif

	CI_LOG_I( "num channels: " << (int)mNumComponents );
}

void* ImageTargetFileTinyExr::getRowPointer( int32_t row )
{
	return &mData[row * mRowBytes];
}

void ImageTargetFileTinyExr::finalize()
{
#if USE_PLANAR_CHANNELS
	// construct a surface (doesn't own data) that allows us to easily pull out the planar channels
	Surface32f surface;
	if( mNumComponents == 3 ) {
		surface = Surface32f( (float*)mData.data(), mWidth, mHeight, (int32_t)mRowBytes, SurfaceChannelOrder::BGR );
		mImagePlanar[0] = surface.getChannelBlue();
		mImagePlanar[1] = surface.getChannelGreen();
		mImagePlanar[2] = surface.getChannelRed();

		mChannelNames = { "G", "B", "R" };
	}
	else if( mNumComponents == 4 ) {
		surface = Surface32f( (float*)mData.data(), mWidth, mHeight, (int32_t)mRowBytes, SurfaceChannelOrder::ABGR );
		mImagePlanar[0] = surface.getChannelAlpha();
		mImagePlanar[1] = surface.getChannelBlue();
		mImagePlanar[2] = surface.getChannelGreen();
		mImagePlanar[3] = surface.getChannelRed();

		mChannelNames = { "A", "G", "B", "R" };
	}
	else {
		throw ImageIoExceptionFailedWriteTinyExr( "TODO: finish implementing, not supported for channel count: " + to_string( mNumComponents ) );
	}
#else
	// de-interleave to planar channels
	for( size_t i = 0; i < mNumComponents; i++ ) {
		auto &component = mImagePlanar[i];
		for( size_t x = 0; x < mWidth * mHeight; x++ ) {
			component[x] = mData.at( x * mNumComponents + i );
		}
	}

	if( mNumComponents == 3 ) {
		mChannelNames = { "G", "B", "R" };
	}
	else if( mNumComponents == 4 ) {
		mChannelNames = { "A", "G", "B", "R" };
	}
	else {
		throw ImageIoExceptionFailedWriteTinyExr( "TODO: finish implementing, not supported for channel count: " + to_string( mNumComponents ) );
	}
#endif

	EXRImage exrImage = {}; // FIXME: why do I need to default initialize this?
	exrImage.num_channels = mNumComponents;
	exrImage.width = mWidth;
	exrImage.height = mHeight;

	const char *channelNames[4];
	float* imagePtr[4];
	int pixelTypes[4];
	for( int i = 0; i < exrImage.num_channels; i++ ) {
		pixelTypes[i] = TINYEXR_PIXELTYPE_FLOAT; // TODO: try changing this to TINYEXR_PIXELTYPE_HALF, I think tinyexr will do the conversion
//		pixelTypes[i] = TINYEXR_PIXELTYPE_HALF;
		channelNames[i] = mChannelNames[i].c_str();

#if USE_PLANAR_CHANNELS
		imagePtr[i] = mImagePlanar[i].getData();
#else
		imagePtr[i] = mImagePlanar[i].data();
#endif
	}

	exrImage.channel_names = channelNames;
	exrImage.images = (unsigned char **)imagePtr;
	exrImage.pixel_types = pixelTypes;

	const char *error;
	int status = SaveMultiChannelEXRToFile( &exrImage, mFilePath.string().c_str(), &error );
	if( status != 0 ) {
		throw ImageIoExceptionFailedWriteTinyExr( string( "Failed to write .exr file to path: " ) + mFilePath.string() + ", error message: " + error );
	}
}

} // namespace cinder