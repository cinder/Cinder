#include "cinder/app/AppBasic.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Camera.h"
#include "cinder/ImageIo.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;

class NormalMappingApp : public AppBasic {
 public:
	void prepareSettings( Settings *settings );
	void setup();
	
	void mouseMove( MouseEvent event );
	
	void drawQuad( ci::Rectf r );
	void draw();
	
	gl::GlslProg	mShader;
	gl::Texture		mDiffuseTex, mSpecStrengthTex, mNormalMapTex;
	vec3			mLightLoc;
	CameraPersp		mCam;
};

void NormalMappingApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 512, 512 );
}

void NormalMappingApp::setup()
{
	mShader = gl::GlslProg( loadResource( RES_NORMAL_MAP_VERT ), loadResource( RES_NORMAL_MAP_FRAG ) );
	mDiffuseTex = gl::Texture( loadImage( loadResource( RES_DIFFUSE_MAP ) ) );
	mSpecStrengthTex = gl::Texture( loadImage( loadResource( RES_SPEC_STRENGTH_MAP ) ) );
	mNormalMapTex = gl::Texture( loadImage( loadResource( RES_NORMAL_MAP_PNG ) ) );
	mLightLoc = vec3( getWindowWidth() / 2.0f, getWindowHeight() / 2.0f, 150 );
	
	mCam.lookAt( vec3( 12.638f, 11.856f, 41.337f ), vec3::zero() );
	mCam.setPerspective( 35, 1.0, 1, 100 );
}

void computeTangent( vec3 v1, vec3 v2, vec2 st1, vec2 st2, vec3 *tangent )
{
	float coef = 1.0f / ( st1.x * st2.y - st2.x * st1.y );
	tangent->x = coef * ((v1.x * st2.y)  + (v2.x * -st1.y));
	tangent->y = coef * ((v1.y * st2.y)  + (v2.y * -st1.y));
	tangent->z = coef * ((v1.z * st2.y)  + (v2.z * -st1.y));
}	

void NormalMappingApp::drawQuad( ci::Rectf r )
{
	GLint tangentUniformLoc = mShader.getAttribLocation( "tangent" );
	vec3 tangent;
	computeTangent( vec3( r.getLowerLeft() - r.getUpperLeft(), 0 ), vec3( r.getUpperRight() - r.getUpperLeft(), 0 ), vec2( 0, 1 ) - vec2( 0, 0 ), vec2( 1, 0 ) - vec2( 0, 0 ), &tangent );
	glBegin( GL_TRIANGLE_STRIP );
		glTexCoord2f( 0, 1 );
		glNormal3f( 0, 0, 1 );
		glVertexAttrib3fv( tangentUniformLoc, &tangent.x );
		gl::vertex( r.getUpperLeft() );

		glTexCoord2f( 0, 0 );
		glNormal3f( 0, 0, 1 );		
		glVertexAttrib3fv( tangentUniformLoc, &tangent.x );
		gl::vertex( r.getLowerLeft() );

		glTexCoord2f( 1, 1 );
		glNormal3f( 0, 0, 1 );
		glVertexAttrib3fv( tangentUniformLoc, &tangent.x );
		gl::vertex( r.getUpperRight() );

		glTexCoord2f( 1, 0 );
		glNormal3f( 0, 0, 1 );
		glVertexAttrib3fv( tangentUniformLoc, &tangent.x );
		gl::vertex( r.getLowerRight() );
	glEnd();
}

void NormalMappingApp::mouseMove( MouseEvent event )
{
	mLightLoc = vec3( ( ( event.getX() / (float)getWindowWidth() ) * 2 - 1 ) * 20, ( ( 0.5f - event.getY() / (float)getWindowHeight() ) * 2 ) * 14, 20 );
//	vec3 lightEyeSpace = mCam.getModelViewMatrix().transformPointAffine( mLightLoc );
//	std::cout << "eye: " << lightEyeSpace << std::endl;
}

void NormalMappingApp::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.1f ) );

	glColor3f( 1, 1, 1 );	
	gl::setMatrices( mCam );
	
	mShader.bind();
	mShader.uniform( "diffuseMap", 0 );
	mShader.uniform( "specMap", 1 );
	mShader.uniform( "normalMap", 2 );
	vec3 lightEyeSpace = mCam.getModelViewMatrix().transformPointAffine( mLightLoc );
	mShader.uniform( "lightLoc", lightEyeSpace );
	mDiffuseTex.bind( 0 );
	mSpecStrengthTex.bind( 1 );	
	mNormalMapTex.bind( 2 );
	drawQuad( ci::Rectf( -10, -10, 10, 10 ) );
	mShader.unbind();
}


CINDER_APP_BASIC( NormalMappingApp, RendererGl )
