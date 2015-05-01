#include "cinder/app/App.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;

class ImageFileTestApp : public App {
 public: 	
	void setup();
	void keyDown( KeyEvent event );
	
	void update();
	void draw();
	
	gl::TextureRef	mTexture;	
	gl::GlslProgRef	mShader;
	float			mAngle;
};


void ImageFileTestApp::setup()
{
	try {
		mTexture = gl::Texture::create( loadImage( loadResource( RES_IMAGE_JPG ) ) );
	}
	catch( ci::Exception &exc ) {
		std::cout << "unable to load the texture file, what: " << exc.what() << std::endl;
	}
	
	try {
		mShader = gl::GlslProg::create( loadResource( RES_PASSTHRU_VERT ), loadResource( RES_BLUR_FRAG ) );
	}
	catch( gl::GlslProgCompileExc &exc ) {
		std::cout << "Shader compile error: " << std::endl;
		std::cout << exc.what();
	}
	catch( ci::Exception &exc ) {
		std::cout << "Unable to load shader, what: " << exc.what() << std::endl;
	}
	
	mAngle = 0.0f;
}

void ImageFileTestApp::keyDown( KeyEvent event )
{
	if( event.getCode() == app::KeyEvent::KEY_f ) {
		setFullScreen( ! isFullScreen() );
	}
}

void ImageFileTestApp::update()
{
	mAngle += 0.05f;
}

void ImageFileTestApp::draw()
{
	gl::clear();

	mTexture->enableAndBind();
	mShader->bind();
	mShader->uniform( "tex0", 0 );
	mShader->uniform( "sampleOffset", vec2( cos( mAngle ), sin( mAngle ) ) * ( 3.0f / getWindowWidth() ) );
	gl::drawSolidRect( getWindowBounds() );

	mTexture->unbind();
}


CINDER_APP( ImageFileTestApp, RendererGl )