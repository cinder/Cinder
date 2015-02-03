#pragma once

#define USE_RENDERER2D

#include "cinder/app/AppCocoaView.h"

struct TestCallbackOrder {
	TestCallbackOrder() : mState( TestCallbackOrder::VIRGIN ), mDoneDraw( false ) {}
	~TestCallbackOrder() {
		if( mState != SHUTDOWN )
			cinder::app::console() << "Failed to call shutdown()" << std::endl;
	}
	
	void	setState( int state ) {
		if( ( ( state == UPDATE ) || ( state == DRAW ) ) && mDoneDraw )
			;
		else if( mState != state - 1 )
			cinder::app::console() << "Fail at state: " << mState << "->" << state << std::endl;
		else
			mState = state;
		if( mState == DRAW )
			mDoneDraw = true;
	}
	
	enum { VIRGIN, PREPARESETTINGS, SETUP, RESIZE, UPDATE, DRAW, SHUTDOWN };
	
	bool	mDoneDraw;
	int		mState;
};

class MyCinderApp : public cinder::app::AppCocoaView {
  public:
	void				prepareSettings( Settings *settings );
	void				setup();
	void				resize();
	void				update();
	void				draw();
	void				shutdown();
	
	void				mouseUp( ci::app::MouseEvent event );
	void				mouseDown( ci::app::MouseEvent event );
	void				mouseDrag( ci::app::MouseEvent event );
	void				mouseMove( ci::app::MouseEvent event );

	virtual void		touchesMoved( ci::app::TouchEvent event ) override;

	void				keyDown( ci::app::KeyEvent event );
	
	void				fileDrop( ci::app::FileDropEvent event );
	
	float				mRadius, mAnimatedRadius;
	cinder::Colorf		mColor;
	TestCallbackOrder	testCbo;
};
