#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"
#include "cinder/ip/Threshold.h"
#include "cinder/ip/Grayscale.h"

using namespace std;
using namespace ci;
using namespace ci::app;

class ThresholdTestApp : public App {
 public:
	void setup() override;
	void update() override;
	void draw() override;

  private:
	void loadFile( const fs::path &path );

	bool					mUseAdaptiveThreshold = false;
	bool					mUseAdaptivePercentage = false;
	bool					mShowOriginalGrayScale = false;
	bool					mUseClassVersion = true;
	int						mThresholdValue, mAdaptiveThresholdKernel;
	float					mAdaptiveThresholdPercentage;
	params::InterfaceGlRef	mParams;
	gl::TextureRef			mTexture;
	Surface8uRef			mSurface;
	Channel8u				mGraySurface, mThresholded;

	ip::AdaptiveThreshold		mThresholdClass;
};

void ThresholdTestApp::setup()
{
	mParams = params::InterfaceGl::create( "Parameters", ivec2( 300, 240 ) );
	mParams->addParam( "Threshold", &mThresholdValue, "min=0 max=255 keyIncr=v keyDecr=V" );
	mParams->addButton( "open file", [this] { loadFile( getOpenFilePath() ); }, "key=o" );
	mParams->addSeparator();
	mParams->addParam( "Use Adapative", &mUseAdaptiveThreshold );
	mParams->addParam( "Use Adapative class", &mUseClassVersion );
	mParams->addParam( "Use Adapative percentage", &mUseAdaptivePercentage );
	mParams->addParam( "Show Grayscale", &mShowOriginalGrayScale );
	mParams->addParam( "Adaptive Kernel", &mAdaptiveThresholdKernel, "min=0 max=1000 keyIncr=k keyDecr=K" );
	mParams->addParam( "Adaptive Percentage", &mAdaptiveThresholdPercentage, "min=0 max=1.0 step=0.01 keyIncr=p keyDecr=P" );
	
	mThresholdValue = 128;
	mAdaptiveThresholdPercentage = 0.01f;
	mAdaptiveThresholdKernel = 64;
	
	loadFile( getOpenFilePath() );
}

void ThresholdTestApp::loadFile( const fs::path &path )
{
	if( ! path.empty() ) {
		mSurface = Surface8u::create( loadImage( path ) );
		mGraySurface = Channel( mSurface->getWidth(), mSurface->getHeight() );
		mThresholded = Channel( mSurface->getWidth(), mSurface->getHeight() );
		ip::grayscale( *mSurface, &mGraySurface );

		mThresholdClass = ip::AdaptiveThreshold( &mGraySurface );
	}
}

void ThresholdTestApp::update()
{
	if( mSurface ) {
		if( mUseClassVersion ) {
			mThresholdClass.calculate( mAdaptiveThresholdKernel, mAdaptiveThresholdPercentage, &mThresholded );
		}
		else if( mUseAdaptiveThreshold ) {
			if( mUseAdaptivePercentage )
				ip::adaptiveThreshold<uint8_t>( mGraySurface, mAdaptiveThresholdKernel, mAdaptiveThresholdPercentage, &mThresholded );
			else {
				ip::adaptiveThresholdZero<uint8_t>( mGraySurface, mAdaptiveThresholdKernel, &mThresholded );
//				ip::adaptiveThresholdZero<uint8_t>( &mGraySurface, mAdaptiveThresholdKernel );
			}
		}
		else {
			ip::threshold<uint8_t>( mGraySurface, mThresholdValue, &mThresholded );
		}

		if( mShowOriginalGrayScale )
			mTexture = gl::Texture::create( mGraySurface );
		else
			mTexture = gl::Texture::create( mThresholded );
	}
}

void ThresholdTestApp::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.15f ) );

	if( mTexture )
		gl::draw( mTexture );
	
	mParams->draw();
}

CINDER_APP( ThresholdTestApp, RendererGl )
