/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.

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

#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"

#include <boost/utility.hpp>
#include <boost/type_traits/is_same.hpp>
#include <cctype>

#if defined( CINDER_COCOA )
	#include "cinder/cocoa/CinderCocoa.h"
#elif defined( CINDER_UWP )
	#include <ppltasks.h>
	#include "cinder/winrt/WinRTUtils.h"
	#include "cinder/Utilities.h"
	#include "cinder/msw/CinderMsw.h"
	using namespace Windows::Storage;
	using namespace Concurrency;
#elif defined( CINDER_ANDROID )
	#include "cinder/app/android/PlatformAndroid.h"	
#endif

using namespace std;

namespace cinder {


///////////////////////////////////////////////////////////////////////////////
// ImageSource
ImageIo::ImageIo()
	: mColorModel( CM_UNKNOWN ), mDataType( DATA_UNKNOWN ), mChannelOrder( CUSTOM )
{
}

void ImageIo::translateRgbColorModelToOffsets( ChannelOrder channelOrder, int8_t *red, int8_t *green, int8_t *blue, int8_t *alpha, int8_t *inc )
{
	switch( channelOrder ) {
		case RGBA:	*red = 0; *green = 1; *blue = 2; *alpha = 3;	*inc = 4;	break;
		case BGRA:	*red = 2; *green = 1; *blue = 0; *alpha = 3;	*inc = 4;	break;
		case ARGB:	*red = 1; *green = 2; *blue = 3; *alpha = 0;	*inc = 4;	break;
		case ABGR:	*red = 3; *green = 2; *blue = 1; *alpha = 0;	*inc = 4;	break;
		case RGBX:	*red = 0; *green = 1; *blue = 2; *alpha = -1;	*inc = 4;	break;
		case BGRX:	*red = 2; *green = 1; *blue = 0; *alpha = -1;	*inc = 4;	break;
		case XRGB:	*red = 1; *green = 2; *blue = 3; *alpha = -1;	*inc = 4;	break;
		case XBGR:	*red = 3; *green = 2; *blue = 1; *alpha = -1;	*inc = 4;	break;
		case RGB:	*red = 0; *green = 1; *blue = 2; *alpha = -1;	*inc = 3;	break;
		case BGR:	*red = 2; *green = 1; *blue = 0; *alpha = -1;	*inc = 3;	break;
		default:
			throw ImageIoExceptionIllegalChannelOrder( "Unexpected channel order." );
	}
}

void ImageIo::translateGrayColorModelToOffsets( ChannelOrder channelOrder, int8_t *gray, int8_t *alpha, int8_t *inc )
{
	switch( channelOrder ) {
		case Y:		*gray = 0;	*alpha = -1;	*inc = 1;	break;
		case YA:	*gray = 0;	*alpha = 1;		*inc = 2;	break;
		default:
			throw ImageIoExceptionIllegalChannelOrder( "Unexpected channel order." );
	}
}

int8_t ImageIo::channelOrderNumChannels( ChannelOrder channelOrder )
{
	switch( channelOrder ) {
		case RGBA:	return 4;	break;
		case BGRA:	return 4;	break;
		case ARGB:	return 4;	break;
		case ABGR:	return 4;	break;
		case RGBX:	return 4;	break;
		case BGRX:	return 4;	break;
		case XRGB:	return 4;	break;
		case XBGR:	return 4;	break;
		case RGB:	return 3;	break;
		case BGR:	return 3;	break;
		case Y:		return 1;	break;
		case YA:	return 2;	break;
		default:
			throw ImageIoExceptionIllegalChannelOrder( "Unexpected channel order." );
	}
}

bool ImageIo::channelOrderHasAlpha( ChannelOrder channelOrder )
{
	switch( channelOrder ) {
		case RGBA:
		case BGRA:
		case ARGB:
		case ABGR:
		case YA:
			return true;
		default:
			return false;
	}
}

uint8_t	ImageIo::dataTypeBytes( DataType dataType )
{
	switch( dataType ) {
		case UINT8: return 1;
		case UINT16: return 2;
		case FLOAT16: return 2;
		case FLOAT32: return 4;
		default:
			throw ImageIoExceptionIllegalDataType( "Unexpected data type." );
	}
}

vector<string> ImageIo::getLoadExtensions()
{
	vector<string> result;
	const map<string,multimap<int32_t,ImageIoRegistrar::SourceCreationFunc> > &sources( ImageIoRegistrar::instance()->mSources );
	for( map<string,multimap<int32_t,ImageIoRegistrar::SourceCreationFunc> >::const_iterator srcIt = sources.begin(); srcIt != sources.end(); ++srcIt )
		result.push_back( srcIt->first );

	return result;
}

vector<string> ImageIo::getWriteExtensions()
{
	vector<string> result;
	const map<string, multimap<int32_t,pair<ImageIoRegistrar::TargetCreationFunc,string> > > &targets( ImageIoRegistrar::instance()->mTargets ); 
	for( map<string, multimap<int32_t,pair<ImageIoRegistrar::TargetCreationFunc,string> > >::const_iterator targetIt = targets.begin(); targetIt != targets.end(); ++targetIt )
		result.push_back( targetIt->first );

	return result;
}


///////////////////////////////////////////////////////////////////////////////
// ImageSource
float ImageSource::getPixelAspectRatio() const
{
	return mPixelAspectRatio;
}

bool ImageSource::isPremultiplied() const
{
	return mIsPremultiplied;
}

size_t ImageSource::getRowBytes() const
{
	return getWidth() * ImageIo::channelOrderNumChannels( getChannelOrder() ) * ImageIo::dataTypeBytes( getDataType() );
}

/* SD - source data type, TD - target data type, TCM - target color model */
template<typename SD, typename TD, ImageIo::ColorModel TCM, bool ALPHA>
void ImageSource::rowFuncSourceRgb( ImageTargetRef target, int32_t row, const void *data )
{
	const SD *sourceData = reinterpret_cast<const SD*>( data );
	TD *targetData = reinterpret_cast<TD*>( target->getRowPointer( row ) );
	int32_t width = getWidth();
	
	if( TCM == CM_RGB ) {
		if( ALPHA ) {
			for( int32_t c = 0; c < width; c++ ) {
				targetData[mRowFuncTargetRed]	= CHANTRAIT<TD>::convert( sourceData[mRowFuncSourceRed] );
				targetData[mRowFuncTargetGreen]	= CHANTRAIT<TD>::convert( sourceData[mRowFuncSourceGreen] );
				targetData[mRowFuncTargetBlue]	= CHANTRAIT<TD>::convert( sourceData[mRowFuncSourceBlue] );
				targetData[mRowFuncTargetAlpha]	= CHANTRAIT<TD>::convert( sourceData[mRowFuncSourceAlpha] );
				targetData += mRowFuncTargetInc;
				sourceData += mRowFuncSourceInc;
			}
		}
		else {
			for( int32_t c = 0; c < width; c++ ) {
				targetData[mRowFuncTargetRed]	= CHANTRAIT<TD>::convert( sourceData[mRowFuncSourceRed] );
				targetData[mRowFuncTargetGreen]	= CHANTRAIT<TD>::convert( sourceData[mRowFuncSourceGreen] );
				targetData[mRowFuncTargetBlue]	= CHANTRAIT<TD>::convert( sourceData[mRowFuncSourceBlue] );
				targetData += mRowFuncTargetInc;
				sourceData += mRowFuncSourceInc;
			}			
		}		
	}
	else if( TCM == CM_GRAY ) {
		if( ALPHA ) {
			for( int32_t c = 0; c < width; c++ ) {
				targetData[mRowFuncTargetGray]	= CHANTRAIT<TD>::convert( CHANTRAIT<SD>::grayscale( sourceData[mRowFuncSourceRed], sourceData[mRowFuncSourceGreen], sourceData[mRowFuncSourceBlue] ) );
				targetData[mRowFuncTargetAlpha]	= CHANTRAIT<TD>::convert( sourceData[mRowFuncSourceAlpha] );
				targetData += mRowFuncTargetInc;
				sourceData += mRowFuncSourceInc;
			}
		}
		else {
			for( int32_t c = 0; c < width; c++ ) {
				targetData[mRowFuncTargetGray]	= CHANTRAIT<TD>::convert( CHANTRAIT<SD>::grayscale( sourceData[mRowFuncSourceRed], sourceData[mRowFuncSourceGreen], sourceData[mRowFuncSourceBlue] ) );
				targetData += mRowFuncTargetInc;
				sourceData += mRowFuncSourceInc;
			}			
		}		
	}
	else { // the target has asked for an unknown color space, so they can sort out conversion on their own
		target->setRow( row, data );
	}
}

/* SD - source data type, TD - target data type, TCM - target color model */
template<typename SD, typename TD, ImageIo::ColorModel TCM, bool ALPHA>
void ImageSource::rowFuncSourceGray( ImageTargetRef target, int32_t row, const void *data )
{
	const SD *sourceData = reinterpret_cast<const SD*>( data );
	TD *targetData = reinterpret_cast<TD*>( target->getRowPointer( row ) );
	int32_t width = getWidth();
	
	if( TCM == CM_RGB ) {
		if( ALPHA ) {
			for( int32_t c = 0; c < width; c++ ) {
				TD convertedData = CHANTRAIT<TD>::convert( sourceData[mRowFuncSourceGray] );
				targetData[mRowFuncTargetRed]	= convertedData;
				targetData[mRowFuncTargetGreen]	= convertedData;
				targetData[mRowFuncTargetBlue]	= convertedData;
				targetData[mRowFuncTargetAlpha]	= CHANTRAIT<TD>::convert( sourceData[mRowFuncSourceAlpha] );
				targetData += mRowFuncTargetInc;
				sourceData += mRowFuncSourceInc;
			}
		}
		else {
			for( int32_t c = 0; c < width; c++ ) {
				TD convertedData = CHANTRAIT<TD>::convert( sourceData[mRowFuncSourceGray] );
				targetData[mRowFuncTargetRed]	= convertedData;
				targetData[mRowFuncTargetGreen]	= convertedData;
				targetData[mRowFuncTargetBlue]	= convertedData;
				targetData += mRowFuncTargetInc;
				sourceData += mRowFuncSourceInc;
			}			
		}		
	}
	else if( TCM == CM_GRAY ) {
		if( ALPHA ) {
			for( int32_t c = 0; c < width; c++ ) {
				targetData[mRowFuncTargetGray]	= CHANTRAIT<TD>::convert( sourceData[mRowFuncTargetGray] );
				targetData[mRowFuncTargetAlpha]	= CHANTRAIT<TD>::convert( sourceData[mRowFuncSourceAlpha] );
				targetData += mRowFuncTargetInc;
				sourceData += mRowFuncSourceInc;
			}
		}
		else {
			for( int32_t c = 0; c < width; c++ ) {
				targetData[mRowFuncTargetGray]	= CHANTRAIT<TD>::convert( sourceData[mRowFuncTargetGray] );
				targetData += mRowFuncTargetInc;
				sourceData += mRowFuncSourceInc;
			}			
		}		
	}
	else { // the target has asked for an unknown color space, so they can sort out conversion on their own
		target->setRow( row, data );
	}
}

void ImageSource::setupRowFuncRgbSource( ImageTargetRef target )
{
	translateRgbColorModelToOffsets( mChannelOrder, &mRowFuncSourceRed, &mRowFuncSourceGreen, &mRowFuncSourceBlue, &mRowFuncSourceAlpha, &mRowFuncSourceInc );
	if( target->getColorModel() == CM_RGB )
		translateRgbColorModelToOffsets( target->getChannelOrder(), &mRowFuncTargetRed, &mRowFuncTargetGreen, &mRowFuncTargetBlue, &mRowFuncTargetAlpha, &mRowFuncTargetInc );	
	else
		translateGrayColorModelToOffsets( target->getChannelOrder(), &mRowFuncTargetGray, &mRowFuncTargetAlpha, &mRowFuncTargetInc );
}

void ImageSource::setupRowFuncGraySource( ImageTargetRef target )
{
	translateGrayColorModelToOffsets( mChannelOrder, &mRowFuncSourceGray, &mRowFuncSourceAlpha, &mRowFuncSourceInc );
	if( target->getColorModel() == CM_RGB )
		translateRgbColorModelToOffsets( target->getChannelOrder(), &mRowFuncTargetRed, &mRowFuncTargetGreen, &mRowFuncTargetBlue, &mRowFuncTargetAlpha, &mRowFuncTargetInc );	
	else
		translateGrayColorModelToOffsets( target->getChannelOrder(), &mRowFuncTargetGray, &mRowFuncTargetAlpha, &mRowFuncTargetInc );
}

template<typename SD, typename TD, ImageIo::ColorModel TCM>
ImageSource::RowFunc ImageSource::setupRowFuncForTypesAndTargetColorModel( ImageTargetRef target )
{
	switch( mColorModel ) {
		case CM_RGB: {
			setupRowFuncRgbSource( target );
			if( mCustomPixelInc != 0 )
				mRowFuncSourceInc = mCustomPixelInc;
			bool alpha = ( mRowFuncSourceAlpha != -1 ) && ( mRowFuncTargetAlpha != -1 );
			if( alpha )
				return &ImageSource::rowFuncSourceRgb<SD,TD,TCM,true>;
			else
				return &ImageSource::rowFuncSourceRgb<SD,TD,TCM,false>;
		}
		break;
		case CM_GRAY: {
			setupRowFuncGraySource( target );
			if( mCustomPixelInc != 0 )
				mRowFuncSourceInc = mCustomPixelInc;
			bool alpha = ( mRowFuncSourceAlpha != -1 ) && ( mRowFuncTargetAlpha != -1 );
			if( alpha )
				return &ImageSource::rowFuncSourceGray<SD,TD,TCM,true>;
			else
				return &ImageSource::rowFuncSourceGray<SD,TD,TCM,false>;
		}
		break;
		case CM_UNKNOWN:
		default:
			throw ImageIoExceptionIllegalColorModel( "Unknown color model." );
	}
}

template<typename SD, typename TD>
ImageSource::RowFunc ImageSource::setupRowFuncForTypes( ImageTargetRef target )
{
	switch( target->getColorModel() ) {
		case CM_RGB:
			return setupRowFuncForTypesAndTargetColorModel<SD,TD,CM_RGB>( target );
		break;
		case CM_GRAY:
			return setupRowFuncForTypesAndTargetColorModel<SD,TD,CM_GRAY>( target );
		break;
		case CM_UNKNOWN:
		default:
			throw ImageIoExceptionIllegalColorModel( "Unknown color model." );
	}
}

template<typename SD>
ImageSource::RowFunc ImageSource::setupRowFuncForSourceType( ImageTargetRef target )
{
	switch( target->getDataType() ) {
		case UINT8:
			return setupRowFuncForTypes<SD,uint8_t>( target );
		break;
		case UINT16:
			return setupRowFuncForTypes<SD,uint16_t>( target );
		break;
		case FLOAT16:
			return setupRowFuncForTypes<SD,half_float>( target );
		break;
		case FLOAT32:
			return setupRowFuncForTypes<SD,float>( target );
		break;
		case DATA_UNKNOWN:
		default:
			throw ImageIoExceptionIllegalDataType( "Unknown data type." );
	}
}

ImageSource::RowFunc ImageSource::setupRowFunc( ImageTargetRef target )
{
	switch( mDataType ) {
		case UINT8:
			return setupRowFuncForSourceType<uint8_t>( target );
		break;
		case UINT16:
			return setupRowFuncForSourceType<uint16_t>( target );
		break;
		case FLOAT16:
			return setupRowFuncForSourceType<half_float>( target );
		break;
		case FLOAT32:
			return setupRowFuncForSourceType<float>( target );
		break;
		case DATA_UNKNOWN:
		default:
			throw ImageIoExceptionIllegalDataType( "Unknown data type." );
	}
}


#if defined( CINDER_UWP )
void loadImageAsync(const fs::path path, std::function<void (ImageSourceRef)> callback, ImageSource::Options options, std::string extension)
{
	auto loadImageTask = create_task([path, options, extension]() -> ImageSourceRef
	{
		return loadImage(path, options, extension);
	});

    auto c2 = loadImageTask.then([path, options, extension, callback](task<ImageSourceRef> previousTask)
    {
        // If we get an ImageIoExceptionFailedLoad, we try to copy the image to the Application temp folder and try again
        try
        {
			// Image was loaded. This callback is on the main UI thread
			callback(previousTask.get());
        }
        catch (const ImageIoExceptionFailedLoad&)
        {
			auto copyTask = winrt::copyFileToTempDirAsync(path);
			copyTask.then([options, extension, callback](StorageFile^ file) 
			{
				fs::path temp = fs::path( msw::toUtf8String( file->Path->Data() ) );
				// Image was loaded. This callback is on the main UI thread
				callback(loadImage(temp, options, extension));
				winrt::deleteFileAsync(temp);
			});
        }
    });
}
#endif

///////////////////////////////////////////////////////////////////////////////
ImageSourceRef loadImage( const fs::path &path, ImageSource::Options options, string extension )
{
#if defined( CINDER_ANDROID )
	if( ci::app::PlatformAndroid::isAssetPath( path ) ) {
		return loadImage( (DataSourceRef)DataSourceAndroidAsset::create( path ), options, extension );			
	}
	else {
		return loadImage( (DataSourceRef)DataSourcePath::create( path ), options, extension );
	}
#else	
	return loadImage( (DataSourceRef)DataSourcePath::create( path ), options, extension );
#endif	
}

ImageSourceRef loadImage( DataSourceRef dataSource, ImageSource::Options options, string extension )
{
#if defined( CINDER_COCOA )
	cocoa::SafeNsAutoreleasePool autorelease;
#endif

	if( extension.empty() ) // this is necessary to limit the lifetime of the objc-based loader's allocations
#if ! defined( CINDER_UWP ) || ( _MSC_VER > 1800 )
		extension = dataSource->getFilePathHint().extension().string();
#else
		extension = dataSource->getFilePathHint().extension();
#endif	
	return ImageIoRegistrar::createSource( dataSource, options, extension );
}

void writeImage( const fs::path &path, const ImageSourceRef &imageSource, ImageTarget::Options options, std::string extension )
{
	writeImage( (DataTargetRef)writeFile( path ), imageSource, options, extension );
}

void writeImage( DataTargetRef dataTarget, const ImageSourceRef &imageSource, ImageTarget::Options options, string extension )
{
#if defined( CINDER_COCOA ) // this is necessary to limit the lifetime of the objc-based loader's allocations
	cocoa::SafeNsAutoreleasePool autorelease;
#endif

	if( extension.empty() ) {
#if ! defined( CINDER_UWP ) || ( _MSC_VER > 1800 )
		extension = dataTarget->getFilePathHint().extension().string();
#else
		extension = dataTarget->getFilePathHint().extension();
#endif
		// strip leading .
		extension = ( ( ! extension.empty() ) && ( extension[0] == '.' ) ) ? extension.substr( 1, string::npos ) : extension;
	}

	ImageTargetRef imageTarget = ImageIoRegistrar::createTarget( dataTarget, imageSource, options, extension );
	if( imageTarget ) {
		writeImage( imageTarget, imageSource );
	}
	else
		throw ImageIoExceptionUnknownExtension( "Could not create target for image with extension: " + extension );
}

void writeImage( ImageTargetRef imageTarget, const ImageSourceRef &imageSource )
{
	imageSource->load( imageTarget );
	imageTarget->finalize();
}

///////////////////////////////////////////////////////////////////////////////
ImageIoRegistrar::Inst* ImageIoRegistrar::instance()
{
	static shared_ptr<Inst> sInst;
	if( ! sInst ) {
		sInst = shared_ptr<Inst>( new ImageIoRegistrar::Inst );
	}
	return sInst.get();
}

ImageTargetRef ImageIoRegistrar::createTarget( DataTargetRef dataTarget, ImageSourceRef imageSource, ImageTarget::Options options, std::string extension )
{
	return instance()->createTarget( dataTarget, imageSource, options, extension );
}

ImageTargetRef ImageIoRegistrar::Inst::createTarget( DataTargetRef dataTarget, ImageSourceRef imageSource, ImageTarget::Options options, std::string extension )
{
	std::transform( extension.begin(), extension.end(), extension.begin(), static_cast<int(*)(int)>(tolower) );	
	
	// strip the leading '.' that may be present
	if( extension.find( '.' ) == 0 )
		extension = extension.substr( 1, string::npos );
	
	map<string, multimap<int32_t,pair<ImageIoRegistrar::TargetCreationFunc,string> > >::iterator sIt = mTargets.find( extension );
	if( sIt != mTargets.end() )	{
		ImageIoRegistrar::TargetCreationFunc creationFunc = sIt->second.begin()->second.first;
		string extensionData = sIt->second.begin()->second.second;
		return (*creationFunc)( dataTarget, imageSource, options, extensionData );
	}
	else
		return ImageTargetRef(); // couldn't find a handler for this extension	
}

ImageSourceRef ImageIoRegistrar::createSource( DataSourceRef dataSource, ImageSource::Options options, string extension )
{
	return instance()->createSource( dataSource, options, extension );
}

ImageSourceRef ImageIoRegistrar::Inst::createSource( DataSourceRef dataSource, ImageSource::Options options, string extension )
{
	std::transform( extension.begin(), extension.end(), extension.begin(), static_cast<int(*)(int)>( tolower ) );

	// strip the leading '.' that may be present
	if( extension.find( '.' ) == 0 )
		extension = extension.substr( 1, string::npos );

	// for non-empty extensions we'll walk everyone who is registered for this extension
	if( ! extension.empty() ) {
		map<string,multimap<int32_t,ImageIoRegistrar::SourceCreationFunc> >::iterator sIt = mSources.find( extension );
		if( sIt != mSources.end() ) {
			for( multimap<int32_t,ImageIoRegistrar::SourceCreationFunc>::const_iterator sourcesIt = sIt->second.begin(); sourcesIt != sIt->second.end(); ++sourcesIt ) {
				try {
					return (*(sourcesIt->second))( dataSource, options );
				}
				catch( ImageIoException &exc ) {
					// if we're out of handlers, rethrow the exception, otherwise continue on
					if( options.getThrowOnFirstException() || ( boost::next( sourcesIt ) == sIt->second.end() && mGenericSources.empty() ) )
						throw;
				}
			}
		}
	}

	// if there is no extension, or none of the registered types got it, we'll have to try the generic loaders
	for( map<int32_t, ImageIoRegistrar::SourceCreationFunc>::const_iterator genericIt = mGenericSources.begin(); genericIt != mGenericSources.end(); ++genericIt ) {
		try {
			return (*(genericIt->second))( dataSource, options );
		}
		catch( ImageIoException &exc ) {
			// if we're out of handlers, rethrow the exception, otherwise continue on
			if( options.getThrowOnFirstException() || boost::next( genericIt ) == mGenericSources.end() )
				throw;
		}
	}

	assert( 0 && "unreachable" );
}

void ImageIoRegistrar::registerSourceType( string extension, SourceCreationFunc func, int32_t priority )
{
	instance()->registerSourceType( extension, func, priority );
}

void ImageIoRegistrar::Inst::registerSourceType( string extension, SourceCreationFunc func, int32_t priority )
{
	// make sure the extension is all lower-case
	std::transform( extension.begin(), extension.end(), extension.begin(), static_cast<int(*)(int)>(tolower) );	
	
	map<string,multimap<int32_t,SourceCreationFunc> >::iterator sIt = mSources.find( extension );
	if( sIt != mSources.end() ) {	// do we already have a source of this extension?
		// let's make sure this func is not already registered against this extension (which can happen in the big generic handlers)
		for( multimap<int32_t,SourceCreationFunc>::const_iterator existIt = sIt->second.begin(); existIt != sIt->second.end(); ++existIt )
			if( existIt->second == func )
				return;
		sIt->second.insert( make_pair( priority, func ) );
	}
	else {
		multimap<int32_t,SourceCreationFunc> newMap;
		newMap.insert( make_pair( priority, func ) );
		mSources.insert( make_pair( extension, newMap ) );
	}
}


void ImageIoRegistrar::registerTargetType( string extension, TargetCreationFunc func, int32_t priority, const string &extensionData )
{
	instance()->registerTargetType( extension, func, priority, extensionData );
}

void ImageIoRegistrar::Inst::registerTargetType( string extension, TargetCreationFunc func, int32_t priority, const string &extensionData )
{
	// make sure the extension is all lower-case
	std::transform( extension.begin(), extension.end(), extension.begin(), static_cast<int(*)(int)>(tolower) );	

	map<string, multimap<int32_t,pair<ImageIoRegistrar::TargetCreationFunc,string> > >::iterator sIt = mTargets.find( extension );
	if( sIt != mTargets.end() ) {	// do we already have a target for this extension?
		sIt->second.insert( make_pair( priority, make_pair( func, extensionData ) ) );
	}
	else {
		multimap<int32_t,pair<ImageIoRegistrar::TargetCreationFunc,string> > newMap;
		newMap.insert( make_pair( priority, make_pair( func, extensionData ) ) );
		mTargets.insert( make_pair( extension, newMap ) );
	}
}

void ImageIoRegistrar::registerSourceGeneric( SourceCreationFunc func, int32_t priority )
{
	instance()->registerSourceGeneric( func, priority );
}

void ImageIoRegistrar::Inst::registerSourceGeneric( SourceCreationFunc func, int32_t priority )
{
	mGenericSources.insert( make_pair( priority, func ) );
}


} // namespace cinder
