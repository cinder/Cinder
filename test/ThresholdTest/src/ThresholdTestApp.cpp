#include "flint/app/AppBasic.h"
#include "flint/params/Params.h"
#include "flint/imp/Threshold.h"
#include "flint/imp/Grayscale.h"

using namespace fli;
using namespace fli::app;

class ThresholdTestApp : public AppBasic {
 public:
	void setup();
	void loadFile( const std::string &path );

	void keyDown( KeyEvent event );

	void update();
	void draw();
	
	bool					mUseAdaptiveThreshold;
	int						mThresholdValue, mAdaptiveThresholdKernel;
	float					mAdaptiveThresholdPercentage;
	params::InterfaceGL		*mParams;
	gl::Texture				mTexture;
	Surface8u				mSurface;
	Channel8u				*mGraySurface, *mThresholded;
};

void ThresholdTestApp::setup()
{
	mParams = new params::InterfaceGL( this, "Parameters", Offset( 200, 400 ) );
	mParams->addParam( "Threshold", &mThresholdValue, "min=0 max=255 keyIncr=v keyDecr=V" );
	mParams->addSeparator();
	mParams->addParam( "Use Adapative", &mUseAdaptiveThreshold );
	mParams->addParam( "Adaptive Kernel", &mAdaptiveThresholdKernel, "min=0 max=1000 keyIncr=k keyDecr=K" );
	mParams->addParam( "Adaptive Percentage", &mAdaptiveThresholdPercentage, "min=0 max=1.0 step=0.01 keyIncr=p keyDecr=P" );
	
	mThresholdValue = 128;
	mAdaptiveThresholdPercentage = 0.01f;
	mAdaptiveThresholdKernel = 64;
	
	loadFile( getOpenFilePath() );
}

void ThresholdTestApp::loadFile( const std::string &path )
{
	if( ! path.empty() ) {
		mSurface = loadImage( path );
		mGraySurface = new Channel( mSurface.getWidth(), mSurface.getHeight() );
		mThresholded = new Channel( mSurface.getWidth(), mSurface.getHeight() );
		imp::grayscale( mSurface, mGraySurface );
	}
}

void ThresholdTestApp::update()
{
	if( mSurface ) {
		if( mUseAdaptiveThreshold ) {
			//imp::adaptiveThreshold<uint8_t>( *mGraySurface, mAdaptiveThresholdKernel, mAdaptiveThresholdPercentage, mThresholded );
			imp::adaptiveThresholdZero<uint8_t>( *mGraySurface, mAdaptiveThresholdKernel, mThresholded );
			//imp::adaptiveThresholdZero<uint8_t>( mGraySurface, mAdaptiveThresholdKernel );
		}
		else {
			imp::threshold<uint8_t>( *mGraySurface, mThresholdValue, mThresholded );
		}
		
		mTexture = gl::Texture( *mThresholded );
		//mTexture = gl::Texture( *mGraySurface );
	}
}

void ThresholdTestApp::keyDown( KeyEvent event )
{
	switch( event.getChar() ) {
		case 'o':
			loadFile( getOpenFilePath() );
		break;
	}
}

void ThresholdTestApp::draw()
{
	glClearColor( 0, 0, 0, 0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	if( mTexture )
		mTexture.draw( 0, 0, mTexture.getWidth(), mTexture.getHeight() );
	
	mParams->draw();
}

// This line tells Flint to actually create the application
FLI_APP_BASIC( ThresholdTestApp, RendererGL )
