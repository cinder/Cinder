#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Batch.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ImmediateModeApp : public AppNative {
  public:
	void setup();
	void draw();

	gl::GlslProgRef		mGlsl;
};

void ImmediateModeApp::setup()
{
	mGlsl = gl::GlslProg::create( gl::GlslProg::Format()
		.vertex(	CI_GLSL( 150,
			uniform mat4	ciModelViewProjection;
			in vec4			ciPosition;
			in vec3			ciColor;
			out vec3		Color;
			
			void main( void ) {
				gl_Position	= ciModelViewProjection * ciPosition;
				Color = ciColor;
			}
		 ) )
		.fragment(	CI_GLSL( 150,
			in vec3		Color;
			out vec3	oColor;
			
			void main( void ) {
				oColor = Color;
			}
		) ) );
}

void ImmediateModeApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );	

	mGlsl->bind();
	gl::VertBatch vb( GL_TRIANGLES );
		vb.color( 1, 0, 0 );
		vb.vertex( getWindowWidth() / 2, 50 );
		vb.color( 0, 1, 0 );
		vb.vertex( getWindowWidth() - 50, getWindowHeight() - 50 );
		vb.color( 0, 0, 1 );
		vb.vertex( 50, getWindowHeight() - 50 );
	vb.draw();
}

CINDER_APP_NATIVE( ImmediateModeApp, RendererGl )
