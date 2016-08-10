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

	// Adjust scale so it doesn't look so small on mobile.
	vec2 scale = vec2( getWindowWidth()/640.0f, getWindowHeight()/480.0f );
	mVertices[0] = vec2( 50, 250 )*scale;
	mVertices[1] = vec2( 20, 50 )*scale;
	mVertices[2] = vec2( 450, 100 )*scale;
	mVertices[3] = vec2( 350, 350 )*scale;


#if ! defined( CINDER_GL_ES )
	fs::path glDir = "ogl";
#else
	fs::path glDir = "es31a";
#endif

	mGlslProg = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( glDir / "bezier.vert" ) )
															.tessellationCtrl( loadAsset( glDir / "bezier.tesc" ) )
															.tessellationEval( loadAsset( glDir / "bezier.tese" ) )
															.fragment( loadAsset( glDir / "bezier.frag" ) ) );
	mGlslProg->uniform( "uSubdivisions", SUBDIVISIONS );
															
	// a VertBatch would be fine for a simple mesh like ours but for more vertices we'd want to use a technique like this.
	mBezierControlMesh = gl::VboMesh::create( NUM_POINTS, GL_PATCHES, { gl::VboMesh::Layout().attrib( geom::POSITION, 2 ) } );
	mBezierControlMesh->bufferAttrib( geom::POSITION, sizeof(vec2) * NUM_POINTS, &mVertices[0] );
	
	mBezierBatch = gl::Batch::create( mBezierControlMesh, mGlslProg );
}

void TessellationBezierApp::mouseDown( MouseEvent event )
{
	// Give mobile a bit more breathing room.
#if defined( CINDER_GL_ES )
	float minDist = 50.0f;
#else
	float minDist = 7.0f;
#endif

	mSelectedIndex = -1;
	for( size_t i = 0; i < NUM_POINTS; ++i )
		if( glm::distance( vec2( event.getPos() ), mVertices[i] ) < minDist )
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

#if defined( CINDER_GL_ES )
	float size = 10.0f;

	// Draw an indicator when a point is selected.
	if( -1 != mSelectedIndex ) {
		gl::color( 1, 1, 0 );
		gl::drawSolidRect( Rectf( 0, 0, getWindowWidth(), 10 ) );
	}

	gl::lineWidth( 5.0f );
#else
	float size = 3.0f;
#endif


	gl::color( 1, 0.5f, 0.25f );
	gl::patchParameteri( GL_PATCH_VERTICES, 4 );
	mBezierBatch->draw();
	
	for( size_t i = 0; i < NUM_POINTS; ++i ) {
		if( mSelectedIndex == i )
			gl::color( 1, 1, 0 );
		else
			gl::color( 0, 0, 1 );
		gl::drawSolidRect( Rectf( mVertices[i] - vec2( 3 ), mVertices[i] + vec2( size ) ) );
	}
}

#if defined( CINDER_GL_ES )
	CINDER_APP( TessellationBezierApp, RendererGl(), []( TessellationBezierApp::Settings* settings ) {
		settings->setMultiTouchEnabled( false );
	})
#else
	CINDER_APP( TessellationBezierApp, RendererGl( RendererGl::Options().msaa( 16 ).version( 4, 0 ) ) )
#endif

