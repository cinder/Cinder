#include "cinder/app/App.h"
#include "InfoPanel.h"
#include "cinder/Text.h"
#include "cinder/gl/Texture.h"

using namespace ci;

InfoPanel::InfoPanel()
{
	mState		= true;
	mOpacity	= 1.0f;
	mFadeTime	= 500;
}


void InfoPanel::createTexture()
{
	TextLayout layout;
	
	layout.setFont( Font( "Arial-BoldMT", 14 ) );
	layout.setColor( Color( 1.0f, 1.0f, 1.0f ) );
	layout.addLine( "Frustum Culling" );
	layout.addLine( " " );
	layout.setFont( Font( "ArialMT", 10 ) );
	layout.setColor( Color( 0.6f, 0.6f, 0.6f ) );
	layout.addLine( "1	test spheres" );
	layout.addLine( "2	test cubes" );
	layout.addLine( "3	test points" );
	layout.addLine( "c	switches between cameras" );
	layout.addLine( "f	toggle fullscreen" );
	layout.addLine( "?	toggle information panel" );
	
	mTexture = gl::Texture( layout.render( true ) );
}


void InfoPanel::update( float counter )
{
	if( mState ){
		if( counter == mFadeTime ){
			toggleState();
		}
		mOpacity -= ( mOpacity - 1.0f ) * 0.1f;
	} else {
		mOpacity -= ( mOpacity - 0.0f ) * 0.1f;	
	}
	
	if( mOpacity > 0.01f ){
		render();
	}
}


void InfoPanel::render()
{
	float x = app::getWindowWidth() - mTexture.getWidth() - 40.0f;
	float y = app::getWindowHeight() - mTexture.getHeight() - 25.0f;
	glColor4f( 1, 1, 1, mOpacity );
	gl::draw( mTexture, Vec2f( x, y ) );
}


void InfoPanel::toggleState()
{
	mState = ! mState;
}

