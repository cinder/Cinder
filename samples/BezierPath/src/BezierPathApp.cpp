#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Path2d.h"
#include "cinder/gl/gl.h"

#include <vector>

using namespace ci;
using namespace ci::app;
using namespace std;

class Path2dApp : public App {
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
		if( mPath.empty() ) {
			mPath.moveTo( event.getPos() );
			mTrackedPoint = 0;
		}
		else
			mPath.lineTo( event.getPos() );
	}

	console() << mPath << std::endl;	
	console() << "Length: " << mPath.calcLength() << std::endl;
}

void Path2dApp::mouseDrag( MouseEvent event )
{
	if( mTrackedPoint >= 0 ) {
		mPath.setPoint( mTrackedPoint, event.getPos() );
	}
	else { // first bit of dragging, so switch our line to a cubic or a quad if Shift is down
		// we want to preserve the end of our current line, because it will still be the end of our curve
		vec2 endPt = mPath.getPoint( mPath.getNumPoints() - 1 );
		// and now we'll delete that line and replace it with a curve
		mPath.removeSegment( mPath.getNumSegments() - 1 );
		
		Path2d::SegmentType prevType = ( mPath.getNumSegments() == 0 ) ? Path2d::MOVETO : mPath.getSegmentType( mPath.getNumSegments() - 1 );		
		
		if( event.isShiftDown() || prevType == Path2d::MOVETO ) { // add a quadratic curve segment
			mPath.quadTo( event.getPos(), endPt );
		}
		else { // add a cubic curve segment
			vec2 tan1;
			if( prevType == Path2d::CUBICTO ) { 		// if the segment before was cubic, let's replicate and reverse its tangent
				vec2 prevDelta = mPath.getPoint( mPath.getNumPoints() - 2 ) - mPath.getPoint( mPath.getNumPoints() - 1 );
				tan1 = mPath.getPoint( mPath.getNumPoints() - 1 ) - prevDelta;
			}
			else if( prevType == Path2d::QUADTO ) {
				// we can figure out what the equivalent cubic tangent would be using a little math
				vec2 quadTangent = mPath.getPoint( mPath.getNumPoints() - 2 );
				vec2 quadEnd = mPath.getPoint( mPath.getNumPoints() - 1 );
				vec2 prevDelta = ( quadTangent + ( quadEnd - quadTangent ) / 3.0f ) - quadEnd;
				tan1 = quadEnd - prevDelta;
			}
			else
				tan1 = mPath.getPoint( mPath.getNumPoints() - 1 );
			
			mPath.curveTo( tan1, event.getPos(), endPt );
		}
		
		// our second-to-last point is the tangent next to the end, and we'll track that
		mTrackedPoint = mPath.getNumPoints() - 2;
	}
	
	console() << mPath << std::endl;
	console() << "Length: " << mPath.calcLength() << std::endl;	
}

void Path2dApp::mouseUp( MouseEvent event )
{
	mTrackedPoint = -1;
}

void Path2dApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'x' )
		mPath.clear();
}

void Path2dApp::draw()
{
	gl::clear( Color( 0.0f, 0.1f, 0.2f ) );
	
	// draw the control points
	gl::color( Color( 1, 1, 0 ) );
	for( size_t p = 0; p < mPath.getNumPoints(); ++p )
		gl::drawSolidCircle( mPath.getPoint( p ), 2.5f );

	// draw the precise bounding box
	if( mPath.getNumSegments() > 1 ) {
		gl::color( ColorA( 0, 1, 1, 0.2f ) );
		gl::drawSolidRect( mPath.calcPreciseBoundingBox() );
	}

	// draw the curve itself
	gl::color( Color( 1.0f, 0.5f, 0.25f ) );
	gl::draw( mPath );
	
	if( mPath.getNumSegments() > 1 ) {	
		// draw some tangents
		gl::color( Color( 0.2f, 0.9f, 0.2f ) );	
		for( float t = 0; t < 1; t += 0.2f )
			gl::drawLine( mPath.getPosition( t ), mPath.getPosition( t ) + normalize( mPath.getTangent( t ) ) * 80.0f );
		
		// draw circles at 1/4, 2/4 and 3/4 the length
		gl::color( ColorA( 0.2f, 0.9f, 0.9f, 0.5f ) );
		for( float t = 0.25f; t < 1.0f; t += 0.25f )
			gl::drawSolidCircle( mPath.getPosition( mPath.calcNormalizedTime( t ) ), 5.0f );
	}
}


CINDER_APP( Path2dApp, RendererGl )
