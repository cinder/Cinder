#include "cinder/app/AppBasic.h"
#include "cinder/Path2d.h"
#include "cinder/cairo/Cairo.h"
#include "cinder/ip/Fill.h"

#include <vector>

using namespace ci;
using namespace ci::app;
using namespace std;

class Path2dApp : public AppBasic {
 public:
	Path2dApp() : mTrackedPoint( -1 ) {}
	
	void mouseDown( MouseEvent event );
	void mouseUp( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void keyDown( KeyEvent event );
	void draw();
	
	Path2d	mPath;
	int		mTrackedPoint;
};

void Path2dApp::mouseDown( MouseEvent event )
{		
	if( event.isLeftDown() ) { // line
		if( mPath.getNumSegments() == 0 ) {
			mPath.moveTo( event.getPos() );
			mTrackedPoint = 0;
		}
		else
			mPath.lineTo( event.getPos() );
	}
}

void Path2dApp::mouseDrag( MouseEvent event )
{
	if( mTrackedPoint >= 0 ) {
		mPath.setPoint( mTrackedPoint, event.getPos() );
	}
	else { // first bit of dragging, so switch our line to a cubic or a quad if Shift is down
		// we want to preserve the end of our current line, because it will still be the end of our curve
		Vec2f endPt = mPath.getPoint( mPath.getNumPoints() - 1 );
		// and now we'll delete that line and replace it with a curve
		mPath.removeSegment( mPath.getNumSegments() - 1 );
		
		Path2d::SegmentType prevType = mPath.getSegmentType( mPath.getNumSegments() - 1 );		
		
		if( event.isShiftDown() ) { // add a quadratic curve segment
			mPath.quadTo( event.getPos(), endPt );
		}
		else { // add a cubic curve segment
			Vec2f tan1;
			if( prevType == Path2d::CUBICTO ) { 		// if the segment before was cubic, let's replicate and reverse its tangent
				Vec2f prevDelta = mPath.getPoint( mPath.getNumPoints() - 2 ) - mPath.getPoint( mPath.getNumPoints() - 1 );
				tan1 = mPath.getPoint( mPath.getNumPoints() - 1 ) - prevDelta;
			}
			else if( prevType == Path2d::QUADTO ) {
				// we can figure out what the equivalent cubic tangent would be using a little math
				Vec2f quadTangent = mPath.getPoint( mPath.getNumPoints() - 2 );
				Vec2f quadEnd = mPath.getPoint( mPath.getNumPoints() - 1 );
				Vec2f prevDelta = ( quadTangent + ( quadEnd - quadTangent ) * 0.3333333f ) - quadEnd;
				tan1 = quadEnd - prevDelta;
			}
			else
				tan1 = mPath.getPoint( mPath.getNumPoints() - 1 );
			
			mPath.curveTo( tan1, event.getPos(), endPt );
		}
		
		// our second-to-last point is the tangent next to the end, and we'll track that
		mTrackedPoint = mPath.getNumPoints() - 2;
	}
}

void Path2dApp::mouseUp( MouseEvent event )
{
	mTrackedPoint = -1;
}

void Path2dApp::keyDown( KeyEvent event ) {
	if( event.getCode() == KeyEvent::KEY_f ) {
		setFullScreen( ! isFullScreen() );
	}
	else if( event.getCode() == KeyEvent::KEY_ESCAPE ) {
		setFullScreen( false );
	}
	else if( event.getChar() == 'x' ) {
		mPath.clear();
	}
}

void Path2dApp::draw()
{
	// clear to the background color
	cairo::Context ctx( cairo::createWindowSurface() );
	ctx.setSourceRgb( 0.0f, 0.1f, 0.2f );
	ctx.paint();
	
	// draw the control points
	ctx.setSourceRgb( 1.0f, 1.0f, 0.0f );
	for( size_t p = 0; p < mPath.getNumPoints(); ++p ) {
		ctx.newSubPath();
		ctx.arc( mPath.getPoint( p ), 2.5, 0, 2 * 3.14159 );
	}
	ctx.stroke();

	// draw the curve itself
	ctx.setSourceRgb( 1.0f, 0.5f, 0.25f );
	ctx.appendPath( mPath );
	ctx.stroke();
}


CINDER_APP_BASIC( Path2dApp, Renderer2d )
