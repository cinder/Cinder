#include "cinder/app/AppBasic.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"

using namespace ci;
using namespace ci::app;

#include <list>
using std::list;

// We'll create a new Cinder Application by deriving from the BasicApp class
class HDRSampleApp : public AppBasic {
 public:
	void setup();
	void draw();

	gl::GlslProg	mShader;
	gl::Texture		mTexture;
};

Surface32f createHdrRed( int width, int height )
{
	Surface32f result( width, height, false );
	Surface32f::Iter it = result.getIter();
	while( it.line() ) {
		while( it.pixel() ) {
			it.r() = -5 + it.x() / (float)width * 10.0f;
			it.g() = 0;
			it.b() = 0;
		}
	}
	
	return result;
}


const char *vertexShader = 
	"void main()\n"
	"{\n"
		"gl_Position = ftransform();\n"
		"gl_TexCoord[0] = gl_MultiTexCoord0;\n"
	"}\n";

const char *fragmentShader = 
	"uniform sampler2D tex0;\n"
	"void main() {\n"
		"float red = texture2D( tex0, gl_TexCoord[0].st ).r;\n"
		"float green = ( red > 1.0 ) ? 1.0 : 0.0;\n"
		"float blue = ( red < 0.0 ) ? 1.0 : 0.0;\n"
		"gl_FragColor.rgb = vec3( red, green, blue );\n"
	"}\n";

void HDRSampleApp::setup()
{
	mTexture = gl::Texture( createHdrRed( 512, 512 ), GL_RGB32F );
	mShader = gl::GlslProg( vertexShader, fragmentShader );
}

void HDRSampleApp::draw()
{
	mShader.bind();
	mShader.uniform( "tex0", 0 );
	gl::draw( mTexture );
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( HDRSampleApp, RendererGL )
