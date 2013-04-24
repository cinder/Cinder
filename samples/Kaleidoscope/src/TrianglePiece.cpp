//
//	TrianglePiece.cpp
//	Instascope
//
//	Created by Greg Kepler on 5/30/12.
//	Copyright (c) 2012 The Barbarian Group. All rights reserved.
//

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rand.h"
#include "cinder/Timeline.h"
#include "TrianglePiece.h"

using namespace ci;
using namespace ci::app;
using namespace std;


TrianglePiece::TrianglePiece( Vec2f _startPt, Vec2f _pt1, Vec2f _pt2, Vec2f _pt3, float _rotation, Vec2f _scale )
	: mAlpha(0.0f), mTransOut(false)
{
	mStartPt = _startPt;
	mVertices[0] = _pt1;
	mVertices[1] = _pt2;
	mVertices[2] = _pt3;
	mRotation = _rotation;
	mScale = _scale;
	
	mReadyToDraw = false;
	mVisible = false;
	mTransOut = false;
}

void TrianglePiece::reset( float _delay, gl::TextureRef tempTex )
{
	mTempTex = tempTex;	
	setTransition(_delay);
}

void TrianglePiece::setTransition( float _delay )
{
	if( mVisible ){
		// transition out
		mTransOut = true;
		mDrawTex = mTempTex;
		app::timeline().apply( &mAlpha, 0.0f, randFloat(0.2f, 0.4f), EaseInQuint()).delay(_delay);
	}
	else{
		// transition in
		app::timeline().apply( &mAlpha, 1.0f, randFloat(0.2f, 0.7f), EaseInQuint()).delay(0.5f + _delay).
					startFn( [&] { setVisible( true ); } );
	}
}

void TrianglePiece::setTransitionOut( float _delay )
{
	setTransition( _delay );
}

void TrianglePiece::setVisible( bool vis )
{
	mVisible = vis;
}

void TrianglePiece::update( gl::TextureRef tex, Vec2f pt1, Vec2f pt2, Vec2f pt3 )
{	 
	if( ! mTransOut ) {
		mTexVertices[0] = pt1;
		mTexVertices[1] = pt2;
		mTexVertices[2] = pt3;
		mDrawTex = tex;
	}
	mReadyToDraw = true;
}

void TrianglePiece::draw()
{
	if( ! mReadyToDraw ) return;
	if( mAlpha == 0.0f ) return;
	
	glPushMatrix();
	gl::translate( mStartPt );				// move to the start point
	gl::scale( mScale );					// scale the triangle
	gl::rotate( Vec3f( 0, 0, mRotation ) );	// rotate on the Z axis
	
	glColor4f(1.0, 1.0, 1.0, mAlpha);
	
	// draw the texture to the triangle
	mDrawTex->enableAndBind();
	gl::drawSolidTriangle(mVertices, mTexVertices);
	mDrawTex->unbind();
	glColor4f(1.0, 1.0, 1.0, 1.0);			// reset the color/alpha
	glPopMatrix();
}

bool TrianglePiece::isOut() const
{
    // had to round here because every once in a while, it's something like .00001
    bool out = mTransOut && mAlpha <= 0.1;
    return out;
}

bool TrianglePiece::isIn() const
{
    return ( ! mTransOut && mAlpha >= 0.9 );
}