#include "cinder/app/AppBasic.h"
#include "cinder/ImageIo.h"

#include <opencv/cv.h>

using namespace ci;
using namespace ci::app;

#include <list>
using std::list;

namespace cinder {

#if 0
class SurfaceCv {
  public:
	SurfaceCv() { }
	SurfaceCv( int32_t width, int32_t height )
		: mMat( height, width, CV_8UC3 )
	{
	}

	SurfaceCv( const cv::Mat &mat )
		: mMat( mat )
	{
	}

	explicit SurfaceCv( Surface surface )
		: mSurface( surface ),
			mMat( surface.getHeight(), surface.getWidth(), surface.hasAlpha() ? CV_8UC4 : CV_8UC3, surface.getData(), surface.getRowBytes() )
	{
	}

/*	SurfaceCv( ImageSourceRef source )
		: mWidth( source->getWidth() ), mHeight( source->getHeight() ), mSurface( source )
	{
		mMat = cv::Mat( mSurface.getHeight(), mSurface.getWidth(), mSurface.hasAlpha() ? CV_8UC4 : CV_8UC3, mSurface.getData(), mSurface.getRowBytes() );
	}*/

	int32_t		getWidth() const { return mMat.cols; }
	int32_t		getHeight() const { return mMat.rows; }	
	
	operator const cv::Mat&() const { return mMat; }
	operator cv::Mat&() { return mMat; }

//	operator ci::Surface() { return Surface( mMat.ptr(), mMat.cols, mMat.rows, mMat.step, SurfaceChannelOrder::RGB ); }

	operator ImageSourceRef() const { return ImageSourceRef( Surface( const_cast<uint8_t*>( mMat.ptr() ), mMat.cols, mMat.rows, mMat.step, SurfaceChannelOrder::RGB ) ); }
	
  private:
	cv::Mat			mMat;
	Surface			mSurface; // this is here only to register a reference count against a constructing Surface
};
#endif

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
		setColorModel( ImageIo::CM_RGB );
		setChannelOrder( ImageIo::RGB );
		setDataType( ImageIo::UINT8 );
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
			setChannelOrder( ImageIo::RGB );			
		break;
		case 4:
			setColorModel( ImageIo::CM_RGB );
			setChannelOrder( ImageIo::RGBA );			
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
	
	cv::Mat result( sourceRef->getWidth(), sourceRef->getHeight(), CV_MAKETYPE( depth, channels) );
	ImageTargetRef target = ImageTargetCvMat::createRef( &result );
	sourceRef->load( target );
	return result;
}

ImageSourceRef fromOcv( cv::Mat &mat )
{
	return ImageSourceRef( new ImageSourceCvMat( mat ) );
}

} // namespace cinder

// We'll create a new Cinder Application by deriving from the BasicApp class
class ocvHelloApp : public AppBasic {
  public:
	void setup();
};

void ocvHelloApp::setup()
{
	const char* imagename = "../../lena.jpg";
	cv::Mat img( toOcv( loadImage( imagename ) ) );
    
	cv::Mat img_yuv;
	cv::cvtColor( img, img_yuv, CV_BGR2YCrCb ); // convert image to YUV color space. The output image will be created automatically
    
	std::vector<cv::Mat> planes; // Vector is template vector class, similar to STL's vector. It can store matrices too.
    split(img_yuv, planes); // split the image into separate color planes
    
    // method 1. process Y plane using an iterator
	cv::MatIterator_<uchar> it = planes[0].begin<uchar>(), it_end = planes[0].end<uchar>();
    for(; it != it_end; ++it)
    {
        double v = *it*1.7 + rand()%21-10;
        *it = cv::saturate_cast<uchar>(v*v/255.);
    }
    
    // method 2. process the first chroma plane using pre-stored row pointer.
    // method 3. process the second chroma plane using individual element access
    for( int y = 0; y < img_yuv.rows; y++ )
    {
        uchar* Uptr = planes[1].ptr<uchar>(y);
        for( int x = 0; x < img_yuv.cols; x++ )
        {
            Uptr[x] = cv::saturate_cast<uchar>((Uptr[x]-128)/2 + 128);
            uchar& Vxy = planes[2].at<uchar>(y, x);
            Vxy = cv::saturate_cast<uchar>((Vxy-128)/2 + 128);
        }
    }
    
    // now merge the results back
    cv::merge(planes, img_yuv);
    // and produce the output RGB image
    cv::cvtColor(img_yuv, img, CV_YCrCb2BGR);
	
	cv::Mat blurred;
	cv::GaussianBlur( img, blurred, cv::Size(111,11), 0 );
	
	writeImage( "../../lena_out.jpg", fromOcv( blurred ) );
	//cv::imwrite("../../lenanew.jpg", img );
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( ocvHelloApp, RendererGl )