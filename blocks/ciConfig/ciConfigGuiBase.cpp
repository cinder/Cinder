//
//  ciConfigGuiBase.cpp
//
//  Created by Roger Sodre on 08/04/2010
//  Copyright 2011 Studio Avante. All rights reserved.
//

#include "ciConfigGuiBase.h"

using namespace cinder;
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
	}
	ciConfigGuiBase::~ciConfigGuiBase()
	{
	}


	// Public
	void ciConfigGuiBase::draw()
	{
		if ( !this->guiIsVisible() )
			return;
		
		this->drawGui();
	}

} // cinder::


