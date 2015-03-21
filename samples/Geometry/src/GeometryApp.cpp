#include "cinder/Camera.h"
#include "cinder/GeomIo.h"
#include "cinder/ImageIo.h"
#include "cinder/MayaCamUI.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/params/Params.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

void prepareSettings( App::Settings* settings );

class GeometryApp : public App {
  public:
	enum Primitive { CAPSULE, CONE, CUBE, CYLINDER, HELIX, ICOSAHEDRON, ICOSPHERE, SPHERE, TEAPOT, TORUS, PLANE };
	enum Quality { LOW, DEFAULT, HIGH };
	enum ViewMode { SHADED, WIREFRAME };
	enum TexturingMode { NONE, PROCEDURAL, SAMPLER };

	void setup() override;
	void resize() override;
	void update() override;
	void draw() override;

	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void keyDown( KeyEvent event ) override;

  private:
	void createGrid();
	void createPhongShader();
	void createWireframeShader();
	void createGeometry();
	void loadGeomSource( const geom::Source &source );
	void createParams();

	Primitive			mPrimitiveSelected;
	Primitive			mPrimitiveCurrent;
	Quality				mQualitySelected;
	Quality				mQualityCurrent;
	ViewMode			mViewMode;

	int					mSubdivision;
	int					mTexturingMode;

	bool				mShowColors;
	bool				mShowNormals, mShowTangents;
	bool				mShowGrid;
	bool				mEnableFaceFulling;

	CameraPersp			mCamera;
	MayaCamUI			mMayaCam;
	bool				mRecenterCamera;
	vec3				mCameraCOI;
	double				mLastMouseDownTime;

	gl::VertBatchRef	mGrid;

	gl::BatchRef		mPrimitive;
	gl::BatchRef		mPrimitiveWireframe;
	gl::BatchRef		mPrimitiveNormalLines, mPrimitiveTangentLines;

	gl::GlslProgRef		mPhongShader;
	gl::GlslProgRef		mWireframeShader;

	gl::TextureRef		mTexture;
	
#if ! defined( CINDER_GL_ES )
	params::InterfaceGlRef	mParams;
#endif
};

void prepareSettings( App::Settings* settings )
{
	settings->setWindowSize(1024, 768);
	settings->setHighDensityDisplayEnabled();
	settings->setMultiTouchEnabled( false );
}

void GeometryApp::setup()
{
	// Initialize variables.
	mPrimitiveSelected = mPrimitiveCurrent = TORUS;
	mQualitySelected = mQualityCurrent = HIGH;
	mTexturingMode = PROCEDURAL;
	mViewMode = SHADED;
	mLastMouseDownTime = 0;
	mShowColors = false;
	mShowNormals = false;
	mShowTangents = false;
	mShowGrid = true;
	mEnableFaceFulling = false;

	mSubdivision = 1;
	
	// Load the textures.
	gl::Texture::Format fmt;
	fmt.setAutoInternalFormat();
	fmt.setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
	mTexture = gl::Texture::create( loadImage( loadAsset("stripes.jpg") ), fmt );

	// Setup the camera.
	mCamera.setEyePoint( normalize( vec3( 3, 3, 6 ) ) * 5.0f );
	mCamera.setCenterOfInterestPoint( mCameraCOI );

	// Load and compile the shaders.
	createPhongShader();
	createWireframeShader();

	// Create the meshes.
	createGrid();
	createGeometry();

	// Enable the depth buffer.
	gl::enableDepthRead();
	gl::enableDepthWrite();

	// Create a parameter window, so we can toggle stuff.
	createParams();
}

void GeometryApp::update()
{
	// If another primitive or quality was selected, reset the subdivision and recreate the primitive.
	if( mPrimitiveCurrent != mPrimitiveSelected || mQualitySelected != mQualityCurrent ) {
		mSubdivision = 1;
		mPrimitiveCurrent = mPrimitiveSelected;
		mQualityCurrent = mQualitySelected;
		createGeometry();
	}

	// After creating a new primitive, gradually move the camera to get a good view.
	if( mRecenterCamera ) {
		float distance = glm::distance( mCamera.getEyePoint(), mCameraCOI );
		mCamera.setEyePoint( mCameraCOI - lerp( distance, 5.0f, 0.1f ) * mCamera.getViewDirection() );
		mCamera.setCenterOfInterestPoint( lerp( mCamera.getCenterOfInterestPoint(), mCameraCOI, 0.25f) );
	}
}

void GeometryApp::draw()
{
	// Prepare for drawing.
	gl::clear();
	gl::setMatrices( mCamera );
	
	// Draw the grid.
	if( mShowGrid && mGrid ) {
		gl::ScopedGlslProg scopedGlslProg( gl::context()->getStockShader( gl::ShaderDef().color() ) );
		// draw the coordinate frame with length 2.
		gl::drawCoordinateFrame( 2 );
		mGrid->draw();
	}

	if( mPrimitive ) {
		gl::ScopedTextureBind scopedTextureBind( mTexture );
		mPhongShader->uniform( "uTexturingMode", mTexturingMode );

		// Rotate it slowly around the y-axis.
		gl::ScopedModelMatrix matScope;
		gl::rotate( float( getElapsedSeconds() / 5 ), 0, 1, 0 );

		// Draw the normals.
		if( mShowNormals && mPrimitiveNormalLines ) {
			gl::ScopedColor colorScope( Color( 1, 1, 0 ) );
			mPrimitiveNormalLines->draw();
		}

		// Draw the tangents.
		if( mShowTangents && mPrimitiveTangentLines ) {
			gl::ScopedColor colorScope( Color( 0, 1, 0 ) );
			mPrimitiveTangentLines->draw();
		}

		// Draw the primitive.
		gl::ScopedColor colorScope( Color( 0.7f, 0.5f, 0.3f ) );

		if( mViewMode == WIREFRAME ) {
			// We're using alpha blending, so render the back side first.
			gl::ScopedAlphaBlend blendScope( false );
			gl::ScopedFaceCulling cullScope( true, GL_FRONT );

			mWireframeShader->uniform( "uBrightness", 0.5f );
			mPrimitiveWireframe->draw();

			// Now render the front side.
			gl::cullFace( GL_BACK );

			mWireframeShader->uniform( "uBrightness", 1.0f );
			mPrimitiveWireframe->draw();
		}
		else
			mPrimitive->draw();
	}

	// Render the parameter window.
#if ! defined( CINDER_GL_ES )
	if( mParams )
		mParams->draw();
#endif
}

void GeometryApp::mouseDown( MouseEvent event )
{
	mRecenterCamera = false;

	mMayaCam.setCurrentCam( mCamera );
	mMayaCam.mouseDown( event.getPos() );

	if( getElapsedSeconds() - mLastMouseDownTime < 0.2f ) {
		mPrimitiveSelected = static_cast<Primitive>( static_cast<int>(mPrimitiveSelected) + 1 );
		createGeometry();
	}

	mLastMouseDownTime = getElapsedSeconds();
}

void GeometryApp::mouseDrag( MouseEvent event )
{
	mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
	mCamera = mMayaCam.getCamera();
}

void GeometryApp::resize()
{
	mCamera.setAspectRatio( getWindowAspectRatio() );
	
	if(mWireframeShader)
		mWireframeShader->uniform( "uViewportSize", vec2( getWindowSize() ) );
}

void GeometryApp::keyDown( KeyEvent event )
{
	switch( event.getCode() ) {
		case KeyEvent::KEY_SPACE:
			mPrimitiveSelected = static_cast<Primitive>( static_cast<int>(mPrimitiveSelected) + 1 );
			createGeometry();
			break;
		case KeyEvent::KEY_c:
			mShowColors = ! mShowColors;
			createGeometry();
			break;
		case KeyEvent::KEY_n:
			mShowNormals = ! mShowNormals;
			break;
		case KeyEvent::KEY_g:
			mShowGrid = ! mShowGrid;
			break;
		case KeyEvent::KEY_q:
			mQualitySelected = Quality( (int)( mQualitySelected + 1 ) % 3 );
			break;
		case KeyEvent::KEY_w:
			if(mViewMode == WIREFRAME)
				mViewMode = SHADED;
			else
				mViewMode = WIREFRAME;
			break;
		case KeyEvent::KEY_RETURN:
			CI_LOG_V( "reload" );
			createPhongShader();
			createGeometry();
			break;
	}
}

void GeometryApp::createParams()
{
#if ! defined( CINDER_GL_ES )
	vector<string> primitives = { "Capsule", "Cone", "Cube", "Cylinder", "Helix", "Icosahedron", "Icosphere", "Sphere", "Teapot", "Torus", "Plane" };
	vector<string> qualities = { "Low", "Default", "High" };
	vector<string> viewModes = { "Shaded", "Wireframe" };
	vector<string> texturingModes = { "None", "Procedural", "Sampler" };

	mParams = params::InterfaceGl::create( getWindow(), "Geometry Demo", toPixels( ivec2( 300, 200 ) ) );
	mParams->setOptions( "", "valueswidth=160 refresh=0.1" );

	mParams->addParam( "Primitive", primitives, (int*) &mPrimitiveSelected );
	mParams->addParam( "Quality", qualities, (int*) &mQualitySelected );
	mParams->addParam( "Viewing Mode", viewModes, (int*) &mViewMode );
	mParams->addParam( "Texturing Mode", texturingModes, (int*) &mTexturingMode );

	mParams->addSeparator();

	mParams->addParam( "Subdivision", &mSubdivision ).min( 1 ).max( 5 ).updateFn( [this] { createGeometry(); } );

	mParams->addSeparator();

	mParams->addParam( "Show Grid", &mShowGrid );
	mParams->addParam( "Show Normals", &mShowNormals );
	mParams->addParam( "Show Tangents", &mShowTangents );
	mParams->addParam( "Show Colors", &mShowColors ).updateFn( [this] { createGeometry(); } );
	mParams->addParam( "Face Culling", &mEnableFaceFulling ).updateFn( [this] { gl::enableFaceCulling( mEnableFaceFulling ); } );
#endif
}

void GeometryApp::createGrid()
{
	mGrid = gl::VertBatch::create( GL_LINES );
	mGrid->begin( GL_LINES );
	for( int i = -10; i <= 10; ++i ) {
		if( i == 0 )
			continue;

		mGrid->color( Color( 0.25f, 0.25f, 0.25f ) );
		mGrid->color( Color( 0.25f, 0.25f, 0.25f ) );
		mGrid->color( Color( 0.25f, 0.25f, 0.25f ) );
		mGrid->color( Color( 0.25f, 0.25f, 0.25f ) );
		
		mGrid->vertex( float(i), 0.0f, -10.0f );
		mGrid->vertex( float(i), 0.0f, +10.0f );
		mGrid->vertex( -10.0f, 0.0f, float(i) );
		mGrid->vertex( +10.0f, 0.0f, float(i) );
	}
	mGrid->end();
}

void GeometryApp::createGeometry()
{
	geom::SourceRef primitive;

	switch( mPrimitiveCurrent ) {
		default:
			mPrimitiveSelected = CAPSULE;
		case CAPSULE:
			switch( mQualityCurrent ) {
				case DEFAULT:	loadGeomSource( geom::Capsule() ); break;
				case LOW:		loadGeomSource( geom::Capsule().subdivisionsAxis( 6 ).subdivisionsHeight( 1 ) ); break;
				case HIGH:		loadGeomSource( geom::Capsule().subdivisionsAxis( 60 ).subdivisionsHeight( 20 ) ); break;
			}
			break;
		case CONE:
			switch( mQualityCurrent ) {
				case DEFAULT:	loadGeomSource( geom::Cone() ); break;
				case LOW:		loadGeomSource( geom::Cone().subdivisionsAxis( 6 ).subdivisionsHeight( 1 ) ); break;
				case HIGH:		loadGeomSource( geom::Cone().subdivisionsAxis( 60 ).subdivisionsHeight( 60 ) ); break;
			}
			break;
		case CUBE:
			switch( mQualityCurrent ) {
				case DEFAULT:	loadGeomSource( geom::Cube() ); break;
				case LOW:		loadGeomSource( geom::Cube().subdivisions( 1 ) ); break;
				case HIGH:		loadGeomSource( geom::Cube().subdivisions( 10 ) ); break;
			}
			break;
		case CYLINDER:
			switch( mQualityCurrent ) {
				case DEFAULT:	loadGeomSource( geom::Cylinder() ); break;
				case LOW:		loadGeomSource( geom::Cylinder().subdivisionsAxis( 6 ) ); break;
				case HIGH:		loadGeomSource( geom::Cylinder().subdivisionsAxis( 60 ).subdivisionsHeight( 20 ) ); break;
			}
			break;
		case HELIX:
			switch( mQualityCurrent ) {
				case DEFAULT:	loadGeomSource( geom::Helix() ); break;
				case LOW:		loadGeomSource( geom::Helix().subdivisionsAxis( 12 ).subdivisionsHeight( 6 ) ); break;
				case HIGH:		loadGeomSource( geom::Helix().subdivisionsAxis( 60 ).subdivisionsHeight( 60 ) ); break;
			}
			break;
		case ICOSAHEDRON:
			loadGeomSource( geom::Icosahedron() );
			break;
		case ICOSPHERE:
			switch( mQualityCurrent ) {
				case DEFAULT:	loadGeomSource( geom::Icosphere() ); break;
				case LOW:		loadGeomSource( geom::Icosphere().subdivisions( 1 ) ); break;
				case HIGH:		loadGeomSource( geom::Icosphere().subdivisions( 5 ) ); break;
			}
			break;
		case SPHERE:
			switch( mQualityCurrent ) {
				case DEFAULT:	loadGeomSource( geom::Sphere() ); break;
				case LOW:		loadGeomSource( geom::Sphere().subdivisions( 6 ) ); break;
				case HIGH:		loadGeomSource( geom::Sphere().subdivisions( 60 ) ); break;
			}
			break;
		case TEAPOT:
			switch( mQualityCurrent ) {
				case DEFAULT:	loadGeomSource( geom::Teapot() ); break;
				case LOW:		loadGeomSource( geom::Teapot().subdivisions( 2 ) ); break;
				case HIGH:		loadGeomSource( geom::Teapot().subdivisions( 12 ) ); break;
			}
			break;
		case TORUS:
			switch( mQualityCurrent ) {
				case DEFAULT:	loadGeomSource( geom::Torus() ); break;
				case LOW:		loadGeomSource( geom::Torus().subdivisionsAxis( 12 ).subdivisionsHeight( 6 ) ); break;
				case HIGH:		loadGeomSource( geom::Torus().subdivisionsAxis( 60 ).subdivisionsHeight( 60 ) ); break;
			}
			break;
		case PLANE:
			ivec2 numSegments;
			switch( mQualityCurrent ) {
				case DEFAULT:	numSegments = ivec2( 10, 10 ); break;
				case LOW:		numSegments = ivec2( 2, 2 ); break;
				case HIGH:		numSegments = ivec2( 100, 100 ); break;
			}

			auto plane = geom::Plane().subdivisions( numSegments );

//			plane.normal( vec3( 0, 0, 1 ) ); // change the normal angle of the plane
//			plane.axes( vec3( 0.70710678118, -0.70710678118, 0 ), vec3( 0.70710678118, 0.70710678118, 0 ) ); // dictate plane u/v axes directly
//			plane.subdivisions( ivec2( 3, 10 ) ).size( vec2( 0.5f, 2.0f ) ).origin( vec3( 0, 1.0f, 0 ) ).normal( vec3( 0, 0, 1 ) ); // change the size and origin so that it is tall and thin, above the y axis.

			loadGeomSource( plane );
			break;
	}
}

void GeometryApp::loadGeomSource( const geom::Source &source )
{
	// The purpose of the TriMesh is to capture a bounding box; without that need we could just instantiate the Batch directly using primitive
	TriMesh::Format fmt = TriMesh::Format().positions().normals().texCoords().tangents();
	if( mShowColors && source.getAvailableAttribs().count( geom::COLOR ) > 0 )
		fmt.colors();

	TriMesh mesh( source, fmt );
	AxisAlignedBox3f bbox = mesh.calcBoundingBox();
	mCameraCOI = mesh.calcBoundingBox().getCenter();
	mRecenterCamera = true;

	if( mSubdivision > 1 )
		mesh.subdivide( mSubdivision );

	if( mPhongShader )
		mPrimitive = gl::Batch::create( mesh, mPhongShader );

	if( mWireframeShader )
		mPrimitiveWireframe = gl::Batch::create( mesh, mWireframeShader );

	vec3 size = bbox.getMax() - bbox.getMin();
	float scale = std::max( std::max( size.x, size.y ), size.z ) / 25.0f;
	mPrimitiveNormalLines = gl::Batch::create( mesh >> geom::VertexNormalLines( scale ), gl::getStockShader( gl::ShaderDef().color() ) );
	if( mesh.hasTangents() )
		mPrimitiveTangentLines = gl::Batch::create( mesh >> geom::VertexNormalLines( scale, geom::TANGENT ), gl::getStockShader( gl::ShaderDef().color() ) );
	else
		mPrimitiveTangentLines.reset();

	getWindow()->setTitle( "Geometry - " + to_string( mesh.getNumVertices() ) + " vertices" );
}

void GeometryApp::createPhongShader()
{
	try {
#if defined( CINDER_GL_ES )
		mPhongShader = gl::GlslProg::create( loadAsset( "phong_es2.vert" ), loadAsset( "phong_es2.frag" ) );
#else
		mPhongShader = gl::GlslProg::create( loadAsset( "phong.vert" ), loadAsset( "phong.frag" ) );
#endif
	}
	catch( Exception &exc ) {
		CI_LOG_E( "error loading phong shader: " << exc.what() );
	}
}

void GeometryApp::createWireframeShader()
{
#if ! defined( CINDER_GL_ES )
	try {
		auto format = gl::GlslProg::Format()
			.vertex( loadAsset( "wireframe.vert" ) )
			.geometry( loadAsset( "wireframe.geom" ) )
			.fragment( loadAsset( "wireframe.frag" ) );

		mWireframeShader = gl::GlslProg::create( format );
	}
	catch( Exception &exc ) {
		CI_LOG_E( "error loading wireframe shader: " << exc.what() );
	}
#endif // ! defined( CINDER_GL_ES )
}

CINDER_APP( GeometryApp, RendererGl, prepareSettings )
