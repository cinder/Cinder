#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ip/Threshold.h"
#include "cinder/ip/Grayscale.h"
#include "cinder/Log.h"
#include "cinder/CinderImGui.h"

#include <algorithm>

using namespace std;
using namespace ci;
using namespace ci::app;

class ThresholdTestApp : public App {
 public:
	void setup() override;
	void update() override;
	void draw() override;
	void keyDown( KeyEvent event ) override;

  private:
	void loadFile( const fs::path &path );
	void drawGui();

	bool					mUseAdaptiveThreshold = false;
	bool					mUseAdaptivePercentage = false;
	bool					mShowOriginalGrayScale = false;
	bool					mUseClassVersion = true;
	int						mThresholdValue, mAdaptiveThresholdKernel;
	float					mAdaptiveThresholdPercentage;
	gl::TextureRef			mTexture;
	Surface8uRef			mSurface;
	Channel8u				mGraySurface, mThresholded;
	bool					mShowGui = true;
	bool					mRequestOpenDialog = false;

	ip::AdaptiveThreshold		mThresholdClass;
};

void ThresholdTestApp::setup()
{
	ImGui::Initialize();

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

void ThresholdTestApp::keyDown( KeyEvent event )
{
	const int code = event.getCode();
	if( code == KeyEvent::KEY_ESCAPE ) {
		quit();
		return;
	}

	if( ! event.isControlDown() )
		return;

	switch( code ) {
	case KeyEvent::KEY_o:
		mRequestOpenDialog = true;
		break;
	case KeyEvent::KEY_a:
		mUseAdaptiveThreshold = ! mUseAdaptiveThreshold;
		CI_LOG_I( "Use adaptive threshold: " << std::boolalpha << mUseAdaptiveThreshold );
		break;
	case KeyEvent::KEY_c:
		mUseClassVersion = ! mUseClassVersion;
		CI_LOG_I( "Use adaptive class: " << std::boolalpha << mUseClassVersion );
		break;
	case KeyEvent::KEY_d:
		mUseAdaptivePercentage = ! mUseAdaptivePercentage;
		CI_LOG_I( "Use adaptive percentage: " << std::boolalpha << mUseAdaptivePercentage );
		break;
	case KeyEvent::KEY_g:
		mShowOriginalGrayScale = ! mShowOriginalGrayScale;
		CI_LOG_I( "Show grayscale: " << std::boolalpha << mShowOriginalGrayScale );
		break;
	case KeyEvent::KEY_h:
	case KeyEvent::KEY_QUESTION:
		mShowGui = ! mShowGui;
		break;
	case KeyEvent::KEY_v:
		if( event.isShiftDown() )
			mThresholdValue = std::max( mThresholdValue - 1, 0 );
		else
			mThresholdValue = std::min( mThresholdValue + 1, 255 );
		CI_LOG_I( "Threshold value: " << mThresholdValue );
		break;
	case KeyEvent::KEY_k:
		if( event.isShiftDown() )
			mAdaptiveThresholdKernel = std::max( mAdaptiveThresholdKernel - 1, 0 );
		else
			mAdaptiveThresholdKernel = std::min( mAdaptiveThresholdKernel + 1, 1000 );
		CI_LOG_I( "Adaptive kernel: " << mAdaptiveThresholdKernel );
		break;
	case KeyEvent::KEY_p:
		if( event.isShiftDown() )
			mAdaptiveThresholdPercentage = std::max( mAdaptiveThresholdPercentage - 0.01f, 0.0f );
		else
			mAdaptiveThresholdPercentage = std::min( mAdaptiveThresholdPercentage + 0.01f, 1.0f );
		CI_LOG_I( "Adaptive percentage: " << mAdaptiveThresholdPercentage );
		break;
	default:
		break;
	}
}

void ThresholdTestApp::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.15f ) );

	if( mTexture )
		gl::draw( mTexture );

	if( mRequestOpenDialog ) {
		mRequestOpenDialog = false;
		loadFile( getOpenFilePath() );
	}

	if( mShowGui )
		drawGui();
}

void ThresholdTestApp::drawGui()
{
	ImGui::ScopedWindow window( "Threshold Controls" );
	if( ImGui::Button( "Open Image..." ) )
		mRequestOpenDialog = true;

	ImGui::SameLine();
	ImGui::Checkbox( "Show Grayscale", &mShowOriginalGrayScale );

	ImGui::Separator();
	ImGui::Text( "Processing Mode" );
	ImGui::Checkbox( "Use Adaptive Threshold", &mUseAdaptiveThreshold );
	ImGui::Checkbox( "Use Adaptive Class", &mUseClassVersion );
	ImGui::Checkbox( "Use Adaptive Percentage", &mUseAdaptivePercentage );

	ImGui::Separator();
	ImGui::SliderInt( "Threshold", &mThresholdValue, 0, 255 );
	ImGui::SliderInt( "Adaptive Kernel", &mAdaptiveThresholdKernel, 0, 1000 );
	ImGui::SliderFloat( "Adaptive Percent", &mAdaptiveThresholdPercentage, 0.0f, 1.0f, "%.2f" );

	ImGui::Separator();
	ImGui::TextUnformatted( "Shortcuts: O=open, H=toggle GUI" );
}

CINDER_APP( ThresholdTestApp, RendererGl )
