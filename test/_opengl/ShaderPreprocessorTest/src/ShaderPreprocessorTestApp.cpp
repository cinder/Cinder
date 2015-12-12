#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/ShaderPreprocessor.h"
#include "cinder/Log.h"
#include "cinder/System.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ShaderPreprocessorTestApp : public App {
	void setup() override;
	void keyDown( KeyEvent event ) override;
	void update() override;
	void draw() override;

	void testGlslProgInclude();
	void testSeparateShaderPreprocessor();

	gl::GlslProgRef			mGlslProg;
	gl::ShaderPreprocessor  mPreprocessor;
};

void ShaderPreprocessorTestApp::setup()
{
	testGlslProgInclude();
//	testSeparateShaderPreprocessor();
}

void ShaderPreprocessorTestApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'r' ) {
		CI_LOG_V( "reload" );
		setup();
	}
}

void ShaderPreprocessorTestApp::testGlslProgInclude()
{
	try {
		auto format = gl::GlslProg::Format()
							.vertex(  loadAsset( "passthrough.vert" ) )
							.fragment( loadAsset( "shaderWithInclude.frag" ) )
        //                    .define( "COLOR_RED", "vec4( 0, 0, 1, 1 )" )
                            .define( "WRONG_HASH" )
        ;

		mGlslProg = gl::GlslProg::create( format );
	}
	catch( std::exception &exc ) {
		CI_LOG_E( "exception caught, type: " << System::demangleTypeName( typeid( exc ).name() ) << ", what: " << exc.what() );
	}

}

void ShaderPreprocessorTestApp::testSeparateShaderPreprocessor()
{
	try {
		auto vert = loadAsset( "passthrough.vert" );
        string fragSource = mPreprocessor.parse( getAssetPath( "shaderWithInclude.frag" ) );

		auto format = gl::GlslProg::Format()
							.preprocess( false )
							.vertex( vert )
							.fragment( fragSource );

		mGlslProg = gl::GlslProg::create( format );
	}
	catch( std::exception &exc ) {
		CI_LOG_E( "exception caught, type: " << System::demangleTypeName( typeid( exc ).name() ) << ", what: " << exc.what() );
	}

}

void ShaderPreprocessorTestApp::update()
{
}

void ShaderPreprocessorTestApp::draw()
{
	gl::clear( Color( 0.1, 0.2, 0.1 ) );

	if( mGlslProg ) {
		gl::ScopedGlslProg glslScope( mGlslProg );
		gl::drawSolidRect( getWindowBounds() );
	}
}

CINDER_APP( ShaderPreprocessorTestApp, RendererGl )
