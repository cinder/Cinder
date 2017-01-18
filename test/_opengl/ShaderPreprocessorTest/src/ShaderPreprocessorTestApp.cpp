/*


* [X] check all cases:
* 
* [X] version, no defines
* [X] no version, no defines case
* [X] version, with defines case
* [X] no version, with defines case
* [X] version with a comment before it, no defines
* [X] version with comment before it, defines
* [X] no version, with defines and comment before it
* [X] no version, no defines, comment before it
* [X] version, no defines, with include
* [X] version, no defines, with include, comment on top
* [X] no version, no defines, with include
* [X] no version, no defines, with include, comment at top
* [X] version, defines, include
* [X] original more complicated shader
* 
* [X] test that the source-string-number is correct here, should go up to 2

* variations:
* 
* [X] versions 150 and 330+ handle #lines differently
* [X] check #version 300 es that it is parsed correctly

*/

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/ShaderPreprocessor.h"
#include "cinder/Log.h"
#include "cinder/System.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ShaderPreprocessorTestApp : public App {
	void setup() override;
	void keyDown( KeyEvent event ) override;
	void update() override;
	void draw() override;

	void reload();
	void testGlslProgInclude();
	void testSeparateShaderPreprocessor();
	void testIncludeHandler();

	gl::GlslProgRef				mGlslProg;
	gl::ShaderPreprocessorRef  mPreprocessor;
	bool						mAddDefineDirective = false;
	const std::string			mWrongHashDefineDirective = "WRONG_HASH";
};

void ShaderPreprocessorTestApp::setup()
{
	mPreprocessor = make_shared<gl::ShaderPreprocessor>();

	if( mAddDefineDirective )
		mPreprocessor->addDefine( mWrongHashDefineDirective );

	reload();
}

void ShaderPreprocessorTestApp::reload()
{
//	testGlslProgInclude();
	testSeparateShaderPreprocessor();
//	testIncludeHandler();
}

void ShaderPreprocessorTestApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'r' ) {
		CI_LOG_I( "reload" );
		reload();
	}
	else if( event.getChar() == 'd' ) {
		mAddDefineDirective = ! mAddDefineDirective;
		CI_LOG_I( "mAddDefineDirectives: " << mAddDefineDirective );
		if( mAddDefineDirective ) {
			mPreprocessor->addDefine( mWrongHashDefineDirective );
		}
		else {
			mPreprocessor->clearDefines();
		}
	}
	else if( event.getChar() == 'v' ) {
		mPreprocessor->setVersion( mPreprocessor->getVersion() == 150 ? 330 : 150 );
		CI_LOG_I( "set #version to: " << mPreprocessor->getVersion() );
	}
	else if( event.getChar() == 'n' ) {
		mPreprocessor->setUseFilenameInLineDirectiveEnabled( ! mPreprocessor->isUseFilenameInLineDirectiveEnabled() );
		CI_LOG_I( "using filename in #line directive: " << boolalpha << mPreprocessor->isUseFilenameInLineDirectiveEnabled() << dec );
	}
}

void ShaderPreprocessorTestApp::testGlslProgInclude()
{
	try {
		auto format = gl::GlslProg::Format()
							.vertex(  loadAsset( "passthrough.vert" ) )
							.fragment( loadAsset( "shaderWithInclude.frag" ) )
//                            .define( "COLOR_RED", "vec4( 0, 0, 1, 1 )" )
        ;

		if( mAddDefineDirective ) {
			format.define( mWrongHashDefineDirective );
		}

		mGlslProg = gl::GlslProg::create( format );
	}
	catch( std::exception &exc ) {
		CI_LOG_EXCEPTION( "exception caught.", exc );
	}

}

void ShaderPreprocessorTestApp::testSeparateShaderPreprocessor()
{
	try {
		auto vert = loadAsset( "passthrough.vert" );
		const fs::path &fragPath = "simple.frag";
		//const fs::path &fragPath = "shaderWithInclude.frag";
		string fragSourceRaw = loadString( loadAsset( fragPath ) );
		string fragSource = mPreprocessor->parse( fragSourceRaw, fragPath );

		// write the preprocessed string out to file within the build folder for comparison
#if defined( CINDER_MAC )
		writeString( fs::path( fragPath.stem().string() + "_preprocessed.frag" ), fragSource );
#else
		writeString( fs::path( "build" ) / ( fragPath.stem().string() + "_preprocessed.frag" ), fragSource );
#endif

		auto format = gl::GlslProg::Format()
							.preprocess( false )
							.vertex( vert )
							.fragment( fragSource )
		;

		mGlslProg = gl::GlslProg::create( format );
	}
	catch( std::exception &exc ) {
		CI_LOG_EXCEPTION( "exception caught", exc );
	}

}

const std::string hashGlsl = R"(
float hash( float n )
{
	return 0.5f; // should result in the pattern being solid, no noise
}
)";

void ShaderPreprocessorTestApp::testIncludeHandler()
{
	auto includeCustomHash = []( const fs::path &path,std::string *source ) -> bool {
		if( path == fs::path( "hash.glsl" ) ) {
			*source = hashGlsl;
			return true;
		}

		return false;
	};

	try {
		auto format = gl::GlslProg::Format()
			.vertex( loadAsset( "passthrough.vert" ) )
			.fragment( loadAsset( "shaderWithInclude.frag" ) )
		;

		format.getPreprocessor()->getSignalInclude().connect( includeCustomHash );

		mGlslProg = gl::GlslProg::create( format );
	}
	catch( std::exception &exc ) {
		CI_LOG_EXCEPTION( "exception caught", exc );
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

void prepareSettings( app::App::Settings *settings )
{
	if( Display::getDisplays().size() > 1 )
		settings->setDisplay( Display::getDisplays()[1] );
}

CINDER_APP( ShaderPreprocessorTestApp, RendererGl, prepareSettings )
