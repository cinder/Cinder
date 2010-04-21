#include "BoxSpinApp.h"

using namespace fli;

void BoxSpinApp::setup( app::App *app )
{
	mAngle = 0;
	mAlive = false;
	
	mShadowTexture = new gl::Texture( loadImage( app->loadResource( "shadow.png" ) ) );
	mLogoTexture = new gl::Texture( loadImage( app->loadResource( "ford.jpg" ) ) );	
	mBoxShader = new gl::GLSLProg( app->loadResource( "box.vert" ), app->loadResource( "box.frag" ) );
}

void BoxSpinApp::update( app::App *app, bool visible )
{
	if( visible ) {
		if( ! mAlive ) {
			mAlive = true;
			mAge = 1.0f / app->getFrameRate();
		}
		else
			mAge += 1.0f / app->getFrameRate();
	}
	else
		mAge = 0;
	
	if( mAge > 1.0f )
		mAngle += 0.06f;
	else
		mAngle += 0.48f * powf( 1.0f - mAge, 1.3 ) + 0.06f;
}

void drawCube( const Vec3f &center, const Vec3f &size )
{
	Vec3f halfSize = size / 2;
	glBegin( GL_QUADS );
		
		glNormal3f( 0.0f, 1.0f, 0.0f);
		glTexCoord2f( 0.0f, 1.0f );	glVertex3fv( center + Vec3f( 1.0f, 1.0f, -1.0f ) * halfSize );
		glTexCoord2f( 1.0f, 1.0f );	glVertex3fv( center + Vec3f( -1.0f, 1.0f,-1.0f) * halfSize );			
		glTexCoord2f( 1.0f, 0.0f );	glVertex3fv( center + Vec3f( -1.0f, 1.0f, 1.0f) * halfSize );			
		glTexCoord2f( 0.0f, 0.0f );	glVertex3fv( center + Vec3f(  1.0f, 1.0f, 1.0f) * halfSize );			
		
		glNormal3f( 0.0f, -1.0f, 0.0f);
		glTexCoord2f( 1.0f, 0.0f );	glVertex3fv( center + Vec3f(  1.0f,-1.0f, 1.0f) * halfSize );			
		glTexCoord2f( 0.0f, 0.0f );	glVertex3fv( center + Vec3f( -1.0f,-1.0f, 1.0f) * halfSize );			
		glTexCoord2f( 0.0f, 1.0f );	glVertex3fv( center + Vec3f( -1.0f,-1.0f,-1.0f) * halfSize );			
		glTexCoord2f( 1.0f, 1.0f );	glVertex3fv( center + Vec3f(  1.0f,-1.0f,-1.0f) * halfSize );			
		
		glNormal3f( 0.0f, 0.0f, 1.0f);
		glTexCoord2f( 1.0f, 0.0f );	glVertex3fv( center + Vec3f(  1.0f, 1.0f, 1.0f) * halfSize );			
		glTexCoord2f( 0.0f, 0.0f );	glVertex3fv( center + Vec3f( -1.0f, 1.0f, 1.0f) * halfSize );			
		glTexCoord2f( 0.0f, 1.0f );	glVertex3fv( center + Vec3f( -1.0f,-1.0f, 1.0f) * halfSize );			
		glTexCoord2f( 1.0f, 1.0f );	glVertex3fv( center + Vec3f(  1.0f,-1.0f, 1.0f) * halfSize );			
		
		glNormal3f( 0.0f, 0.0f, -1.0f);
		glTexCoord2f( 1.0f, 1.0f );	glVertex3fv( center + Vec3f(  1.0f,-1.0f,-1.0f) * halfSize );			
		glTexCoord2f( 0.0f, 1.0f );	glVertex3fv( center + Vec3f( -1.0f,-1.0f,-1.0f) * halfSize );			
		glTexCoord2f( 0.0f, 0.0f );	glVertex3fv( center + Vec3f( -1.0f, 1.0f,-1.0f) * halfSize );			
		glTexCoord2f( 1.0f, 0.0f );	glVertex3fv( center + Vec3f(  1.0f, 1.0f,-1.0f) * halfSize );			
		
		glNormal3f( -1.0f, 0.0f, 0.0f);
		glTexCoord2f( 0.0f, 0.0f ); glVertex3fv( center + Vec3f( -1.0f, 1.0f, 1.0f) * halfSize );			
		glTexCoord2f( 0.0f, 1.0f );	glVertex3fv( center + Vec3f( -1.0f, 1.0f,-1.0f) * halfSize );			
		glTexCoord2f( 1.0f, 1.0f );	glVertex3fv( center + Vec3f( -1.0f,-1.0f,-1.0f) * halfSize );			
		glTexCoord2f( 1.0f, 0.0f );	glVertex3fv( center + Vec3f( -1.0f,-1.0f, 1.0f) * halfSize );			
		
		glNormal3f( 1.0f, 0.0f, 0.0f);
		glTexCoord2f( 1.0f, 1.0f );	glVertex3fv( center + Vec3f(  1.0f, 1.0f,-1.0f) * halfSize );			
		glTexCoord2f( 1.0f, 0.0f );	glVertex3fv( center + Vec3f(  1.0f, 1.0f, 1.0f) * halfSize );			
		glTexCoord2f( 0.0f, 0.0f );	glVertex3fv( center + Vec3f(  1.0f,-1.0f, 1.0f) * halfSize );			
		glTexCoord2f( 0.0f, 1.0f );	glVertex3fv( center + Vec3f(  1.0f,-1.0f,-1.0f) * halfSize );			
	glEnd();
}

void BoxSpinApp::draw( app::App *app, const Vec4d &camLoc )
{
	gl::enableAlphaBlending();
	gl::enableDepthWrite();
	gl::enableDepthRead();
	glColor4f( 1, 1, 1, 0.65f );

	float size = 200 * std::min( 1.0f, mAge );
	gl::rotate( Quatf( Vec3f::zAxis(), mAngle ) );

	glColor4f( 1, 1, 1, 1.0f );
	float shadowScale = 2.2f;
	mShadowTexture->draw( -size * shadowScale / 2, -size * shadowScale / 2, size * shadowScale, size * shadowScale );
	mShadowTexture->disable();
	
	mLogoTexture->enableAndBind();
	mBoxShader->bind();
	mBoxShader->uniform( "camLoc", Vec3f( camLoc.x, camLoc.y, camLoc.z ) );
	mBoxShader->uniform( "tex0", 0 );
	drawCube( Vec3f( 0, 0, size / 2 + size * 0.2f ), Vec3f( size, size, size ) );
	mLogoTexture->disable();
	mBoxShader->unbind();
}