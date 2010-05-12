#include "cinder/app/AppBasic.h"
#include "cinder/gl/Texture.h"
#include "cinder/Capture.h"

#include <OpenCV/cv.h>
#include "CinderOpenCV.h"

using namespace ci;
using namespace ci::app;

class ocvVideoApp : public AppBasic {
  public:
	void setup();
	void update();
	void draw();
	
	Capture			mCap;
	gl::Texture		mTexture;
};

void ocvVideoApp::setup()
{
	try {
		mCap = Capture( 640, 480 );
		mCap.start();
	}
	catch( ... ) {
		console() << "Failed to initialize capture" << std::endl;
	}
}

void ocvVideoApp::update()
{
	if( mCap && mCap.checkNewFrame() ) {
		cv::Mat matEdges;
		cv::Sobel( toOcv( mCap.getSurface() ), matEdges, CV_8U, 1, 0, 3);
//		cv::Laplacian( toOcv( mCap.getSurface() ), matEdges, CV_8U );
		
//		cv::line( matEdges, cv::Point( 1, 1 ), cv::Point( 30, 30 ), CV_RGB( 1.0, 0.5, 0.25 ) );
		cv::circle( matEdges, cv::Point( 200, 200 ), 300, CV_RGB( 1.0, 0.5, 25 ), -1 );
		
		mTexture = gl::Texture( fromOcv( matEdges ) );
	}	 
}

void ocvVideoApp::draw()
{
	gl::clear();
	if( mTexture ) {
		gl::draw( mTexture );
	}
}


CINDER_APP_BASIC( ocvVideoApp, RendererGl )
