
#include "InfoPanel.h"
#include "cinder/Text.h"
#include "cinder/gl/Texture.h"

using namespace ci;

InfoPanel::InfoPanel()
{
	mState		= true;
	mOpacity	= 1.0f;
}


void InfoPanel::createTexture()
{
	TextLayout layout;
	
	layout.setFont( Font( "Arial-BoldMT", 14 ) );
	layout.setColor( Color( 1.0f, 1.0f, 0.0f ) );
	layout.addLine( "OpenGL Lighting" );
	
	layout.setFont( Font( "ArialMT", 10 ) );
	layout.setColor( Color( 1.0f, 0.7f, 0.0f ) );
	layout.addLine( "mouse move	reposition light source" );
	layout.addLine( "click and hold	switch to directional light" );
	layout.setColor( Color( 0.5f, 0.2f, 0.0f ) );
	layout.addLine( "A	toggle ambient lighting" );
	layout.addLine( "D	toggle diffuse lighting" );
	layout.addLine( "S	toggle specular lighting" );
	layout.addLine( "E	toggle emissive lighting" );
	layout.addLine( "?	toggle information panel" );
	layout.addLine( "< >	adjust shininess" );
	
	mTexture = gl::Texture( layout.render( true ) );
}


void InfoPanel::update( Vec2f aWindowDim, float aCounter )
{
	if( mState ){
		if( aCounter == 250.0f ){
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
	float x = aWindowDim.x - mTexture.getWidth() - 20.0f;
	float y = aWindowDim.y - mTexture.getHeight() - 40.0f;
	glColor4f( 1, 1, 1, mOpacity );
	gl::draw( mTexture, Vec2f( x, y ) );
}


void InfoPanel::toggleState()
{
	mState = ! mState;
}

