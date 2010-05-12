#pragma once

#include <opencv/cv.h>

#include "cinder/Cinder.h"
#include "cinder/ImageIo.h"

namespace cinder {

class ImageTargetCvMat : public ImageTarget {
  public:
	static shared_ptr<ImageTargetCvMat> createRef( cv::Mat *mat ) { return shared_ptr<ImageTargetCvMat>( new ImageTargetCvMat( mat ) ); }

	virtual bool hasAlpha() const;
	
	virtual void*	getRowPointer( int32_t row );
	
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

bool ImageTargetCvMat::hasAlpha() const
{
	return mMat->channels() == 4;
}

void* ImageTargetCvMat::getRowPointer( int32_t row )
{
	return reinterpret_cast<void*>( reinterpret_cast<uint8_t*>(mMat->data) + row * mMat->step );
}

cv::Mat toOcv( ci::ImageSourceRef sourceRef )
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

ImageSourceRef fromOcv( cv::Mat &mat )
{
	return ImageSourceRef( new ImageSourceCvMat( mat ) );
}

} // namespace cinder
