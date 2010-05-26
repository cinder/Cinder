#pragma once

#include <opencv/cv.h>

#include "cinder/Cinder.h"
#include "cinder/ImageIo.h"

namespace cinder {

class ImageTargetCvMat : public ImageTarget {
  public:
	static shared_ptr<ImageTargetCvMat> createRef( cv::Mat *mat ) { return shared_ptr<ImageTargetCvMat>( new ImageTargetCvMat( mat ) ); }

	virtual bool hasAlpha() const { return mMat->channels() == 4; }	
	virtual void*	getRowPointer( int32_t row ) { return reinterpret_cast<void*>( reinterpret_cast<uint8_t*>(mMat->data) + row * mMat->step ); }
	
  protected:
	ImageTargetCvMat( cv::Mat *mat );
	
	cv::Mat		*mMat;
};

class ImageSourceCvMat : public ImageSource {
  public:
	ImageSourceCvMat( const cv::Mat &mat )
		: ImageSource()
	{
		mWidth = mat.cols;
		mHeight = mat.rows;
		if( (mat.channels() == 3) || (mat.channels() == 4) ) {
			setColorModel( ImageIo::CM_RGB );
			if( mat.channels() == 4 )
				setChannelOrder( ImageIo::BGRA );
			else
				setChannelOrder( ImageIo::BGR );
		}
		else if( mat.channels() == 1 ) {
			setColorModel( ImageIo::CM_GRAY );
			setChannelOrder( ImageIo::Y );
		}
		
		switch( mat.depth() ) {
			case CV_8U: setDataType( ImageIo::UINT8 ); break;
			case CV_16U: setDataType( ImageIo::UINT16 ); break;
			case CV_32F: setDataType( ImageIo::FLOAT32 ); break;
			default:
				throw ImageIoExceptionIllegalDataType();
		}

		mRowBytes = mat.step;
		mData = reinterpret_cast<const uint8_t*>( mat.data );
	}

	void load( ImageTargetRef target ) {
		// get a pointer to the ImageSource function appropriate for handling our data configuration
		ImageSource::RowFunc func = setupRowFunc( target );
		
		const uint8_t *data = mData;
		for( int32_t row = 0; row < mHeight; ++row ) {
			((*this).*func)( target, row, data );
			data += mRowBytes;
		}
	}
	
	const uint8_t		*mData;
	int32_t				mRowBytes;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// ImageTargetCvMat
ImageTargetCvMat::ImageTargetCvMat( cv::Mat *mat )
	: ImageTarget(), mMat( mat )
{
	switch( mat->depth() ) {
		case CV_8U: setDataType( ImageIo::UINT8 ); break;
		case CV_16U: setDataType( ImageIo::UINT16 ); break;
		case CV_32F: setDataType( ImageIo::FLOAT32 ); break;
		default:
			throw ImageIoExceptionIllegalDataType();
	}
	
	switch( mat->channels() ) {
		case 1:
			setColorModel( ImageIo::CM_GRAY );
			setChannelOrder( ImageIo::Y );			
		break;
		case 3:
			setColorModel( ImageIo::CM_RGB );
			setChannelOrder( ImageIo::BGR );
		break;
		case 4:
			setColorModel( ImageIo::CM_RGB );
			setChannelOrder( ImageIo::BGRA );			
		break;
		default:
			throw ImageIoExceptionIllegalColorModel();
		break;
	}
}

inline cv::Mat toOcv( ci::ImageSourceRef sourceRef )
{
	int depth = CV_8U;
	if( sourceRef->getDataType() == ImageIo::UINT16 )
		depth = CV_16U;
	else if( sourceRef->getDataType() == ImageIo::FLOAT32 )
		depth = CV_32F;
	int channels = ImageIo::channelOrderNumChannels( sourceRef->getChannelOrder() );
	
	cv::Mat result( sourceRef->getHeight(), sourceRef->getWidth(), CV_MAKETYPE( depth, channels) );
	ImageTargetRef target = ImageTargetCvMat::createRef( &result );
	sourceRef->load( target );
	return result;
}

inline cv::Mat toOcvRef( Channel8u &channel )
{
	return cv::Mat( channel.getHeight(), channel.getWidth(), CV_MAKETYPE( CV_8U, 1 ), channel.getData(), channel.getRowBytes() );
}

inline cv::Mat toOcvRef( Channel32f &channel )
{
	return cv::Mat( channel.getHeight(), channel.getWidth(), CV_MAKETYPE( CV_32F, 1 ), channel.getData(), channel.getRowBytes() );
}

inline cv::Mat toOcvRef( Surface8u &surface )
{
	return cv::Mat( surface.getHeight(), surface.getWidth(), CV_MAKETYPE( CV_8U, surface.hasAlpha()?4:3), surface.getData(), surface.getRowBytes() );
}

inline cv::Mat toOcvRef( Surface32f &surface )
{
	return cv::Mat( surface.getHeight(), surface.getWidth(), CV_MAKETYPE( CV_32F, surface.hasAlpha()?4:3), surface.getData(), surface.getRowBytes() );
}

inline ImageSourceRef fromOcv( cv::Mat &mat )
{
	return ImageSourceRef( new ImageSourceCvMat( mat ) );
}

inline cv::Scalar toOcv( const Color &color )
{
	return CV_RGB( color.r * 255, color.g * 255, color.b * 255 );
}

inline Vec2f fromOcv( const cv::Point2f &point )
{
	return Vec2f( point.x, point.y );
}

inline cv::Point2f toOcv( const Vec2f &point )
{
	return cv::Point2f( point.x, point.y );
}

inline Vec2i fromOcv( const cv::Point &point )
{
	return Vec2i( point.x, point.y );
}

inline cv::Point toOcv( const Vec2i &point )
{
	return cv::Point( point.x, point.y );
}

inline cv::Rect toOcv( const ci::Area &r )
{
	return cv::Rect( r.x1, r.y1, r.getWidth(), r.getHeight() );
}

inline ci::Area fromOcv( const cv::Rect &r )
{
	return Area( r.x, r.y, r.x + r.width, r.y + r.height );
}

} // namespace cinder
