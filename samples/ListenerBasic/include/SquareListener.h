#pragma once

#include "cinder/App/App.h"
#include "cinder/Vector.h"

class SquareListener : public ci::app::App::Listener {
 public:
	SquareListener( ci::app::App *app );
 
	virtual bool	mouseDown( ci::app::MouseEvent event );
	virtual bool	mouseDrag( ci::app::MouseEvent event );
	
	void	draw();
	
 private:
	ci::Rectf		mRect;
	bool			mSelected;
};