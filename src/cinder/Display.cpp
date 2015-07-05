/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#include "cinder/Display.h"
#include <map>
using namespace std;

#include "cinder/app/Platform.h"

#if defined( CINDER_COCOA_TOUCH )
	#include "cinder/app/cocoa/PlatformCocoa.h"
	#include <UIKit/UIKit.h>
#endif

namespace cinder {

const vector<DisplayRef>& Display::getDisplays()
{
	return app::Platform::get()->getDisplays();
}

DisplayRef Display::getDisplayForPoint( const ivec2 &pt )
{
	const vector<DisplayRef>& displays = getDisplays();
	for( vector<DisplayRef>::const_iterator displayIt = displays.begin(); displayIt != displays.end(); ++displayIt ) {
		if( (*displayIt)->contains( pt ) )
			return *displayIt;
	}

	return DisplayRef(); // failure
}

Area Display::getSpanningArea()
{
	Area result = (*Display::getDisplays().begin())->getBounds();
	for( vector<DisplayRef>::const_iterator displayIt = (Display::getDisplays().begin())++; displayIt != Display::getDisplays().end(); ++displayIt ) {
		result.include( (*displayIt)->getBounds() );
	}
	
	return result;
}

Area Display::getBounds() const
{
#if defined( CINDER_COCOA_TOUCH )
	// WORKAROUND for iOS 8 - mArea was cached and could be flipped if we're in landscape, so instead use UIScreen's bounds
	CGRect frame = [reinterpret_cast<const cinder::DisplayCocoaTouch*>( this )->getUiScreen() bounds];
	return Area( frame.origin.x, frame.origin.y, frame.origin.x + frame.size.width, frame.origin.y + frame.size.height );
#else
	return mArea;
#endif
}

ivec2 Display::getSystemCoordinate( const ivec2 &displayRelativeCoordinate ) const
{
	return mArea.getUL() + displayRelativeCoordinate;
}

DisplayRef Display::getMainDisplay()
{
	auto displays = app::Platform::get()->getDisplays();
	if( ! displays.empty() )
		return displays[0];
	else
		return nullptr;
}

} // namespace cinder
