#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

const int NUM_POINTS = 4; // can't be changed
const int SUBDIVISIONS = 30; // can be

class TessellationBezierApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void mouseUp( MouseEvent event ) override;
	void draw() override;
	
	int		mSelectedIndex;
	vec2	mVertices[NUM_POINTS];
	
	gl::GlslProgRef		mGlslProg;
	gl::VboMeshRef		mBezierControlMesh;
	gl::BatchRef		mBezierBatch;
};

void TessellationBezierApp::setup()
{
	mSelectedIndex = -1;
	
	mVertices[0] = vec2( 50, 250 );
	mVertices[1] = vec2( 20, 50 );
	mVertices[2] = vec2( 450, 100 );
	mVertices[3] = vec2( 350, 350 );
	
	mGlslProg = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "bezier.vert" ) )
															.tessellationCtrl( loadAsset( "bezier.tesc" ) )
															.tessellationEval( loadAsset( "bezier.tese" ) )
															.fragment( loadAsset( "bezier.frag" ) ) );
	mGlslProg->uniform( "uSubdivisions", SUBDIVISIONS );
															
	// a VertBatch would be fine for a simple mesh like ours but for more vertices we'd want to use a technique like this.
	mBezierControlMesh = gl::VboMesh::create( NUM_POINTS, GL_PATCHES, { gl::VboMesh::Layout().attrib( geom::POSITION, 2 ) } );
	mBezierControlMesh->bufferAttrib( geom::POSITION, sizeof(vec2) * NUM_POINTS, &mVertices[0] );
	
	mBezierBatch = gl::Batch::create( mBezierControlMesh, mGlslProg );
}

void TessellationBezierApp::mouseDown( MouseEvent event )
{
	mSelectedIndex = -1;
	for( size_t i = 0; i < NUM_POINTS; ++i )
		if( glm::distance( vec2( event.getPos() ), mVertices[i] ) < 7.0f )
			mSelectedIndex = i;
}

void TessellationBezierApp::mouseDrag( MouseEvent event )
{
	if( mSelectedIndex > -1 ) {
		mVertices[mSelectedIndex] = event.getPos();
		// update the points of our mesh
		mBezierControlMesh->bufferAttrib( geom::POSITION, sizeof(vec2) * NUM_POINTS, &mVertices[0] );		
	}
}

void TessellationBezierApp::mouseUp( MouseEvent event )
{
	mSelectedIndex = -1;
}

void TessellationBezierApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );

	gl::color( 1, 0.5f, 0.25f );
	gl::patchParameteri( GL_PATCH_VERTICES, 4 );
	mBezierBatch->draw();
	
	for( size_t i = 0; i < NUM_POINTS; ++i ) {
		if( mSelectedIndex == i )
			gl::color( 1, 1, 0 );
		else
			gl::color( 0, 0, 1 );
		gl::drawSolidRect( Rectf( mVertices[i] - vec2( 3 ), mVertices[i] + vec2( 3 ) ) );
	}
}

CINDER_APP( TessellationBezierApp, RendererGl( RendererGl::Options().msaa( 16 ).version( 4, 0 ) ) )
