#pragma once

#include "cinder/App/App.h"
#include "cinder/Vector.h"

class SquareListener {
 public:
	SquareListener( ci::app::App *app );
	~SquareListener();
 
	void	mouseDown( ci::app::MouseEvent event );
	void	mouseDrag( ci::app::MouseEvent event );
	
	void	draw();
	
 private:
	ci::Rectf		mRect;
	bool			mSelected;
	
	ci::app::App	*mApp;
	ci::CallbackId	mCbMouseDown, mCbMouseDrag;
};