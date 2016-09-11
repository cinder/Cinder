#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"
#include "cinder/ip/Checkerboard.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ExtrudeSplineApp : public App {
  public:
	ExtrudeSplineApp();
	void draw() override;
	
	gl::BatchRef		mBatch[5];
	gl::BatchRef		mBatchNormals[5];
	gl::BatchRef		mBatchLines[5];
	gl::Texture2dRef	mTexture;

	CameraPersp			mCamera;
	CameraUi			mCameraUi;
};

ExtrudeSplineApp::ExtrudeSplineApp()
: mCamera( CameraPersp( getWindowWidth(), getWindowHeight(), 50.0f, 0.15f, 1000.0f ).calcFraming( Sphere( vec3( 0.0f, 0.0f, 0.0f ), 2.0f ) ) ),
mCameraUi( &mCamera, getWindow(), -1 ),
mTexture( gl::Texture2d::create( ip::checkerboard( 512, 512 ), gl::Texture2d::Format().minFilter( GL_NEAREST ).magFilter( GL_NEAREST ).wrap( GL_REPEAT ) ) )
{
	// circle helper function
	const auto createCircle = []( float radius, int segments ) {
		Shape2d shape;
		shape.moveTo( vec2( 1, 0 ) * radius );
		float inc = ( 2.0f * M_PI ) / (float) segments;
		for( float angle = 0.0f; angle <= M_PI * 2.0f; angle += inc ) {
			shape.lineTo( vec2( cos( angle ), sin( angle ) ) * radius );
		}
		shape.lineTo( vec2( 1, 0 ) * radius );
		return shape;
	};
	// spline helper function
	const auto createSpline = []( const vector<vec3> splinePoints ) {
		return BSpline3f( splinePoints, splinePoints.size() - 1, false, true );
	};
	
	// create the test geom::Sources
	TriMesh source[5] = { 
		geom::ExtrudeSpline( createCircle( 0.125f, 3 ),  // normals issues
							createSpline( { vec3( 0, 0.0f, 0 ), vec3( 0, 0.25f, 0 ), vec3( 0, 0.5f, 0 ), vec3( 0, 0.75f, 0 ), vec3( 0, 1.0f, 0 ) } ), 
							30 )
							 >> geom::Translate( -2.0f, 0.0f, 0.0f ),

		geom::ExtrudeSpline( createCircle( 0.125f, 5 ),  // normals issues 
							createSpline( { vec3( 0, 0.0f, 0 ), vec3( 0, 0.25f, 0 ), vec3( 0, 0.5f, 0 ), vec3( 0, 0.75f, 0 ), vec3( 0, 1.0f, 0 ) } ), 
							30 )
							 >> geom::Translate( -1.0f, 0.0f, 0.0f ),

		geom::ExtrudeSpline( createCircle( 0.125f, 5 ),   // normals reconstructed with TriMesh::recalculateNormals
							createSpline( { vec3( 0, 0.0f, 0 ), vec3( 0, 0.25f, 0 ), vec3( 0, 0.5f, 0 ), vec3( 0, 0.75f, 0 ), vec3( 0, 1.0f, 0 ) } ), 
							30 )
							 >> geom::Translate( 0.0f, 0.0f, 0.0f ),

		geom::ExtrudeSpline( createCircle( 0.125f, 9 ), // last segment normals / black faces
							 createSpline( { vec3( 0, 0.0f, 0 ), vec3( 0.1, 0.5, 0 ), vec3( 0.2, 0.75, 0 ), vec3( 0.4, 1, 0 ), vec3( 0.6, 0.75, 0 ) } ), 
							 30 ).thickness( []( float t ) { return smoothstep( 1.0f, 0.15f, t ); } ) 
							 >> geom::Translate( 1.0f, 0.0f, 0.0f ),
		
		geom::ExtrudeSpline( createCircle( 0.025f, 15 ),  // last segment normals / black faces 
							 createSpline( { vec3( 0, 0.0f, 0 ), vec3( 0.0, 0.0, 0 ), vec3( -0.2, 0.75, 0 ), vec3( -0.4, 1, 0 ) } ), 
							 30 ).thickness( []( float t ) { return smoothstep( 1.0f, 0.15f, t ) * smoothstep( 0.0f, 0.15f, t ); } ) 
							 >> geom::Translate( 2.0f, 0.0f, 0.0f )
	};
	
	// recalculate the normals only for the third ExtrudeSpline
	source[2].recalculateNormals( true, true );

	// create 3 batches for each source, a shaded with texture, a wireframe and a normal lines.
	using ColorAttribFn = function<ci::Colorf(vec3)>;
	const auto glslProg0 = gl::getStockShader( gl::ShaderDef().lambert().texture() );
	const auto glslProg1 = gl::getStockShader( gl::ShaderDef().color() );
	for( size_t i = 0; i < 5; ++i ) {
		mBatch[i] = gl::Batch::create( source[i] >> geom::ColorFromAttrib( geom::Attrib::TEX_COORD_0, (ColorAttribFn) []( vec3 uv ) { return Colorf( uv.x, uv.y, uv.z ); } ), glslProg0 );
		mBatchLines[i] = gl::Batch::create( source[i] >> geom::Lines(), glslProg1 );
		mBatchNormals[i] = gl::Batch::create( source[i] >> geom::VertexNormalLines( 0.01f ), glslProg1 );
	}
}

void ExtrudeSplineApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	
	gl::ScopedDepth scopedDepth( true );
	gl::ScopedTextureBind scopedTexBind( mTexture );
	gl::setMatrices( mCamera );

	for( size_t i = 0; i < 5; ++i ) {
		mBatch[i]->draw();
		mBatchLines[i]->draw();
		gl::ScopedColor color( 0, 1, 0 );
		mBatchNormals[i]->draw();
	}
}

CINDER_APP( ExtrudeSplineApp, RendererGl( RendererGl::Options().msaa( 8 ) ) )
