#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/Text.h"

static int sCount = 0;
using namespace ci;
using namespace ci::app;

//#define SNAPSHOT_FBO

class HeadlessTestApp : public App {
  public:
		void setup() final;
		void draw() final;

		void renderSnapshot();
		void saveSnapshot( const fs::path& path );
  private:
#if defined( SNAPSHOT_FBO )
		ci::gl::FboRef			mFbo;
#endif
		gl::TextureRef			mTextTexture;
		ci::Font				mFont;
};

void HeadlessTestApp::setup()
{
#if defined( SNAPSHOT_FBO )
	mFbo = ci::gl::Fbo::create( getWindowSize().x, getWindowSize().y, ci::gl::Fbo::Format().colorTexture() );
#endif
	mFont = ci::Font( "Times New Roman", 64 );
}

void HeadlessTestApp::renderSnapshot()
{
	std::string txt = "Cinder is rendering headless!";
	TextBox tbox = TextBox().alignment( TextBox::CENTER ).font( mFont ).size( ivec2( TextBox::GROW, TextBox::GROW ) ).text( txt );
	tbox.setColor( Color( 1.0f, 0.55f, 0.35f ) );
	mTextTexture = gl::Texture2d::create( tbox.render() );
	if( mTextTexture )
		gl::draw( mTextTexture, ci::vec2( getWindowSize().x * .5f - mTextTexture->getSize().x * .5f, getWindowSize().y * .5 ) );
}

void prepareSettings( HeadlessTestApp::Settings* settings )
{
	settings->setWindowSize( ci::vec2( 1920, 1080 ) );
}

void HeadlessTestApp::saveSnapshot( const fs::path& path ) 
{
	if( ! path.empty() ) {
#if defined( SNAPSHOT_FBO )
		if( mFbo ) {
			Surface s8( mFbo->getColorTexture()->createSource() );
			writeImage( writeFile( path ), s8 );
		}
#else
		writeImage( writeFile( path ), copyWindowSurface() );
#endif
		console() << "Saved snapshot : " << path << std::endl;
	}
}

void HeadlessTestApp::draw()
{
	if( ( sCount % 120	) == 0 ) {
#if defined( SNAPSHOT_FBO )
		ci::gl::ScopedFramebuffer scopedFbo( mFbo );
#endif
		ci::gl::clear( ci::Color( .0f, .2f, 1.0f ) );
		gl::setMatricesWindow( getWindowSize() );
		gl::enableAlphaBlending();
		renderSnapshot();
		saveSnapshot( "headless-snapshot.png" );
	}
	sCount++;
}

CINDER_APP( HeadlessTestApp, RendererGl, prepareSettings )
