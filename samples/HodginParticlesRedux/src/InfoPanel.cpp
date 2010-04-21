
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
	layout.setColor( Color( 1.0f, 1.0f, 0.0f ) );
	layout.addLine( "Hodgin Particles Redux" );
	
	layout.setFont( Font( "ArialMT", 10 ) );
	layout.setColor( Color( 1.0f, 0.7f, 0.0f ) );
	layout.addLine( "click and drag to create particles" );
	layout.setColor( Color( 0.5f, 0.2f, 0.0f ) );
	layout.addLine( "C	toggle constraints (floor plane)" );
	layout.addLine( "Q	render particle quads" );
	layout.addLine( "T	render particle trail" );
	layout.addLine( "P	toggle perlin noise force" );
	layout.addLine( "G	toggle gravitational force" );
	layout.addLine( "F	toggle fullscreen" );
	layout.addLine( "?	toggle information panel" );
	
	mTexture = gl::Texture( layout.render( true ) );
}


void InfoPanel::update( Vec2f aWindowDim, float aCounter )
{
	if( mState ){
		if( aCounter == mFadeTime ){
			toggleState();
		}
		mOpacity -= ( mOpacity - 1.0f ) * 0.1f;
	} else {
		mOpacity -= ( mOpacity - 0.0f ) * 0.1f;	
	}
	
	if( mOpacity > 0.01f ){
		render( aWindowDim );
	}
}


void InfoPanel::render( Vec2f aWindowDim )
{
	float x = aWindowDim.x - mTexture.getWidth() - 40.0f;
	float y = aWindowDim.y - mTexture.getHeight() - 25.0f;
	glColor4f( 1, 1, 1, mOpacity );
	gl::draw( mTexture, Vec2f( x, y ) );
}


void InfoPanel::toggleState()
{
	mState = ! mState;
}

