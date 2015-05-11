#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Texture1dApp : public App {
  public:
	void setup() override;
	void draw() override;
	
	gl::Texture1dRef	mTex;
	gl::GlslProgRef		mShader;
};

void Texture1dApp::setup()
{
	mTex = gl::Texture1d::create( loadImage( loadAsset( "gradient.png" ) ) );
	
	mShader = gl::GlslProg::create( gl::GlslProg::Format()
		.vertex(	CI_GLSL( 150,
			uniform mat4	ciModelViewProjection;
			in vec4			ciPosition;
			in float		ciTexCoord0;
			
			out float		TexCoord0;
			
			void main( void ) {
				gl_Position	= ciModelViewProjection * ciPosition;
				TexCoord0 = ciTexCoord0;
			}
		 ) )
		.fragment(	CI_GLSL( 150,
			uniform sampler1D	uTex0;
			in float			TexCoord0;
			out vec4			oColor;
			
			void main( void ) {
				oColor = texture( uTex0, TexCoord0 ).rgba;
			}
		) ) );
	
}

void Texture1dApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	
	mTex->bind();
	gl::ScopedGlslProg scpGlsl( mShader );
	mShader->uniform( "uTex0", 0 );
	gl::drawSolidRect( getWindowBounds() );
}

CINDER_APP( Texture1dApp, RendererGl )
