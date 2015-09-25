#pragma once

#define USE_RENDERER2D

#include "cinder/app/cocoa/AppCocoaView.h"

struct TestCallbackOrder {
	TestCallbackOrder() : mState( TestCallbackOrder::VIRGIN ), mDoneDraw( false ) {}
	~TestCallbackOrder()
	{
		if( mState != CLEANUP )
			cinder::app::console() << "Failed to call cleanup()" << std::endl;
	}
	
	void	setState( int state )
	{
		if( ( ( state == UPDATE ) || ( state == DRAW ) ) && mDoneDraw )
			;
		else if( mState != state - 1 )
			cinder::app::console() << "Fail at state: " << mState << "->" << state << std::endl;
		else
			mState = state;
		if( mState == DRAW )
			mDoneDraw = true;
	}
	
	enum { VIRGIN, SETUP, RESIZE, UPDATE, DRAW, CLEANUP };
	
	bool	mDoneDraw;
	int		mState;
};

class MyCinderApp : public cinder::app::AppCocoaView {
  public:
	static void			prepareSettings( Settings *settings );
	MyCinderApp();
	~MyCinderApp();

	void				setup() override;
	void				resize() override;
	void				update() override;
	void				draw() override;
	void				cleanup() override;
	
	void				mouseUp( ci::app::MouseEvent event ) override;
	void				mouseDown( ci::app::MouseEvent event ) override;
	void				mouseDrag( ci::app::MouseEvent event ) override;
	void				mouseMove( ci::app::MouseEvent event ) override;

	void				touchesMoved( ci::app::TouchEvent event ) override;

	void				keyDown( ci::app::KeyEvent event ) override;
	
	void				fileDrop( ci::app::FileDropEvent event ) override;
	
	float				mRadius, mAnimatedRadius;
	cinder::Colorf		mColor;
	TestCallbackOrder	testCbo;
};
