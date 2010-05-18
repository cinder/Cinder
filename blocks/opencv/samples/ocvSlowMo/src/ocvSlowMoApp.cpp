#include "cinder/app/AppBasic.h"
#include "cinder/gl/Texture.h"
#include "cinder/Capture.h"
#include "cinder/qtime/QuickTime.h"
#include "cinder/params/Params.h"

#include <OpenCV/cv.h>
#include "CinderOpenCV.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ocvSlowMoApp : public AppBasic {
  public:
	void setup();
	void update();
	void draw();

	void keyDown( KeyEvent event );
	void calcCurrentFrame();
	
	gl::Texture				mTexture;
	qtime::MovieSurface		mMovie;
	Surface					mSurface, mPrevSurface;
	cv::Mat					mCurrentFrame, mPrevFrame, mFlow;
	float					mRate, mTime;
	bool					mOpticalFlow, mBilinear;
	int						mPyramidLevels;
	int						mWinSize;
	
	params::InterfaceGl		mParams;
};

void ocvSlowMoApp::setup()
{
	try {
		vector<string> ext;
		ext.push_back( "mov" ); ext.push_back( "m4v" );
		string path = getOpenFilePath( "", ext );
		if( ! path.empty() ) {
			mMovie = qtime::MovieSurface( path );
			mMovie.play();
			mMovie.stop();
			mMovie.setLoop( true, true );
			mMovie.seekToStart();
			mSurface = mPrevSurface = mMovie.getSurface();
			mCurrentFrame = mPrevFrame = toOcv( Channel( mSurface ) );
			mTime = 0;
		}
	}
	catch( ... ) {
		console() << "Failed to open the quicktime" << std::endl;
	}

	mParams = params::InterfaceGl( "Parameters", Vec2i( 200, 400 ) );
	mRate = 20;
	mParams.addParam( "Rate", &mRate, "min=1 max=200 step=1 keyIncr=r keyDecr=R" );
	mOpticalFlow = false;
	mParams.addParam( "Optical Flow", &mOpticalFlow, "keyIncr=l" );
	mBilinear = false;
	mParams.addParam( "Bilinear", &mBilinear, "keyIncr=b" );
	mPyramidLevels = 2;
	mParams.addParam( "Pyramid Levels", &mPyramidLevels, "min=1 max=20 step=1 keyIncr=p keyDecr=P" );	
	mWinSize = 2;
	mParams.addParam( "Window size", &mWinSize, "min=1 max=30 step=1 keyIncr=w keyDecr=W" );	
}

/*Color8u sampleBilinear( const Surface8u &surface, Vec2f pos )
{
	pos.x = constrain<float>( pos.x, 0, surface.getWidth() - 1);
	pos.y = constrain<float>( pos.y, 0, surface.getHeight() - 1);
	float xfrac = pos.x - math<float>::floor( pos.x );
	float yfrac = pos.y - math<float>::floor( pos.y );
	int32_t xi = (int32_t)pos.x;
	int32_t yi = (int32_t)pos.y;
	int32_t xi1 = constrain<int32_t>( xi + 1, 0, surface.getWidth() - 1 );
	int32_t yi1 = constrain<int32_t>( yi + 1, 0, surface.getHeight() - 1 );

	const uint8_t *p = surface.getData( Vec2i( xi, yi ) );
	float weight = (1 - xfrac) * (1 - yfrac);
	float redSum = weight * p[surface.getRedOffset()];
	float greenSum = weight * p[surface.getGreenOffset()];	
	float blueSum = weight * p[surface.getBlueOffset()];		

	p = surface.getData( Vec2i( xi1, yi ) );
	weight = (xfrac) * (1 - yfrac);
	redSum += weight * p[surface.getRedOffset()];
	greenSum += weight * p[surface.getGreenOffset()];	
	blueSum += weight * p[surface.getBlueOffset()];		

	p = surface.getData( Vec2i( xi, yi1 ) );
	weight = (1 - xfrac) * (yfrac);
	redSum += weight * p[surface.getRedOffset()];
	greenSum += weight * p[surface.getGreenOffset()];
	blueSum += weight * p[surface.getBlueOffset()];		

	p = surface.getData( Vec2i( xi1, yi1 ) );
	weight = (xfrac) * (yfrac);
	redSum += weight * p[surface.getRedOffset()];
	greenSum += weight * p[surface.getGreenOffset()];
	blueSum += weight * p[surface.getBlueOffset()];		

	return Color8u( (uint8_t)redSum, (uint8_t)greenSum, (uint8_t)blueSum );
}*/

Color8u sampleBilinear( const Surface8u &surface, Vec2f pos )
{
	pos.x = constrain<float>( pos.x, 0, surface.getWidth() - 1);
	pos.y = constrain<float>( pos.y, 0, surface.getHeight() - 1);
	int32_t xfrac = ( pos.x - math<float>::floor( pos.x ) ) * 256;
	int32_t yfrac = ( pos.y - math<float>::floor( pos.y ) ) * 256;
	int32_t xi = (int32_t)pos.x;
	int32_t yi = (int32_t)pos.y;
	int32_t xi1 = constrain<int32_t>( xi + 1, 0, surface.getWidth() - 1 );
	int32_t yi1 = constrain<int32_t>( yi + 1, 0, surface.getHeight() - 1 );

	const uint8_t *p = surface.getData( Vec2i( xi, yi ) );
	int32_t weight = (256 - xfrac) * (256 - yfrac);
	int32_t redSum = weight * p[surface.getRedOffset()];
	int32_t greenSum = weight * p[surface.getGreenOffset()];	
	int32_t blueSum = weight * p[surface.getBlueOffset()];		

	p = surface.getData( Vec2i( xi1, yi ) );
	weight = (xfrac) * (256 - yfrac);
	redSum += weight * p[surface.getRedOffset()];
	greenSum += weight * p[surface.getGreenOffset()];	
	blueSum += weight * p[surface.getBlueOffset()];		

	p = surface.getData( Vec2i( xi, yi1 ) );
	weight = (256 - xfrac) * (yfrac);
	redSum += weight * p[surface.getRedOffset()];
	greenSum += weight * p[surface.getGreenOffset()];
	blueSum += weight * p[surface.getBlueOffset()];		

	p = surface.getData( Vec2i( xi1, yi1 ) );
	weight = (xfrac) * (yfrac);
	redSum += weight * p[surface.getRedOffset()];
	greenSum += weight * p[surface.getGreenOffset()];
	blueSum += weight * p[surface.getBlueOffset()];		

	return Color8u( redSum / 65536, greenSum / 65536, blueSum / 65536 );
}

Surface8u interpolateFramesFade( const Surface &start, const Surface &end, float blend, cv::Mat motion )
{
	Surface8u result = start.clone( false );

	int32_t sBlend = 256 * blend;
	int32_t eBlend = 256 - sBlend;
	
	Surface::Iter resultIter = result.getIter();
	Surface::ConstIter startIter = start.getIter();	
	Surface::ConstIter endIter = end.getIter();		
	while( resultIter.line() ) {
		startIter.line(); endIter.line();
		while( resultIter.pixel() ) {
			startIter.pixel(); endIter.pixel();
			resultIter.r() = ( startIter.r() * sBlend + endIter.r() * eBlend ) / 256;
			resultIter.g() = ( startIter.g() * sBlend + endIter.g() * eBlend ) / 256;			
			resultIter.b() = ( startIter.b() * sBlend + endIter.b() * eBlend ) / 256;
		}
	}
	
	return result;
}

Surface8u interpolateFrames( const Surface &start, const Surface &end, float blend, cv::Mat motion )
{
	Surface8u result = start.clone( false );

	float invBlend = 1.0f - blend;
	int32_t sBlend = 256 * blend;
	int32_t eBlend = 256 - sBlend;
	
	Surface::Iter resultIter = result.getIter();
	Surface::ConstIter startIter = start.getIter();	
	Surface::ConstIter endIter = end.getIter();		
	while( resultIter.line() ) {
		startIter.line(); endIter.line();
		while( resultIter.pixel() ) {
			startIter.pixel(); endIter.pixel();
			float *ptr = reinterpret_cast<float*>(motion.data + motion.step * resultIter.y() + resultIter.x() * sizeof(float)*2);
			Vec2f delta( ptr[0], ptr[1] );
			ColorA8u oldPixel = start.getPixel( Vec2i( delta * invBlend ) + Vec2i( resultIter.x(), resultIter.y() ) );
			resultIter.r() = ( oldPixel.r * sBlend + endIter.r() * eBlend ) / 256;
			resultIter.g() = ( oldPixel.g * sBlend + endIter.g() * eBlend ) / 256;			
			resultIter.b() = ( oldPixel.b * sBlend + endIter.b() * eBlend ) / 256;
		}
	}
	
	return result;
}

Surface8u interpolateFramesBilinear( const Surface &start, const Surface &end, float blend, cv::Mat motion )
{
	Surface8u result = start.clone( false );

	float invBlend = 1.0f - blend;
	int32_t sBlend = 256 * blend;
	int32_t eBlend = 256 - sBlend;
	
	Surface::Iter resultIter = result.getIter();
	Surface::ConstIter startIter = start.getIter();	
	Surface::ConstIter endIter = end.getIter();		
	while( resultIter.line() ) {
		startIter.line(); endIter.line();
		while( resultIter.pixel() ) {
			startIter.pixel(); endIter.pixel();
			float *ptr = reinterpret_cast<float*>(motion.data + motion.step * resultIter.y() + resultIter.x() * sizeof(float)*2);
			Vec2f delta( ptr[0], ptr[1] );
			//Color8u oldPixel = start.getPixel( Vec2i( delta * invBlend ) + Vec2i( resultIter.x(), resultIter.y() ) );
			Color8u oldPixel = sampleBilinear( start, delta * invBlend + Vec2f( resultIter.getPos() ) );
			resultIter.r() = ( oldPixel.r * sBlend + endIter.r() * eBlend ) / 256;
			resultIter.g() = ( oldPixel.g * sBlend + endIter.g() * eBlend ) / 256;
			resultIter.b() = ( oldPixel.b * sBlend + endIter.b() * eBlend ) / 256;
		}
	}
	
	return result;
}

void ocvSlowMoApp::calcCurrentFrame()
{
	mPrevSurface = mSurface;
	mPrevFrame = mCurrentFrame;	
	mSurface = mMovie.getSurface();
	mCurrentFrame = cv::Mat( toOcv( Channel( mSurface ) ) );
	cv::calcOpticalFlowFarneback( mPrevFrame, mCurrentFrame, mFlow, 0.5, mPyramidLevels, mWinSize * 2 + 1, 2, 7, 1.5, cv::OPTFLOW_FARNEBACK_GAUSSIAN );	
	mTime = 0;
}

void ocvSlowMoApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' ) {
		setFullScreen( ! isFullScreen() );
	}
	else if( event.getCode() == KeyEvent::KEY_RIGHT ) {
		mMovie.stepForward();
		calcCurrentFrame();
	}
	else if( event.getCode() == KeyEvent::KEY_LEFT ) {
		mMovie.stepBackward();
		calcCurrentFrame();
	}
	else if( event.getChar() == 'u' ) {
		cv::Mat currentFrame( toOcv( Channel( mSurface ) ) );
		cv::calcOpticalFlowFarneback( mPrevFrame, mCurrentFrame, mFlow, 0.5, mPyramidLevels, mWinSize * 2 + 1, 2, 7, 1.5, cv::OPTFLOW_FARNEBACK_GAUSSIAN );	
	}
}

void ocvSlowMoApp::update()
{
	if( ! mFlow.data )
		return;

	mTime = fmod( mTime + 1 / mRate, 1.0f );
	
	if( ! mOpticalFlow )
		mTexture = gl::Texture( interpolateFramesFade( mSurface, mPrevSurface, mTime, mFlow ) );
	else if( mBilinear )
		mTexture = gl::Texture( interpolateFramesBilinear( mSurface, mPrevSurface, mTime, mFlow ) );
	else
		mTexture = gl::Texture( interpolateFrames( mSurface, mPrevSurface, mTime, mFlow ) );
	
	/*glDisable( GL_TEXTURE_2D );
	glColor3f( 1, 1, 0 );
	glBegin( GL_LINES );
	Surface8u vectorSurface( mMovie.getWidth(), mMovie.getHeight(), false );
	for( int y = 0; y < flow.rows; y += 4 ) {
		const float* f = (const float*)(flow.data + flow.step*y);
		for( int x = 0; x < flow.cols; x += 4 ) {
			Vec2f v( f[2*x], f[2*x+1] );
			v.normalize();
			gl::vertex( Vec2f( x, y ) );
			gl::vertex( Vec2f( x + v.x * 4, y + v.y * 4 ) );
		}
	}
	glEnd();
	
	mPrevFrame = currentFrame;
//	mTexture = gl::Texture( surface );*/
}

void ocvSlowMoApp::draw()
{
	if( ! mMovie )
		return;
	if( ! mFlow.data )
		return;
	if( ! mTexture )
		return;

	gl::clear();
	
	gl::setMatricesWindow( getWindowSize() );
	glColor3f( 1, 1, 1 );
	gl::draw( mTexture, getWindowBounds() );
	
	glDisable( GL_TEXTURE_2D );
	glColor3f( 1, 1, 0 );
	gl::drawSolidRect( Rectf( 0, getWindowHeight() - 20, getWindowWidth() * mTime, getWindowHeight() ) );	
	
	mParams.draw();
}


CINDER_APP_BASIC( ocvSlowMoApp, RendererGl )
