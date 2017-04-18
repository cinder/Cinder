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
	Path2d	mSubPath1, mSubPath2;
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
	mSubPath1 = mPath.getSubPath( 0, 0.3f );
	mSubPath2 = mPath.getSubPath( 0.3f, 1.0f );
	mSubPath1.transform( glm::translate( mat3(), vec2( 0 ) ) );
	mSubPath2.transform( glm::translate( mat3(), vec2( 0 ) ) );
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
	mSubPath1 = mPath.getSubPath( 0, 0.3f );
	console() << mSubPath1 << std::endl;
	mSubPath2 = mPath.getSubPath( 0.3f, 1.0f );
	mSubPath1.transform( glm::translate( mat3(), vec2( 0 ) ) );
	mSubPath2.transform( glm::translate( mat3(), vec2( 0 ) ) );
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

Path2d calcQuadSub( const Path2d &p, float t0, float t1 )
{
	Path2d result;
	result.moveTo( p.getPosition( t0 ) );
	vec2 i0 = (1 - t0) * p.getPoint( 1 ) + t0 * p.getPoint( 2 );
	float u1 = ( t1 - t0 ) / ( 1 - t0 );
	vec2 i1 = (1 - u1) * p.getPosition( t0 ) + u1 * i0;
	result.quadTo( i1, p.getPosition( t1 ) );
	return result;
}

void Path2dApp::draw()
{
	gl::clear( Color( 0.0f, 0.1f, 0.2f ) );
	
Path2d p;
p.moveTo( 100, 200 + getElapsedSeconds() + 10 );
p.quadTo( 180, 20, 300, 300 );
gl::draw( p );
gl::color( Color( 1, 1, 0 ) );
float t0 = 0.1f, t1 = 0.3f + fmod( getElapsedSeconds(), 0.7f );
gl::drawSolidCircle( p.getPosition( t0 ), 2.5f );
gl::drawSolidCircle( p.getPosition( t1 ), 2.5f );

gl::color( Color( 0, 1, 0 ) );
gl::draw( calcQuadSub( p, t0, t1 ) );

	// draw the control points
	gl::color( Color( 1, 1, 0 ) );
	for( size_t p = 0; p < mPath.getNumPoints(); ++p )
		gl::drawSolidCircle( mPath.getPoint( p ), 2.5f );

	// draw the curve itself
	gl::color( Color( 1.0f, 0.5f, 0.25f ) );
	gl::draw( mPath );
	
	if( mPath.getNumSegments() >= 1 ) {
		gl::lineWidth( 4.0f );
		gl::color( 0.9f, 0.2f, 0.3f );
		gl::draw( mSubPath1 );
		gl::color( 0.1f, 0.9f, 0.3f );
		gl::draw( mSubPath2 );
	}
}


CINDER_APP( Path2dApp, RendererGl( RendererGl::Options().msaa( 16 ) ) )
