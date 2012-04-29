//
//  ciConfigGuiBase.cpp
//
//  Created by Roger Sodre on 08/04/2010
//  Copyright 2010 Studio Avante. All rights reserved.
//

#include "ciConfigGuiBase.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace cinder {
	
	//
	// CUSTOM CONFIG CLASS
	ciConfigGuiBase::ciConfigGuiBase() : ciConfig()
	{
		mGuiOffset = Vec2f(0,0);
		bAutoDraw = false;
		mAutoHideSeconds = 0;
		mGroupCount = 0;
		bUsingFbo = false;
		bShouldResize = false;
		// Register resize
		cbResize = App::get()->registerResize( this, &ciConfigGuiBase::onResize );
	}
	ciConfigGuiBase::~ciConfigGuiBase()
	{
	}

	bool ciConfigGuiBase::onResize( app::ResizeEvent event )
	{
		bShouldResize = true;
		return false;
	}
	
	void ciConfigGuiBase::guiSetUseFbo( bool _use )
	{
		bUsingFbo = _use;
		bShouldResize = true;
	}

	
	// Public
	void ciConfigGuiBase::draw()
	{
		if ( !this->guiIsVisible() )
			return;
		
		if (bUsingFbo)
		{
			// Update Fbo
			bool updated = false;
			if ( this->guiHasChanged() || bShouldResize )
			{
				if ( !mFbo || bShouldResize )
				{
					mFbo = gl::Fbo( app::getWindowWidth(), app::getWindowHeight(), true, true, false );
					mFbo.getTexture().setFlipped();
					bShouldResize = false;
				}
				mFbo.bindFramebuffer();
				gl::setMatricesWindow( mFbo.getSize() );
				gl::setViewport( mFbo.getBounds() );
				//gl::clear( ColorA(1,0,0,0.5) );	// debug size with color
				gl::clear( ColorA(0,0,0,0) );
				this->drawGui();
				mFbo.unbindFramebuffer();
				updated = true;
			}
			// draw over
			Rectf bounds = Rectf( Vec2f(0,0), this->guiGetSize() );
			Area srcFlipped = Area( bounds.x1, app::getWindowHeight()-bounds.y1, bounds.x2, app::getWindowHeight()-bounds.y2 );
			gl::setMatricesWindow( getWindowSize() );
			gl::setViewport( getWindowBounds() );
			gl::enableAlphaBlending();
#ifdef RELEASE
			gl::color( ColorA(1,1,1,1) );
#else
			gl::color( ColorA(1,1,1,(updated?0.85:1)) );
#endif
			//gl::draw( mFbo.getTexture() );
			gl::draw( mFbo.getTexture(), srcFlipped, bounds );
			gl::disableAlphaBlending();
		}
		else
			this->drawGui();
	}

} // cinder::


