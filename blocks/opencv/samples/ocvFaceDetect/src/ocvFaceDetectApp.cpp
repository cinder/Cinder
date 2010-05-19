#include "cinder/app/AppBasic.h"
#include "cinder/Capture.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "CinderOpenCv.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ocvFaceDetectApp : public AppBasic {
 public:
	void setup();

	void updateFaces( Channel grayCameraImage );
	void update();
	
	void draw();

	Capture			mCapture;
	gl::Texture		mCameraTexture;
	
	cv::CascadeClassifier mFaceCascade;
	cv::CascadeClassifier mEyeCascade;
	
	vector<Rectf>	mFaces;
	vector<Rectf>	mEyes;
};

void ocvFaceDetectApp::setup()
{
	mFaceCascade.load( getResourcePath( "haarcascade_frontalface_alt.xml" ) );
	mEyeCascade.load( getResourcePath( "haarcascade_eye.xml" ) );	
	
	mCapture = Capture( 640, 480 );
	mCapture.start();
}

void ocvFaceDetectApp::updateFaces( Channel grayCameraImage )
{
	static const int calcScale = 2; // calculate the image at half scale
	int scaledWidth = grayCameraImage.getWidth() / calcScale;
	int scaledHeight = grayCameraImage.getHeight() / calcScale; 
	cv::Mat smallImg( scaledHeight, scaledWidth, CV_8UC1 );

	cv::resize( toOcv( grayCameraImage ), smallImg, smallImg.size(), 0, 0, cv::INTER_LINEAR );
	cv::equalizeHist( smallImg, smallImg );

	// clear out the old ones
	mFaces.clear();
	mEyes.clear();

	vector<cv::Rect> faces;
	mFaceCascade.detectMultiScale( smallImg, faces, 1.1, 2, CV_HAAR_SCALE_IMAGE );
	for( vector<cv::Rect>::const_iterator faceIter = faces.begin(); faceIter != faces.end(); faceIter++ ) {
		Rectf faceRect( fromOcv( *faceIter ) );
		faceRect *= calcScale;
		mFaces.push_back( faceRect );
		
		vector<cv::Rect> eyes;
		mEyeCascade.detectMultiScale( smallImg( *faceIter ), eyes, 1.1, 2, CV_HAAR_SCALE_IMAGE );
		for( vector<cv::Rect>::const_iterator eyeIter = eyes.begin(); eyeIter != eyes.end(); ++eyeIter ) {
			Rectf eyeRect( fromOcv( *eyeIter ) );
			eyeRect = eyeRect * calcScale + faceRect.getUpperLeft();
			mEyes.push_back( eyeRect );
		}
	}
}

void ocvFaceDetectApp::update()
{
	if( mCapture.checkNewFrame() ) {
		Channel gray( mCapture.getSurface() );
		mCameraTexture = gl::Texture( mCapture.getSurface() );
		updateFaces( gray );
	}
}

void ocvFaceDetectApp::draw()
{
	if( ! mCameraTexture ) return;

	gl::setMatricesWindow( getWindowSize() );
	gl::enableAlphaBlending();
	
	gl::color( Color( 1, 1, 1 ) );
	gl::draw( mCameraTexture );
	
	mCameraTexture.disable();
	
	gl::color( ColorA( 1, 1, 0, 0.5f ) );
	for( vector<Rectf>::const_iterator faceIter = mFaces.begin(); faceIter != mFaces.end(); ++faceIter )
		gl::drawSolidRect( *faceIter );
	
	gl::color( ColorA( 0, 0, 1, 0.5f ) );
	for( vector<Rectf>::const_iterator eyeIter = mEyes.begin(); eyeIter != mEyes.end(); ++eyeIter )
		gl::drawSolidCircle( eyeIter->getCenter(), eyeIter->getWidth() / 2 );	
}

CINDER_APP_BASIC( ocvFaceDetectApp, RendererGl )
