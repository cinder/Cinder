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

using namespace std;
using namespace ci;

const char *vertexShaderGlsl = 
	"void main()\n"
	"{\n"
		"gl_Position = ftransform();\n"
		"gl_TexCoord[0] = gl_MultiTexCoord0;\n"
	"}\n";

const char *voronoiShaderGlsl =
	"#version 120\n"
	"#extension GL_ARB_texture_rectangle: enable\n" 
	"uniform sampler2DRect tex0;\n"
	"uniform vec2 sampleScale;\n"
	"void main() {\n"
	"	vec2 pos[9];\n"
	"	pos[0] = texture2DRect( tex0, gl_TexCoord[0].st + vec2(-1.0,-1.0) * sampleScale ).rg;\n"
	"	pos[1] = texture2DRect( tex0, gl_TexCoord[0].st + vec2(0.0,-1.0) * sampleScale ).rg;\n"
	"	pos[2] = texture2DRect( tex0, gl_TexCoord[0].st + vec2(1.0,-1.0) * sampleScale ).rg;\n"
	"	pos[3] = texture2DRect( tex0, gl_TexCoord[0].st + vec2(-1.0,0.0) * sampleScale ).rg;\n"
	"	pos[4] = texture2DRect( tex0, gl_TexCoord[0].st + vec2(0.0,0.0) * sampleScale ).rg;\n"
	"	pos[5] = texture2DRect( tex0, gl_TexCoord[0].st + vec2(1.0,0.0) * sampleScale ).rg;\n"
	"	pos[6] = texture2DRect( tex0, gl_TexCoord[0].st + vec2(-1.0,1.0) * sampleScale ).rg;\n"
	"	pos[7] = texture2DRect( tex0, gl_TexCoord[0].st + vec2(0.0,1.0) * sampleScale ).rg;\n"
	"	pos[8] = texture2DRect( tex0, gl_TexCoord[0].st + vec2(1.0,1.0) * sampleScale ).rg;\n"
	"	\n"
	"	vec2 smallest = vec2( -1, -1 );\n"
	"	float smallestDistance = 65535.0 * 65535.0;//distance( gl_TexCoord[0].st, pos[0] );\n"
	"	for( int s = 0; s < 9; ++s ) {\n"
// This test seems to blow up the 8800 under Mac OS X 10.5.8 quite handily, but it's not necessary if we seed the field w/ large negative closest sites	
//	"		if( pos[s].x < 0 ) continue;\n
	"		float distance = distance( gl_TexCoord[0].st, pos[s] );\n"
	"		if( distance < smallestDistance ) {\n"
	"			smallestDistance = distance;\n"
	"			smallest = pos[s];\n"
	"		}\n"
	"	}\n"
	"\n"
	"	gl_FragColor.rg = smallest;\n"
	"}\n";
	
const char *distanceShaderGlsl =
	"uniform sampler2DRect tex0;\n"
	"void main() {\n"
	"	gl_FragColor.rgb = vec3( distance( texture2DRect( tex0, gl_TexCoord[0].st ).rg, gl_TexCoord[0].st ) );\n"
	"}\n";

gl::Texture encodePoints( const vector<Vec2i> &points, int width, int height )
{
	Surface32f result( width, height, false );
	ip::fill( &result, Colorf( -65535.0f, -65535.0f, 0 ) ); // seed the result with a huge distance that will easily be "beaten" by any given site
	for( vector<Vec2i>::const_iterator ptIt = points.begin(); ptIt != points.end(); ++ptIt )
		result.setPixel( *ptIt, Color( (float)ptIt->x, (float)ptIt->y, 0 ) );
	
	return gl::Texture( result );
}

ci::Surface32f calcDiscreteVoronoiGpu( const std::vector<ci::Vec2i> &points, int width, int height )
{
	static gl::GlslProg voronoiShader( vertexShaderGlsl, voronoiShaderGlsl );
	
	// allocate the FBOs
	gl::Fbo::Format format;
	format.setTarget( GL_TEXTURE_RECTANGLE_ARB );
	format.setColorInternalFormat( GL_RGB32F_ARB );
	format.enableDepthBuffer( false );
	format.setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
	format.setMinFilter( GL_NEAREST );
	format.setMagFilter( GL_NEAREST );
	gl::Fbo fbo[2];
	fbo[0] = gl::Fbo( width, height, format );
	fbo[1] = gl::Fbo( width, height, format );
	
	// draw the encoded points into FBO 1
	fbo[0].bindFramebuffer();
	gl::setMatricesWindow( fbo[0].getSize(), false );
	gl::draw( encodePoints( points, width, height ) );
	
	// ping-pong between the two FBOs
	voronoiShader.bind();
	voronoiShader.uniform( "tex0", 0 );
	int curFbo;
	int numPasses = log2ceil( std::max( width, height ) );
	for( int pass = 1; pass <= numPasses; ++pass ) {
		voronoiShader.uniform( "sampleScale", Vec2f( 1, 1 ) * (float)( 1 << ( numPasses - pass ) ) );
		curFbo = pass % 2;
		fbo[curFbo].bindFramebuffer();
		fbo[(curFbo+1)%2].bindTexture();
		gl::drawSolidRect( fbo[0].getBounds(), true );
	}
	
	fbo[curFbo].unbindFramebuffer();
	voronoiShader.unbind();

	// now curFbo contains the last pass of the voronoi diagram
	return Surface32f( fbo[curFbo].getTexture() );
}

ci::Channel32f calcDistanceMapGpu( const vector<Vec2i> &points, int width, int height )
{
	static gl::GlslProg voronoiShader( vertexShaderGlsl, voronoiShaderGlsl );
	static gl::GlslProg distanceShader( vertexShaderGlsl, distanceShaderGlsl );
	
	// allocate the FBOs
	gl::Fbo::Format format;
	format.setTarget( GL_TEXTURE_RECTANGLE_ARB );
	format.setColorInternalFormat( GL_RGB32F_ARB );
	format.enableDepthBuffer( false );
	format.setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
	format.setMinFilter( GL_NEAREST );
	format.setMagFilter( GL_NEAREST );
	gl::Fbo fbo[2];
	fbo[0] = gl::Fbo( width, height, format );
	fbo[1] = gl::Fbo( width, height, format );
	
	// draw the encoded points into FBO 1
	fbo[0].bindFramebuffer();
	gl::setMatricesWindow( fbo[0].getSize(), false );
	gl::draw( encodePoints( points, width, height ) );
	
	// ping-pong between the two FBOs
	voronoiShader.bind();
	voronoiShader.uniform( "tex0", 0 );
	int curFbo;
	int numPasses = log2ceil( std::max( width, height ) );
	for( int pass = 1; pass <= numPasses; ++pass ) {
		voronoiShader.uniform( "sampleScale", Vec2f( 1, 1 ) * (float)( 1 << ( numPasses - pass ) ) );
		curFbo = pass % 2;
		fbo[curFbo].bindFramebuffer();
		fbo[(curFbo+1)%2].bindTexture();
		gl::drawSolidRect( fbo[0].getBounds(), true );
	}

	// now curFbo contains the last pass of the voronoi diagram; bind that as the texture
	// and render a quad using the distance shader
	distanceShader.bind();
	distanceShader.uniform( "tex0", 0 );
	
	fbo[(curFbo+1)%2].bindFramebuffer();
	fbo[curFbo].bindTexture();
	gl::drawSolidRect( fbo[0].getBounds(), true );
	fbo[(curFbo+1)%2].unbindFramebuffer();
	distanceShader.unbind();
	
	return Channel32f( fbo[(curFbo+1)%2].getTexture() );
}