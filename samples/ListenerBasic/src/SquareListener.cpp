#include "SquareListener.h"
#include "cinder/gl/gl.h"
#include "cinder/Rect.h"

using namespace ci;
using namespace ci::app;

SquareListener::SquareListener( app::WindowRef window )
	: mWindow( window )
{
	mRect = Rectf( Vec2f( -40, -40 ), Vec2f( 40, 40 ) );
	mRect.offset( window->getCenter() );
	mSelected = false;
	
	mCbMouseDown = mWindow->getSignalMouseDown().connect( std::bind( &SquareListener::mouseDown, this, std::placeholders::_1 ) );
	mCbMouseDrag = mWindow->getSignalMouseDrag().connect( std::bind( &SquareListener::mouseDrag, this, std::placeholders::_1 ) );	
}
 
void SquareListener::mouseDown( MouseEvent &event )
{
	mSelected = mRect.contains( event.getPos() );

	// if we got selected then we handled the event, otherwise let it pass through
	event.setHandled( mSelected );
}

void SquareListener::mouseDrag( MouseEvent &event )
{
	if( mSelected )
		mRect.offsetCenterTo( event.getPos() );

	// if we are selected then we handled the event, otherwise let it pass through
	event.setHandled( mSelected );
}

void SquareListener::draw()
{
	gl::color( ( mSelected ) ? Color::white() : Color::black() );
	gl::drawSolidRect( mRect );
}
