#include "cinder/app/AppBasic.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;

class ImageFileTestApp : public AppBasic {
 public: 	
	void setup();
	void keyDown( KeyEvent event );
	
	void update();
	void draw();
	
	gl::Texture		mTexture;	
	gl::GlslProg	mShader;
	float			mAngle;
};


void ImageFileTestApp::setup()
{
	try {
		mTexture = gl::Texture( loadImage( loadResource( RES_IMAGE_JPG ) ) );
	}
	catch( ... ) {
		std::cout << "unable to load the texture file!" << std::endl;
	}
	
	try {
		mShader = gl::GlslProg( loadResource( RES_PASSTHRU_VERT ), loadResource( RES_BLUR_FRAG ) );
	}
	catch( gl::GlslProgCompileExc &exc ) {
		std::cout << "Shader compile error: " << std::endl;
		std::cout << exc.what();
	}
	catch( ... ) {
		std::cout << "Unable to load shader" << std::endl;
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

	mTexture.enableAndBind();
	mShader.bind();
	mShader.uniform( "tex0", 0 );
	mShader.uniform( "sampleOffset", Vec2f( cos( mAngle ), sin( mAngle ) ) * ( 3.0f / getWindowWidth() ) );
	gl::drawSolidRect( getWindowBounds() );

	mTexture.unbind();
}


CINDER_APP_BASIC( ImageFileTestApp, RendererGl )