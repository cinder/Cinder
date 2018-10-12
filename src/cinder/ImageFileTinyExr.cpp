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
// ImageSourceFileTinyExr
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

ImageSourceFileTinyExr::ImageSourceFileTinyExr( DataSourceRef dataSource, ImageSource::Options /*options*/ )
	: mExrHeader( new EXRHeader, FreeEXRHeader ) // We're using the provided FreeEXRHeader function as a custom deleter
	, mExrImage( new EXRImage, FreeEXRImage )    // We're using the provided FreeEXRImage function as a custom deleter
{
	InitEXRHeader( mExrHeader.get() );
	InitEXRImage( mExrImage.get() );

	int status = 0;

	EXRVersion version;
	status = ParseEXRVersionFromFile( &version, dataSource->getFilePath().string().c_str() );
	if( status != TINYEXR_SUCCESS )
		throw ImageIoExceptionFailedLoadTinyExr( string( "Failed to parse OpenEXR version" ) );

	if( version.multipart || version.non_image )
		throw ImageIoExceptionFailedLoadTinyExr( string( "Multipart or DeepImage EXR's are not supported yet" ) );

	if( dataSource->isFilePath() ) {
		const string filename = dataSource->getFilePath().string();

		const char *error;
		status = ParseEXRHeaderFromFile( mExrHeader.get(), &version, filename.c_str(), &error );
		if( status != TINYEXR_SUCCESS )
			throw ImageIoExceptionFailedLoadTinyExr( string( "Failed to parse OpenEXR header; Error message: " ) + error );

		status = LoadEXRImageFromFile( mExrImage.get(), mExrHeader.get(), filename.c_str(), &error );
		if( status != TINYEXR_SUCCESS )
			throw ImageIoExceptionFailedLoadTinyExr( string( "Failed to parse OpenEXR file; Error message: " ) + error );
	}
	else {
		const auto memory = static_cast<const unsigned char *>( dataSource->getBuffer()->getData() );

		const char *error;
		status = ParseEXRHeaderFromMemory( mExrHeader.get(), &version, memory, &error );
		if( status != TINYEXR_SUCCESS )
			throw ImageIoExceptionFailedLoadTinyExr( string( "Failed to parse OpenEXR header; Error message: " ) + error );
		
		status = LoadEXRImageFromMemory( mExrImage.get(), mExrHeader.get(), memory, &error );
		if( status != TINYEXR_SUCCESS )
			throw ImageIoExceptionFailedLoadTinyExr( string( "Failed to parse OpenEXR file; Error message: " ) + error );
	}

	// verify that the channels are all the same size; currently we don't support variably sized channels
	const int pixelType = mExrHeader->pixel_types[0];
	for( int c = 1; c < mExrHeader->num_channels; ++c ) {
		if( pixelType != mExrHeader->pixel_types[c] )
			throw ImageIoExceptionFailedLoadTinyExr( "TinyExr: heterogneous channel data types not supported" );
	}

	switch( pixelType ) {
		case TINYEXR_PIXELTYPE_HALF:
			setDataType( ImageIo::FLOAT16 );
			break;
		case TINYEXR_PIXELTYPE_FLOAT:
			setDataType( ImageIo::FLOAT32 );
			break;
		default:
			throw ImageIoExceptionFailedLoadTinyExr( "TinyExr: Unknown data type" );
			break;
	}

	setSize( mExrImage->width, mExrImage->height );

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
			throw ImageIoExceptionFailedLoadTinyExr( "TinyExr: Unsupported number of channels (" + to_string( mExrImage->num_channels ) + ")" );
	}
}

void ImageSourceFileTinyExr::load( ImageTargetRef target )
{
	ImageSource::RowFunc rowFunc = setupRowFunc( target );

	const size_t numChannels = mExrHeader->num_channels;
	const void * red = nullptr, *green = nullptr, *blue = nullptr, *alpha = nullptr;

	for( size_t c = 0; c < numChannels; ++c ) {
		if( strcmp( mExrHeader->channels[c].name, "R" ) == 0 )
			red = mExrImage->images[c];
		else if( strcmp( mExrHeader->channels[c].name, "G" ) == 0 )
			green = mExrImage->images[c];
		else if( strcmp( mExrHeader->channels[c].name, "B" ) == 0 )
			blue = mExrImage->images[c];
		else if( strcmp( mExrHeader->channels[c].name, "A" ) == 0 )
			alpha = mExrImage->images[c];
	}

	if( ( !red ) || ( !green ) || ( !blue ) )
		throw ImageIoExceptionFailedLoadTinyExr( "Unable to locate channels for RGB" );

	// load one interleaved row at a time
	if( getDataType() == ImageIo::FLOAT32 ) {
		vector<float> rowData( mWidth * mExrImage->num_channels, 0 );
		for( int32_t row = 0; row < mHeight; row++ ) {
			for( int32_t col = 0; col < mWidth; col++ ) {
				rowData.at( col * numChannels + 0 ) = static_cast<const float *>( red )[row * mWidth + col];
				rowData.at( col * numChannels + 1 ) = static_cast<const float *>( green )[row * mWidth + col];
				rowData.at( col * numChannels + 2 ) = static_cast<const float *>( blue )[row * mWidth + col];
				if( alpha )
					rowData.at( col * numChannels + 3 ) = static_cast<const float *>( alpha )[row * mWidth + col];
			}

			( ( *this ).*rowFunc )( target, row, rowData.data() );
		}
	}
	else { // float16
		vector<uint16_t> rowData( mWidth * mExrImage->num_channels, 0 );
		for( int32_t row = 0; row < mHeight; row++ ) {
			for( int32_t col = 0; col < mWidth; col++ ) {
				rowData.at( col * numChannels + 0 ) = static_cast<const uint16_t *>( red )[row * mWidth + col];
				rowData.at( col * numChannels + 1 ) = static_cast<const uint16_t *>( green )[row * mWidth + col];
				rowData.at( col * numChannels + 2 ) = static_cast<const uint16_t *>( blue )[row * mWidth + col];
				if( alpha )
					rowData.at( col * numChannels + 3 ) = static_cast<const uint16_t *>( alpha )[row * mWidth + col];
			}

			( ( *this ).*rowFunc )( target, row, rowData.data() );
		}
	}
}

// ----------------------------------------------------------------------------------------------------
// ImageTargetFileTinyExr
// ----------------------------------------------------------------------------------------------------

void ImageTargetFileTinyExr::registerSelf()
{
	static bool   sRegistered = false;
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

ImageTargetFileTinyExr::ImageTargetFileTinyExr( DataTargetRef dataTarget, ImageSourceRef imageSource, ImageTarget::Options options, const std::string & /*extensionData*/ )
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
			if( mNumComponents == 3 )
				mChannelNames = { "B", "G", "R" };
			else
				mChannelNames = { "A", "B", "G", "R" };
			break;
		case ImageIo::ColorModel::CM_GRAY:
			mNumComponents = ( imageSource->hasAlpha() ) ? 2 : 1;
			setColorModel( ImageIo::ColorModel::CM_GRAY );
			setChannelOrder( ( mNumComponents == 2 ) ? ImageIo::ChannelOrder::YA : ImageIo::ChannelOrder::Y );
			if( mNumComponents == 2 )
				mChannelNames = { "Y", "A" };
			else
				mChannelNames = { "Y" };
			break;
		default:
			throw ImageIoExceptionIllegalColorModel();
	}

	// TODO: consider supporting half float and uint types as well
	setDataType( ImageIo::DataType::FLOAT32 );
	mData.resize( mHeight * mWidth * mNumComponents );
}

void *ImageTargetFileTinyExr::getRowPointer( int32_t row )
{
	return &mData[row * mWidth * mNumComponents];
}

void ImageTargetFileTinyExr::finalize()
{
	// turn interleaved data into a series of planar channels
	vector<Channel32f> channels;
	unsigned char *    imagePtr[4];
	for( int c = 0; c < mNumComponents; ++c ) {
		channels.emplace_back( getWidth(), getHeight() );
		Channel32f srcChannel( getWidth(), getHeight(), mNumComponents * mWidth * sizeof(float), mNumComponents, mData.data() + c );
		channels.back().copyFrom( srcChannel, srcChannel.getBounds() );

		imagePtr[c] = reinterpret_cast<unsigned char *>( channels[c].getData() );
	}

	int pixelTypes[4], requested_pixel_types[4];
	for( int i = 0; i < mNumComponents; i++ ) {
		pixelTypes[i] = TINYEXR_PIXELTYPE_FLOAT;
		requested_pixel_types[i] = TINYEXR_PIXELTYPE_HALF; // export as half-floats to reduce file size
	}

	std::vector<EXRChannelInfo> info( mNumComponents );

	// create image descriptor
	EXRImage exrImage;
	InitEXRImage( &exrImage );

	exrImage.num_channels = mNumComponents;
	exrImage.width = mWidth;
	exrImage.height = mHeight;
	exrImage.images = imagePtr;

	// create header descriptor
	EXRHeader exrHeader;
	InitEXRHeader( &exrHeader );

	exrHeader.num_channels = mNumComponents;
	exrHeader.channels = info.data();
	for( int i = 0; i < mNumComponents; ++i ) {
		strncpy( exrHeader.channels[i].name, mChannelNames[i].data(), mChannelNames[i].size() );
	}
	exrHeader.pixel_types = pixelTypes;
	exrHeader.requested_pixel_types = requested_pixel_types;
	exrHeader.compression_type = TINYEXR_COMPRESSIONTYPE_NONE; // TODO add option to enable compression

	const char *error;

	const int status = SaveEXRImageToFile( &exrImage, &exrHeader, mFilePath.string().c_str(), &error );
	if( status != TINYEXR_SUCCESS )
		throw ImageIoExceptionFailedWriteTinyExr( string( "TinyExr: failed to write. Error: " ) + error );

	// Note: since header and image descriptors do not own any data, we explicitely do not call FreeEXRHeader and FreeEXRImage here!
}

} // namespace cinder
