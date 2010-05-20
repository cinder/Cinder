#include "cinder/app/AppBasic.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"

#include "CinderOpenCV.h"

#include <opencv/cv.h>

using namespace ci;
using namespace ci::app;

#include <list>
using std::list;

// We'll create a new Cinder Application by deriving from the BasicApp class
class ocvHelloApp : public AppBasic {
  public:
	void setup();
	void draw();
	
	gl::Texture	mTexture;
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
	
	writeImage( "../../gray_out.jpg", fromOcv( planes[0] ) );
	
	cv::Mat blurred;
	cv::GaussianBlur( img, blurred, cv::Size(111,11), 0 );
	
	writeImage( "../../lena_out.jpg", fromOcv( blurred ) );
	//cv::imwrite("../../lenanew.jpg", img );
	mTexture = gl::Texture( fromOcv( blurred ) );
}

void ocvHelloApp::draw()
{
	gl::clear();
	gl::draw( mTexture );
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( ocvHelloApp, RendererGl )