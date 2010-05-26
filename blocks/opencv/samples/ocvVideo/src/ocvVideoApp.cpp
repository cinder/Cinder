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
		cv::Mat input( toOcv( mCap.getSurface() ) ), output;
		cv::threshold( input, output, 128, 255, CV_8U );
//		cv::Sobel( input, output, CV_8U, 1, 0, 3);
//		cv::Laplacian( input, output, CV_8U );		
//		cv::circle( output, toOcv( Vec2f(200, 200) ), 300, toOcv( Color( 0, 0.5f, 1 ) ), -1 );
//		cv::line( output, cv::Point( 1, 1 ), cv::Point( 30, 30 ), toOcv( Color( 1, 0.5f, 0 ) ) );
		
		mTexture = gl::Texture( fromOcv( output ) );
	}	 
}

void ocvVideoApp::draw()
{
	gl::clear();
	if( mTexture )
		gl::draw( mTexture );
}


CINDER_APP_BASIC( ocvVideoApp, RendererGl )
