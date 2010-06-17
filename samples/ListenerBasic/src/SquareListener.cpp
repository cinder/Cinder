#include "SquareListener.h"
#include "cinder/gl/gl.h"
#include "cinder/Rect.h"

using namespace ci;
using namespace ci::app;

SquareListener::SquareListener( App *app )
	: mApp( app )
{
	mRect = Rectf( Vec2f( -40, -40 ), Vec2f( 40, 40 ) );
	mRect.offset( app->getWindowCenter() );
	mSelected = false;
	
	mCbMouseDown = mApp->registerMouseDown( this, &SquareListener::mouseDown );
	mCbMouseDrag = mApp->registerMouseDrag( this, &SquareListener::mouseDrag );	
}

SquareListener::~SquareListener()
{
	mApp->unregisterMouseDown( mCbMouseDown );
	mApp->unregisterMouseDrag( mCbMouseDrag );	
}
 
bool SquareListener::mouseDown( MouseEvent event )
{
	mSelected = mRect.contains( event.getPos() );

	// if we got selected then we handled the event, otherwise let it pass through
	return mSelected;
}

bool SquareListener::mouseDrag( MouseEvent event )
{
	if( mSelected )
		mRect.offsetCenterTo( event.getPos() );

	// if we are selected then we handled the event, otherwise let it pass through
	return mSelected;
}

void SquareListener::draw()
{
	gl::color( ( mSelected ) ? Color::white() : Color::black() );
	gl::drawSolidRect( mRect );
}
