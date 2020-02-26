// This code implements the paper "Jump Flooding in GPU with Applications to Voronoi Diagram and Distance Transform"
// by Guodong Rong and Tiow-Seng Tan
//
// http://www.comp.nus.edu.sg/~tants/jfa.html

#include "VoronoiGpu.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/ip/Fill.h"
#include "cinder/Rand.h"
#include "cinder/app/App.h"

using namespace std;
using namespace ci;

const char *vertexShaderGlsl = CI_GLSL( 150,
	uniform mat4 ciModelViewProjection;
	in vec4 ciPosition;
	in vec2 ciTexCoord0;
	out vec2 TexCoord0;
	
	void main()
	{
		gl_Position = ciModelViewProjection * ciPosition;
		TexCoord0 = ciTexCoord0;
	}
);

const char *voronoiShaderGlsl = CI_GLSL( 150,
	uniform sampler2DRect	uTex0;
	uniform vec2			uSampleScale;
	in vec2 TexCoord0;
	out vec3 oColor;
	void main() {
		vec2 pos[9];
		pos[0] = texture( uTex0, TexCoord0.st + vec2(-1.0,-1.0) * uSampleScale ).rg;
		pos[1] = texture( uTex0, TexCoord0.st + vec2(0.0,-1.0) * uSampleScale ).rg;
		pos[2] = texture( uTex0, TexCoord0.st + vec2(1.0,-1.0) * uSampleScale ).rg;
		pos[3] = texture( uTex0, TexCoord0.st + vec2(-1.0,0.0) * uSampleScale ).rg;
		pos[4] = texture( uTex0, TexCoord0.st + vec2(0.0,0.0) * uSampleScale ).rg;
		pos[5] = texture( uTex0, TexCoord0.st + vec2(1.0,0.0) * uSampleScale ).rg;
		pos[6] = texture( uTex0, TexCoord0.st + vec2(-1.0,1.0) * uSampleScale ).rg;
		pos[7] = texture( uTex0, TexCoord0.st + vec2(0.0,1.0) * uSampleScale ).rg;
		pos[8] = texture( uTex0, TexCoord0.st + vec2(1.0,1.0) * uSampleScale ).rg;

		vec2 smallest = vec2( -1, -1 );
		float smallestDistance = 65535.0 * 65535.0;
		for( int s = 0; s < 9; ++s ) {
// This test seems to blow up the 8800 under Mac OS X 10.5.8 quite handily, but it's not necessary if we seed the field w/ large negative closest sites	
//	"		if( pos[s].x < 0 ) continue;\n
			float dist = distance( TexCoord0.st, pos[s] );
			if( dist < smallestDistance ) {
				smallestDistance = dist;
				smallest = pos[s];
			}
		}
	
		oColor.rgb = vec3( smallest, 1.0 );
	}
);
	
const char *distanceShaderGlsl = CI_GLSL( 150,
	uniform sampler2DRect uTex0;
	in vec2 TexCoord0;
	out vec3 oColor;
	
	void main() {
		oColor.rgb = vec3( distance( texture( uTex0, TexCoord0.st ).rg, TexCoord0.st ) );
	}
);

gl::TextureRef encodePoints( const vector<ivec2> &points, int width, int height )
{
	Surface32f result( width, height, false );
	ip::fill( &result, Colorf( -65535.0f, -65535.0f, 0 ) ); // seed the result with a huge distance that will easily be "beaten" by any given site
	for( vector<ivec2>::const_iterator ptIt = points.begin(); ptIt != points.end(); ++ptIt ) {
		vec2 bottomUpPoint = *ptIt;
		bottomUpPoint.y = height - bottomUpPoint.y;
		result.setPixel( bottomUpPoint, Color( bottomUpPoint.x, bottomUpPoint.y, 0 ) );
	}
	
	return gl::Texture::create( result, gl::Texture::Format().target( GL_TEXTURE_RECTANGLE_ARB ) );
}

ci::Surface32f calcDiscreteVoronoiGpu( const std::vector<ci::ivec2> &points, int width, int height )
{
	static gl::GlslProgRef voronoiShader = gl::GlslProg::create( vertexShaderGlsl, voronoiShaderGlsl );
	
	// allocate the FBOs
	gl::Fbo::Format format;
	format.setColorTextureFormat( gl::Texture::Format().target( GL_TEXTURE_RECTANGLE_ARB ).minFilter( GL_NEAREST ).
		magFilter( GL_NEAREST ).internalFormat( GL_RGB32F ).wrap( GL_CLAMP_TO_EDGE ) );
	gl::FboRef fbo[2];
	fbo[0] = gl::Fbo::create( width, height, format );
	fbo[1] = gl::Fbo::create( width, height, format );
	
	// draw the encoded points into FBO 1
	fbo[0]->bindFramebuffer();
	gl::setMatricesWindow( fbo[0]->getSize() );
	gl::draw( encodePoints( points, width, height ) );
	
	// ping-pong between the two FBOs
	gl::ScopedGlslProg scpGlsl( voronoiShader );
	voronoiShader->uniform( "uTex0", 0 );
	int curFbo = 0;
	int numPasses = log2ceil( std::max( width, height ) );
	for( int pass = 1; pass <= numPasses; ++pass ) {
		voronoiShader->uniform( "uSampleScale", vec2( 1, 1 ) * (float)( 1 << ( numPasses - pass ) ) );
		curFbo = pass % 2;
		fbo[curFbo]->bindFramebuffer();
		fbo[(curFbo+1)%2]->bindTexture();
		gl::drawSolidRect( fbo[0]->getBounds(), vec2( 0, fbo[0]->getHeight() ), vec2( fbo[0]->getWidth(), 0 ) );
	}
	
	fbo[curFbo]->unbindFramebuffer();

	// now curFbo contains the last pass of the voronoi diagram
	return Surface32f( fbo[curFbo]->getColorTexture()->createSource() );
}

ci::Channel32f calcDistanceMapGpu( const vector<ivec2> &points, int width, int height )
{
	static gl::GlslProgRef voronoiShader = gl::GlslProg::create( vertexShaderGlsl, voronoiShaderGlsl );
	static gl::GlslProgRef distanceShader = gl::GlslProg::create( vertexShaderGlsl, distanceShaderGlsl );
	
	// allocate the FBOs
	gl::Fbo::Format format;
	format.setColorTextureFormat( gl::Texture::Format().target( GL_TEXTURE_RECTANGLE_ARB ).minFilter( GL_NEAREST ).
		magFilter( GL_NEAREST ).internalFormat( GL_RGB32F ).wrap( GL_CLAMP_TO_EDGE ) );
	gl::FboRef fbo[2];
	fbo[0] = gl::Fbo::create( width, height, format );
	fbo[1] = gl::Fbo::create( width, height, format );
	
	// draw the encoded points into FBO 1
	fbo[0]->bindFramebuffer();
	gl::setMatricesWindow( fbo[0]->getSize() );
	gl::draw( encodePoints( points, width, height ) );
	
	// ping-pong between the two FBOs
	gl::ScopedGlslProg vorGlslScp( voronoiShader );
	voronoiShader->uniform( "uTex0", 0 );
	int curFbo = 0;
	int numPasses = log2ceil( std::max( width, height ) );
	for( int pass = 1; pass <= numPasses; ++pass ) {
		voronoiShader->uniform( "uSampleScale", vec2( 1, 1 ) * (float)( 1 << ( numPasses - pass ) ) );
		curFbo = pass % 2;
		fbo[curFbo]->bindFramebuffer();
		fbo[(curFbo+1)%2]->bindTexture();
		gl::drawSolidRect( fbo[0]->getBounds(), vec2( 0, fbo[0]->getHeight() ), vec2( fbo[0]->getWidth(), 0 ) );
	}

	// now curFbo contains the last pass of the voronoi diagram; bind that as the texture
	// and render a quad using the distance shader
	{
		gl::ScopedFramebuffer scpFb( fbo[(curFbo+1)%2] );
		gl::ScopedTextureBind scpBind( fbo[curFbo]->getColorTexture() );
		gl::ScopedGlslProg scpGlsl( distanceShader );
		distanceShader->uniform( "uTex0", 0 );
		
		gl::drawSolidRect( fbo[0]->getBounds(), vec2( 0, fbo[0]->getHeight() ), vec2( fbo[0]->getWidth(), 0 ) );
	}

	fbo[0]->unbindFramebuffer();
	
	return Channel32f( fbo[(curFbo+1)%2]->getColorTexture()->createSource() );
}