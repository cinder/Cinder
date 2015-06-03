#include "cinder/app/App.h"
#include "cinder/cairo/Cairo.h"
#include "cinder/Perlin.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;

class perlinTestApp : public App {
 public:	
	void		prepareSettings( Settings *settings );

	void		setup();
	void		mouseDown( MouseEvent event );
	void		mouseDrag( MouseEvent event );
	void		keyDown( KeyEvent event );
	void		update();
	void		draw();

	void		renderNoise();

	cairo::SurfaceImage		*mNoiseSurface;
	int						mSeed;
	int						mOctaves;
	float					mTime;
	Perlin					mPerlin;
	
	float					mFrequency;
	
	vec2	mGradientPos;
	bool	mDrawLines;
	bool	mDrawNormalized;
	bool	mPaused;
};

void perlinTestApp::renderNoise()
{
	Surface8u &s = mNoiseSurface->getSurface();
	mPerlin = Perlin( mOctaves, mSeed );

	Surface8u::Iter iter = s.getIter();
	while( iter.line() ) {
		while( iter.pixel() ) {
			float v = ( mPerlin.fBm( vec3( iter.x(), iter.y(), mTime ) * mFrequency ) + 1.0f ) / 2.0f;
			v *= v * v;
			uint8_t val = v * 255;
			iter.r() = iter.g() = iter.b() = val;
		}
	}
}

void perlinTestApp::prepareSettings( Settings *settings )
{
	settings->setResizable( false );
}

void perlinTestApp::setup()
{
	mSeed = clock() & 65535;
	mOctaves = 4;
	mTime = 0.0f;
	mFrequency = 1 / 200.0f;
	mDrawNormalized = false;
	mPaused = false;
	mDrawLines = true;
	
	mGradientPos = vec2( getWindowWidth(), getWindowHeight() ) / 2.0f;

	mNoiseSurface = new cairo::SurfaceImage( getWindowWidth(), getWindowHeight(), false );
	renderNoise();
}

void perlinTestApp::mouseDown( MouseEvent event )
{		
	mGradientPos = vec2( event.getX(), event.getY() );
}

void perlinTestApp::mouseDrag( MouseEvent event )
{		
	mGradientPos = vec2( event.getX(), event.getY() );
}

void perlinTestApp::keyDown( KeyEvent event )
{
	if( event.getCode() == KeyEvent::KEY_ESCAPE ) {
		setFullScreen( false );
	}
	else if( event.getChar() == 's' ) {
		mSeed = clock();
		renderNoise();
	}
	else if( event.getChar() == 'o' ) {
		mOctaves--;
		if( mOctaves <= 0 ) mOctaves = 1;
		renderNoise();
	}
	else if( event.getChar() == 'O' ) {
		mOctaves++;
		renderNoise();
	}
	else if( event.getChar() == 'x' ) {
		mSeed = 0;
		renderNoise();
	}
	else if( event.getChar() == 'f' ) {
		mFrequency /= 2.0f;
		renderNoise();
	}
	else if( event.getChar() == 'F' ) {
		mFrequency *= 2.0f;
		renderNoise();
	}
	else if( event.getChar() == 'n' ) {
		mDrawNormalized = ! mDrawNormalized;
	}
	else if( event.getChar() == ' ' ) {
		mPaused = ! mPaused;
	}
	else if( event.getChar() == 'l' ) {
		mDrawLines = ! mDrawLines;
	}
}

void perlinTestApp::update()
{
	if( ! mPaused )
		mTime += 4.0f;
	renderNoise();
}

void perlinTestApp::draw()
{
	cairo::Context ctx( cairo::createWindowSurface() );
	ctx.copySurface( *mNoiseSurface, mNoiseSurface->getBounds() );

	// draw the gradient
	ctx.setSourceRgb( 1.0, 0.5, 0.25 );
	ctx.moveTo( mGradientPos );
	vec3 norm = mPerlin.dfBm( vec3( mGradientPos.x, mGradientPos.y, mTime ) * mFrequency ) * 20.0f;
	ctx.lineTo( mGradientPos.x + norm.x, mGradientPos.y + norm.y );
	ctx.stroke();	

	// draw gradient vector field
	if( mDrawLines ) {
		ctx.setLineWidth( 1.0f );
		ctx.setSourceRgba( 1.0, 1.0, 0.05, 0.25 );
		for( int y = 0; y < getWindowHeight(); y += 20 ) {
			for( int x = 0; x < getWindowWidth(); x += 20 ) {
				ctx.moveTo( vec2( x, y ) );
				vec3 norm = mPerlin.dfBm( vec3( x, y, mTime ) * mFrequency ) * 20.0f;
				if( mDrawNormalized ) {
					if( length2( vec2( norm.x, norm.y ) ) > 0 )
						ctx.lineTo( vec2( x, y ) + normalize( vec2( norm.x, norm.y ) ) * 20.0f );
				}
				else
					ctx.lineTo( vec2( x, y ) + vec2( norm.x, norm.y ) );
				/*float thisVal = mPerlin.fBm( vec3( x, y, mTime ) * mFrequency );
				float rightVal = mPerlin.fBm( vec3( x, y, mTime ) * mFrequency + vec3( 0.001f, 0, 0 ) );
				float downVal = mPerlin.fBm( vec3( x, y, mTime ) * mFrequency + vec3( 0, 0.001f, 0 ) );
				vec2 deriv = vec2( ( rightVal - thisVal ) / 0.001f, ( downVal - thisVal ) / 0.001f );
				ctx.lineTo( vec2( x, y ) + deriv.normalized() * 20.0f );*/
			}
		}
		ctx.stroke();	
	}
}


CINDER_APP( perlinTestApp, Renderer2d )