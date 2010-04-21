#include "QuickTimeApp.h"

using namespace fli;

void QuickTimeApp::setup( app::App *app )
{
	const std::string moviePath = app->getAppPath() + std::string( "/Contents/Resources/" ) + std::string( "Lexus.mov" );
	mMovie = shared_ptr<qtime::MovieGLTexture>( new qtime::MovieGLTexture( moviePath, app->getCGLContext(), app->getCGLPixelFormat() ) );
	mMovie->setLoop( true );
}

void QuickTimeApp::update( app::App *app, bool visible )
{
	if( ! visible ) return;

	if( mMovie ) {
		mMovie->stepForward();
		mTexture = &mMovie->getTexture();
	}
	else
		mTexture = 0;
}

void QuickTimeApp::draw( app::App *app, const Vec4d &camLoc )
{
	gl::enableAlphaBlending();
	glColor4f( 1, 1, 1, 0.65f );
	if( mTexture ) {
		glScalef( 600 * ( mTexture->getHeight() / (float)mTexture->getWidth() ), 600, 1 );
		gl::rotate( Quatf( Vec3f::zAxis(), toRadians( 90.0f ) ) );
		mTexture->enableAndBind();
		glBegin( GL_QUADS );
			glTexCoord2f( mTexture->getLeft(), mTexture->getBottom() );
			glVertex3f( -0.5f, -0.5, 1 );
			glTexCoord2f( mTexture->getRight(), mTexture->getBottom() );
			glVertex3f( 0.5f, -0.5, 1 );
			glTexCoord2f( mTexture->getRight(), mTexture->getTop() );
			glVertex3f( 0.5f, 0.5, 1 );
			glTexCoord2f( mTexture->getLeft(), mTexture->getTop() );
			glVertex3f( -0.5f, 0.5, 1 );
		glEnd();
		glDisable( mTexture->getTarget() );
	}
}