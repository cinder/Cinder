#include "cinder/app/AppBasic.h"
#include "cinder/gl/Texture.h"
#include "cinder/Capture.h"
#include <cv.h>
#include <highgui.h>
#include <string>
#include <sstream>
#include <cstdlib>
using namespace ci;
using namespace ci::app;

#include <list>
using std::list;

// We'll create a new Cinder Application by deriving from the BasicApp class
class ocvVideoApp : public AppBasic {
 public:
	// Cinder will always call this function whenever the user drags the mouse
	void mouseDrag( MouseEvent event );
	void setup();
	// Cinder calls this function 30 times per second by default
	void update();
	void draw();
	
	// This will maintain a list of points which we will draw line segments between
	list<Vec2f>		mPoints;
	Capture mCap;
	gl::Texture		mTexture;
	Surface			mSurf;
	cv::Mat			mMat, mMatOut;
	int ctr;
};

void ocvVideoApp::mouseDrag( MouseEvent event )
{
	// add wherever the user drags to the end of our list of points
	mPoints.push_back( event.getPos() );
}

void ocvVideoApp::setup()
{
	try {
		mCap = Capture( 640, 480 );
		mCap.start();
	}
	catch( ... ) {
		console() << "Failed to initialize capture" << std::endl;
	}
	
	ctr = 0;
	
}

void ocvVideoApp::update()
{
	
	// get surface, put it into a cv::Mat and write it to disk; just testing the data conversion
	
	if( mCap && mCap.checkNewFrame() ) {
		mSurf = mCap.getSurface();
		mMat = cv::Mat(480,640,CV_8UC3,mSurf.getData());
		/*
		std::stringstream ss;
		ss << "../../img-";
		ss << ctr;
		ss << ".jpg";
		cv::Mat img_bgr;
		cv::cvtColor(mMat, img_bgr, CV_RGB2BGR);
		cv::imwrite(ss.str(), img_bgr);
		ctr++;
		 */
		
	//	cv::GaussianBlur(mMat, mMatOut, cv::Size(19,19), 0);
		cv::Sobel(mMat, mMatOut, CV_8U, 1, 0, 3);
		
		Surface outSurf = Surface(640,480,false);
		outSurf.setData(mMatOut.data, 640, 480, 1);
		
		mTexture = gl::Texture(outSurf);
	}
	 
	
}

void ocvVideoApp::draw()
{
	if( mTexture ) {
		gl::draw( mTexture );
	}
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( ocvVideoApp, RendererGL )
