//
//	TrianglePiece.h
//	Instascope
//
//	Created by Greg Kepler on 5/31/12.
//	Copyright (c) 2012 The Barbarian Group. All rights reserved.
//

#pragma once

#include "cinder/gl/Texture.h"
#include "cinder/Timeline.h"

class TrianglePiece {
  public:
	TrianglePiece( ci::Vec2f _startPt, ci::Vec2f _pt1, ci::Vec2f _pt2, ci::Vec2f _pt3, float _rotation, ci::Vec2f _scale );

	void		update( ci::gl::TextureRef tex, ci::Vec2f pt1, ci::Vec2f pt2, ci::Vec2f pt3 );
	void		draw();
	void		reset( float _delay, ci::gl::TextureRef tempTex );
	void		setTransition( float _delay );
	void		setTransitionOut( float _delay );
	bool		isOut() const;
	bool		isIn() const;
	
  protected:
	void		setVisible( bool vis );
	void		outComplete();
	
	ci::gl::TextureRef	mTempTex, mDrawTex;
	
	ci::Vec2f		mStartPt, mVertices[3], mTexVertices[3];
	
	float			mRotation;
	ci::Anim<float> mAlpha;
	ci::Vec2f		mScale;
	
	bool			mVisible, mReadyToDraw;
	bool			mTransOut;
};