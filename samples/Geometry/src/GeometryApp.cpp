#include "cinder/Camera.h"
#include "cinder/GeomIo.h"
#include "cinder/ImageIo.h"
#include "cinder/CameraUi.h"
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
	GeometryApp();

	enum Primitive { CAPSULE, CONE, CUBE, CYLINDER, HELIX, ICOSAHEDRON, ICOSPHERE, SPHERE, TEAPOT, TORUS, TORUSKNOT, PLANE, RECT, ROUNDEDRECT, CIRCLE, RING, PRIMITIVE_COUNT };
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

	void fileDrop( FileDropEvent event ) override;

private:
	void createGrid();
	void createPhongShader();
	void createWireShader();
	void createWireframeShader();
	void createGeometry();
	void loadGeomSource( const geom::Source &source, const geom::Source &sourceWire );
	void createParams();
	void updateParams();

	Primitive			mPrimitiveSelected;
	Primitive			mPrimitiveCurrent;
	Quality				mQualitySelected;
	Quality				mQualityCurrent;
	ViewMode			mViewMode;
	int					mTexturingMode;

	bool				mShowColors;
	bool				mShowNormals, mShowTangents;
	bool				mShowGrid;
	bool				mShowSolidPrimitive;
	bool				mShowWirePrimitive;
	bool				mEnableFaceFulling;

	CameraPersp			mCamera;
	CameraUi			mCamUi;
	bool				mRecenterCamera;
	vec3				mCameraTarget, mCameraLerpTarget, mCameraViewDirection;
	double				mLastMouseDownTime;

	gl::VertBatchRef	mGrid;

	gl::BatchRef		mPrimitive;
	gl::BatchRef		mPrimitiveWire;
	gl::BatchRef		mPrimitiveWireframe;
	gl::BatchRef		mPrimitiveNormalLines, mPrimitiveTangentLines;

	gl::GlslProgRef		mPhongShader;
	gl::GlslProgRef		mWireShader;
	gl::GlslProgRef		mWireframeShader;

	gl::TextureRef		mTexture;

	float				mCapsuleRadius;
	float				mCapsuleLength;

	float				mConeRatio;

	float				mHelixRatio;
	unsigned			mHelixTwist;
	float				mHelixOffset;
	float				mHelixCoils;

	float				mRingWidth;

	float				mRoundedRectRadius;

	unsigned			mTorusTwist;
	float				mTorusOffset;
	float				mTorusRatio;

	int					mTorusKnotP;
	int					mTorusKnotQ;
	float				mTorusKnotRadius;
	vec3				mTorusKnotScale;

#if ! defined( CINDER_GL_ES )
	params::InterfaceGlRef	mParams;

	typedef std::vector<params::InterfaceGl::OptionsBase> ParamGroup;
	std::vector<ParamGroup>	mPrimitiveParams;
#endif
};

void prepareSettings( App::Settings* settings )
{
	settings->setWindowSize( 1024, 768 );
	settings->setHighDensityDisplayEnabled();
	settings->setMultiTouchEnabled( false );
	settings->disableFrameRate();
}

GeometryApp::GeometryApp()
	: mCapsuleRadius( 0.5f ), mCapsuleLength( 1 )
	, mConeRatio( 0.5f )
	, mHelixRatio( 0.25f ), mHelixTwist( 0 ), mHelixOffset( 0 ), mHelixCoils( 3 )
	, mRingWidth( 0.25f )
	, mRoundedRectRadius( 0.2f )
	, mTorusTwist( 0 ), mTorusOffset( 0 ), mTorusRatio( 0.25 )
	, mTorusKnotP( 2 ), mTorusKnotQ( 5 ), mTorusKnotRadius( 0.15f ), mTorusKnotScale( 1, 0.2, 1 )
{
}

void GeometryApp::setup()
{
	gl::enableVerticalSync( true );

	// Initialize variables.
	mPrimitiveSelected = mPrimitiveCurrent = TEAPOT;
	mQualitySelected = mQualityCurrent = HIGH;
	mTexturingMode = PROCEDURAL;
	mViewMode = SHADED;
	mLastMouseDownTime = 0;
	mShowColors = false;
	mShowNormals = false;
	mShowTangents = false;
	mShowGrid = true;
	mShowSolidPrimitive = true;
	mShowWirePrimitive = false;
	mEnableFaceFulling = false;

	// Load the textures.
	gl::Texture::Format fmt;
	fmt.setAutoInternalFormat();
	fmt.setWrap( GL_REPEAT, GL_REPEAT );
	mTexture = gl::Texture::create( loadImage( loadAsset( "stripes.jpg" ) ), fmt );

	// Setup the camera.
	mCamera.lookAt( normalize( vec3( 3, 3, 6 ) ) * 5.0f, mCameraTarget );
	mCamUi = CameraUi( &mCamera );

	// Load and compile the shaders.
	createPhongShader();
	createWireShader();
	createWireframeShader();

	// Create the meshes.
	createGrid();
	createGeometry();

	// Create a parameter window, so we can toggle stuff.
	createParams();
}

void GeometryApp::update()
{
	// If another primitive or quality was selected, reset the subdivision and recreate the primitive.
	if( mPrimitiveCurrent != mPrimitiveSelected || mQualitySelected != mQualityCurrent ) {
		mPrimitiveCurrent = mPrimitiveSelected;
		mQualityCurrent = mQualitySelected;
		createGeometry();
	}

	// After creating a new primitive, gradually move the camera to get a good view.
	if( mRecenterCamera ) {
		float distance = glm::distance( mCamera.getEyePoint(), mCameraLerpTarget );
		vec3 eye = mCameraLerpTarget - lerp( distance, 5.0f, 0.25f ) * mCameraViewDirection;
		mCameraTarget = lerp( mCameraTarget, mCameraLerpTarget, 0.25f );
		mCamera.lookAt( eye, mCameraTarget );
	}
}

void GeometryApp::draw()
{
	// Prepare for drawing.
	gl::clear();
	gl::setMatrices( mCamera );

	// Enable the depth buffer.
	gl::enableDepthRead();
	gl::enableDepthWrite();

	if( mPrimitive ) {
		gl::ScopedTextureBind scopedTextureBind( mTexture );
		mPhongShader->uniform( "uTexturingMode", mTexturingMode );
		mPhongShader->uniform( "uFreq", ( mPrimitiveCurrent == TORUSKNOT ) ? ivec2( 100, 10 ) : ivec2( 20 ) );

		// Rotate it slowly around the y-axis.
		gl::ScopedModelMatrix matScope;
		//gl::rotate( float( getElapsedSeconds() / 5 ), 0, 1, 0 );

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

		// Draw the wire primitive.
		if( mShowWirePrimitive && mPrimitiveWire ) {
			gl::ScopedColor color( Color( 1, 1, 1 ) );
			gl::ScopedLineWidth linewidth( 2.5f );

			mPrimitiveWire->draw();
		}

		// Draw the primitive.
		if( mShowSolidPrimitive ) {
			gl::ScopedColor colorScope( Color( 1, 1, 1 ) );

			if( mViewMode == WIREFRAME ) {
				// We're using alpha blending, so render the back side first.
				gl::ScopedBlendAlpha blendScope;
				gl::ScopedFaceCulling cullScope( true, GL_FRONT );

				mWireframeShader->uniform( "uBrightness", 0.5f );
				mPrimitiveWireframe->draw();

				// Now render the front side.
				gl::cullFace( GL_BACK );

				mWireframeShader->uniform( "uBrightness", 1.0f );
				mPrimitiveWireframe->draw();
			}
			else {
				gl::ScopedFaceCulling cullScope( mEnableFaceFulling, GL_BACK );

				mPrimitive->draw();
			}
		}
	}

	// 
	gl::disableDepthWrite();

	// Draw the grid.
	if( mShowGrid && mGrid ) {
		gl::ScopedGlslProg scopedGlslProg( gl::context()->getStockShader( gl::ShaderDef().color() ) );

		mGrid->draw();

		// draw the coordinate frame with length 2.
		gl::drawCoordinateFrame( 2 );
	}

	// Disable the depth buffer.
	gl::disableDepthRead();

	// Render the parameter windows.
#if ! defined( CINDER_GL_ES )
	if( mParams ) {
		mParams->draw();
	}
#endif
}

void GeometryApp::mouseDown( MouseEvent event )
{
	mRecenterCamera = false;

	mCamUi.mouseDown( event );

	if( getElapsedSeconds() - mLastMouseDownTime < 0.2f ) {
		mPrimitiveSelected = static_cast<Primitive>( static_cast<int>( mPrimitiveSelected ) + 1 );
		createGeometry();
	}

	mLastMouseDownTime = getElapsedSeconds();
}

void GeometryApp::mouseDrag( MouseEvent event )
{
	mCamUi.mouseDrag( event );
}

void GeometryApp::resize()
{
	mCamera.setAspectRatio( getWindowAspectRatio() );

	if( mWireframeShader )
		mWireframeShader->uniform( "uViewportSize", vec2( getWindowSize() ) );
}

void GeometryApp::keyDown( KeyEvent event )
{
	switch( event.getCode() ) {
		case KeyEvent::KEY_SPACE:
			mPrimitiveSelected = static_cast<Primitive>( ( static_cast<int>( mPrimitiveSelected ) + 1 ) % PRIMITIVE_COUNT );
			createGeometry();
			updateParams();
			break;
		case KeyEvent::KEY_c:
			mShowColors = !mShowColors;
			createGeometry();
			break;
		case KeyEvent::KEY_n:
			mShowNormals = !mShowNormals;
			break;
		case KeyEvent::KEY_g:
			mShowGrid = !mShowGrid;
			break;
		case KeyEvent::KEY_q:
			mQualitySelected = Quality( (int)( mQualitySelected + 1 ) % 3 );
			break;
#if ! defined( CINDER_GL_ES )
		case KeyEvent::KEY_v:
			if( mViewMode == WIREFRAME )
				mViewMode = SHADED;
			else
				mViewMode = WIREFRAME;
			break;
#endif 
		case KeyEvent::KEY_w:
			mShowWirePrimitive = !mShowWirePrimitive;
			break;
		case KeyEvent::KEY_RETURN:
			CI_LOG_V( "reload" );
			createPhongShader();
			createGeometry();
			break;
	}
}

void GeometryApp::fileDrop( FileDropEvent event )
{
	try {
		gl::Texture::Format fmt;
		fmt.setAutoInternalFormat();
		fmt.setWrap( GL_REPEAT, GL_REPEAT );

		mTexture = gl::Texture2d::create( loadImage( event.getFile( 0 ) ), fmt );
	}
	catch( const std::exception &exc ) {
	}
}

void GeometryApp::createParams()
{
#if ! defined( CINDER_GL_ES )
	vector<string> primitives = { "Capsule", "Cone", "Cube", "Cylinder", "Helix", "Icosahedron", "Icosphere", "Sphere", "Teapot", "Torus", "Torus Knot", "Plane", "Rectangle", "Rounded Rectangle", "Circle", "Ring" };
	vector<string> qualities = { "Low", "Default", "High" };
	vector<string> viewModes = { "Shaded", "Wireframe" };
	vector<string> texturingModes = { "None", "Procedural", "Sampler" };

	mParams = params::InterfaceGl::create( getWindow(), "Geometry Demo", toPixels( ivec2( 300, 400 ) ) );
	mParams->setOptions( "", "valueswidth=100 refresh=0.1" );

	mParams->addParam( "Primitive", primitives, (int*)&mPrimitiveSelected ).updateFn( std::bind( &GeometryApp::updateParams, this ) );
	mParams->addParam( "Quality", qualities, (int*)&mQualitySelected );
	mParams->addParam( "Viewing Mode", viewModes, (int*)&mViewMode );
	mParams->addParam( "Texturing Mode", texturingModes, (int*)&mTexturingMode );

	mParams->addSeparator();

	mParams->addParam( "Show Grid", &mShowGrid );
	mParams->addParam( "Show Normals", &mShowNormals );
	mParams->addParam( "Show Tangents", &mShowTangents );
	mParams->addParam( "Show Colors", &mShowColors ).updateFn( [this] { createGeometry(); } );
	mParams->addParam( "Face Culling", &mEnableFaceFulling ).updateFn( [this] { gl::enableFaceCulling( mEnableFaceFulling ); } );

	mParams->addSeparator();

	mParams->addParam( "Show Wire Primitive", &mShowWirePrimitive );
	mParams->addParam( "Show Solid Primitive", &mShowSolidPrimitive );

	mParams->addSeparator();

	//
	mPrimitiveParams.resize( PRIMITIVE_COUNT );

	// Capsule
	mPrimitiveParams[CAPSULE].push_back( mParams->addParam( "Capsule: Radius", &mCapsuleRadius ).step( 0.01f ).updateFn( [this] { createGeometry(); } ) );
	mPrimitiveParams[CAPSULE].push_back( mParams->addParam( "Capsule: Length", &mCapsuleLength ).step( 0.05f ).updateFn( [this] { createGeometry(); } ) );

	// Cone
	mPrimitiveParams[CONE].push_back( mParams->addParam( "Cone: Ratio", &mConeRatio ).step( 0.01f ).updateFn( [this] { createGeometry(); } ) );

	// Helix
	mPrimitiveParams[HELIX].push_back( mParams->addParam( "Helix: Ratio", &mHelixRatio ).step( 0.01f ).updateFn( [this] { createGeometry(); } ) );
	mPrimitiveParams[HELIX].push_back( mParams->addParam( "Helix: Coils", &mHelixCoils ).step( 0.1f ).updateFn( [this] { createGeometry(); } ) );
	mPrimitiveParams[HELIX].push_back( mParams->addParam( "Helix: Twist", &mHelixTwist ).updateFn( [this] { createGeometry(); } ) );
	mPrimitiveParams[HELIX].push_back( mParams->addParam( "Helix: Twist Offset", &mHelixOffset ).step( 0.05f ).updateFn( [this] { createGeometry(); } ) );

	// Ring
	mPrimitiveParams[RING].push_back( mParams->addParam( "Ring: Width", &mRingWidth ).step( 0.01f ).updateFn( [this] { createGeometry(); } ) );

	// Rounded Rect
	mPrimitiveParams[ROUNDEDRECT].push_back( mParams->addParam( "Corner Radius", &mRoundedRectRadius ).step( 0.01f ).updateFn( [this] { createGeometry(); } ) );

	// Torus
	mPrimitiveParams[TORUS].push_back( mParams->addParam( "Torus: Ratio", &mTorusRatio ).step( 0.01f ).updateFn( [this] { createGeometry(); } ) );
	mPrimitiveParams[TORUS].push_back( mParams->addParam( "Torus: Twist", &mTorusTwist ).updateFn( [this] { createGeometry(); } ) );
	mPrimitiveParams[TORUS].push_back( mParams->addParam( "Torus: Twist Offset", &mTorusOffset ).step( 0.05f ).updateFn( [this] { createGeometry(); } ) );

	// Torus Knot
	mPrimitiveParams[TORUSKNOT].push_back( mParams->addParam( "Torus Knot: Parameter P", &mTorusKnotP ).updateFn( [this] { createGeometry(); } ) );
	mPrimitiveParams[TORUSKNOT].push_back( mParams->addParam( "Torus Knot: Parameter Q", &mTorusKnotQ ).updateFn( [this] { createGeometry(); } ) );
	mPrimitiveParams[TORUSKNOT].push_back( mParams->addParam( "Torus Knot: Scale X", &mTorusKnotScale.x ).step( 0.1f ).updateFn( [this] { createGeometry(); } ) );
	mPrimitiveParams[TORUSKNOT].push_back( mParams->addParam( "Torus Knot: Scale Y", &mTorusKnotScale.y ).step( 0.1f ).updateFn( [this] { createGeometry(); } ) );
	mPrimitiveParams[TORUSKNOT].push_back( mParams->addParam( "Torus Knot: Scale Z", &mTorusKnotScale.z ).step( 0.1f ).updateFn( [this] { createGeometry(); } ) );
	mPrimitiveParams[TORUSKNOT].push_back( mParams->addParam( "Torus Knot: Radius", &mTorusKnotRadius ).step( 0.05f ).updateFn( [this] { createGeometry(); } ) );

	updateParams();
#endif
}

void GeometryApp::updateParams()
{
#if ! defined( CINDER_GL_ES )
	for( int i = 0; i < mPrimitiveParams.size(); ++i ) {
		for( auto &param : mPrimitiveParams[i] ) {
			param.setVisible( i == mPrimitiveSelected );
		}
	}
#endif
}

void GeometryApp::createGrid()
{
	mGrid = gl::VertBatch::create( GL_LINES );
	mGrid->begin( GL_LINES );
	for( int i = -10; i <= 10; ++i ) {
		mGrid->color( Color( 0.25f, 0.25f, 0.25f ) );
		mGrid->color( Color( 0.25f, 0.25f, 0.25f ) );
		mGrid->color( Color( 0.25f, 0.25f, 0.25f ) );
		mGrid->color( Color( 0.25f, 0.25f, 0.25f ) );

		mGrid->vertex( float( i ), 0.0f, -10.0f );
		mGrid->vertex( float( i ), 0.0f, +10.0f );
		mGrid->vertex( -10.0f, 0.0f, float( i ) );
		mGrid->vertex( +10.0f, 0.0f, float( i ) );
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
				case DEFAULT:	loadGeomSource( geom::Capsule().radius( mCapsuleRadius ).length( mCapsuleLength ), geom::WireCapsule().radius( mCapsuleRadius ).length( mCapsuleLength ) ); break;
				case LOW:		loadGeomSource( geom::Capsule().radius( mCapsuleRadius ).length( mCapsuleLength ).subdivisionsAxis( 6 ).subdivisionsHeight( 1 ), geom::WireCapsule().radius( mCapsuleRadius ).length( mCapsuleLength ) ); break;
				case HIGH:		loadGeomSource( geom::Capsule().radius( mCapsuleRadius ).length( mCapsuleLength ).subdivisionsAxis( 60 ).subdivisionsHeight( 20 ), geom::WireCapsule().radius( mCapsuleRadius ).length( mCapsuleLength ) ); break;
			}
			break;
		case CONE:
			switch( mQualityCurrent ) {
				case DEFAULT:	loadGeomSource( geom::Cone().ratio( mConeRatio ), geom::WireCone() ); break;
				case LOW:		loadGeomSource( geom::Cone().ratio( mConeRatio ).subdivisionsAxis( 6 ).subdivisionsHeight( 1 ), geom::WireCone() ); break;
				case HIGH:		loadGeomSource( geom::Cone().ratio( mConeRatio ).subdivisionsAxis( 60 ).subdivisionsHeight( 60 ), geom::WireCone() ); break;
			}
			break;
		case CUBE:
			switch( mQualityCurrent ) {
				case DEFAULT:	loadGeomSource( geom::Cube(), geom::WireCube() ); break;
				case LOW:		loadGeomSource( geom::Cube().subdivisions( 1 ), geom::WireCube() ); break;
				case HIGH:		loadGeomSource( geom::Cube().subdivisions( 10 ), geom::WireCube() ); break;
			}
			break;
		case CYLINDER:
			switch( mQualityCurrent ) {
				case DEFAULT:	loadGeomSource( geom::Cylinder(), geom::WireCylinder() ); break;
				case LOW:		loadGeomSource( geom::Cylinder().subdivisionsAxis( 6 ).subdivisionsCap( 1 ), geom::WireCylinder() ); break;
				case HIGH:		loadGeomSource( geom::Cylinder().subdivisionsAxis( 60 ).subdivisionsHeight( 20 ).subdivisionsCap( 10 ), geom::WireCylinder() ); break;
			}
			break;
		case HELIX:
			switch( mQualityCurrent ) {
				case DEFAULT:	loadGeomSource( geom::Helix().ratio( mHelixRatio ).coils( mHelixCoils ).twist( mHelixTwist, mHelixOffset ), geom::WireCube() ); break;
				case LOW:		loadGeomSource( geom::Helix().ratio( mHelixRatio ).coils( mHelixCoils ).twist( mHelixTwist, mHelixOffset ).subdivisionsAxis( 12 ).subdivisionsHeight( 6 ), geom::WireCube() ); break;
				case HIGH:		loadGeomSource( geom::Helix().ratio( mHelixRatio ).coils( mHelixCoils ).twist( mHelixTwist, mHelixOffset ).subdivisionsAxis( 60 ).subdivisionsHeight( 60 ), geom::WireCube() ); break;
			}
			break;
		case ICOSAHEDRON:
			loadGeomSource( geom::Icosahedron(), geom::WireIcosahedron() );
			break;
		case ICOSPHERE:
			switch( mQualityCurrent ) {
				case DEFAULT:	loadGeomSource( geom::Icosphere(), geom::WireSphere() ); break;
				case LOW:		loadGeomSource( geom::Icosphere().subdivisions( 1 ), geom::WireSphere() ); break;
				case HIGH:		loadGeomSource( geom::Icosphere().subdivisions( 5 ), geom::WireSphere() ); break;
			}
			break;
		case SPHERE:
			switch( mQualityCurrent ) {
				case DEFAULT:	loadGeomSource( geom::Sphere(), geom::WireSphere() ); break;
				case LOW:		loadGeomSource( geom::Sphere().subdivisions( 6 ), geom::WireSphere() ); break;
				case HIGH:		loadGeomSource( geom::Sphere().subdivisions( 60 ), geom::WireSphere() ); break;
			}
			break;
		case TEAPOT:
			switch( mQualityCurrent ) {
				case DEFAULT:	loadGeomSource( geom::Teapot(), geom::WireCube() ); break;
				case LOW:		loadGeomSource( geom::Teapot().subdivisions( 2 ), geom::WireCube() ); break;
				case HIGH:		loadGeomSource( geom::Teapot().subdivisions( 12 ), geom::WireCube() ); break;
			}
			break;
		case TORUS:
			switch( mQualityCurrent ) {
				case DEFAULT:	loadGeomSource( geom::Torus().twist( mTorusTwist, mTorusOffset ).ratio( mTorusRatio ), geom::WireTorus().ratio( mTorusRatio ) ); break;
				case LOW:		loadGeomSource( geom::Torus().twist( mTorusTwist, mTorusOffset ).ratio( mTorusRatio ).subdivisionsAxis( 12 ).subdivisionsHeight( 6 ), geom::WireTorus().ratio( mTorusRatio ) ); break;
				case HIGH:		loadGeomSource( geom::Torus().twist( mTorusTwist, mTorusOffset ).ratio( mTorusRatio ).subdivisionsAxis( 60 ).subdivisionsHeight( 60 ), geom::WireTorus().ratio( mTorusRatio ) ); break;
			}
			break;
		case TORUSKNOT:
			switch( mQualityCurrent ) {
				case DEFAULT:	loadGeomSource( geom::TorusKnot().parameters( mTorusKnotP, mTorusKnotQ ).radius( mTorusKnotRadius ).scale( mTorusKnotScale ), geom::WireCube() ); break;
				case LOW:		loadGeomSource( geom::TorusKnot().parameters( mTorusKnotP, mTorusKnotQ ).radius( mTorusKnotRadius ).scale( mTorusKnotScale ).subdivisionsAxis( 6 ).subdivisionsHeight( 64 ), geom::WireCube() ); break;
				case HIGH:		loadGeomSource( geom::TorusKnot().parameters( mTorusKnotP, mTorusKnotQ ).radius( mTorusKnotRadius ).scale( mTorusKnotScale ).subdivisionsAxis( 32 ).subdivisionsHeight( 1024 ), geom::WireCube() ); break;
			}
			break;
		case PLANE:
			switch( mQualityCurrent ) {
				case DEFAULT:	loadGeomSource( geom::Plane().subdivisions( ivec2( 10, 10 ) ), geom::WirePlane().subdivisions( ivec2( 10, 10 ) ) ); break;
				case LOW:		loadGeomSource( geom::Plane().subdivisions( ivec2( 2, 2 ) ), geom::WirePlane().subdivisions( ivec2( 2, 2 ) ) ); break;
				case HIGH:		loadGeomSource( geom::Plane().subdivisions( ivec2( 100, 100 ) ), geom::WirePlane().subdivisions( ivec2( 100, 100 ) ) ); break;
			}
			break;

		case RECT:
			loadGeomSource( geom::Rect(), geom::WirePlane() ); break;
			break;
		case ROUNDEDRECT:
			switch( mQualityCurrent ) {
				case DEFAULT:	loadGeomSource( geom::RoundedRect().cornerRadius( mRoundedRectRadius ).cornerSubdivisions( 3 ), geom::WireRoundedRect().cornerRadius( mRoundedRectRadius ).cornerSubdivisions( 3 ) ); break;
				case LOW:		loadGeomSource( geom::RoundedRect().cornerRadius( mRoundedRectRadius ).cornerSubdivisions( 1 ), geom::WireRoundedRect().cornerRadius( mRoundedRectRadius ).cornerSubdivisions( 1 ) ); break;
				case HIGH:		loadGeomSource( geom::RoundedRect().cornerRadius( mRoundedRectRadius ).cornerSubdivisions( 9 ), geom::WireRoundedRect().cornerRadius( mRoundedRectRadius ).cornerSubdivisions( 9 ) ); break;
			}
			break;
		case CIRCLE:
			switch( mQualityCurrent ) {
				case DEFAULT:	loadGeomSource( geom::Circle().subdivisions( 24 ), geom::WireCircle().subdivisions( 24 ) ); break;
				case LOW:		loadGeomSource( geom::Circle().subdivisions( 8 ), geom::WireCircle().subdivisions( 8 ) ); break;
				case HIGH:		loadGeomSource( geom::Circle().subdivisions( 120 ), geom::WireCircle().subdivisions( 120 ) ); break;
			}
			break;
		case RING:
			switch( mQualityCurrent ) {
				case DEFAULT:	loadGeomSource( geom::Ring().width( mRingWidth ).subdivisions( 24 ), geom::WireCircle().subdivisions( 24 ).radius( 1 + 0.5f * mRingWidth ) ); break;
				case LOW:		loadGeomSource( geom::Ring().width( mRingWidth ).subdivisions( 8 ), geom::WireCircle().subdivisions( 8 ).radius( 1 + 0.5f * mRingWidth ) ); break;
				case HIGH:		loadGeomSource( geom::Ring().width( mRingWidth ).subdivisions( 120 ), geom::WireCircle().subdivisions( 120 ).radius( 1 + 0.5f * mRingWidth ) ); break;
			}
			break;
	}
}

void GeometryApp::loadGeomSource( const geom::Source &source, const geom::Source &sourceWire )
{
	// The purpose of the TriMesh is to capture a bounding box; without that need we could just instantiate the Batch directly using primitive
	TriMesh::Format fmt = TriMesh::Format().positions().normals().texCoords().tangents();
	if( mShowColors && source.getAvailableAttribs().count( geom::COLOR ) > 0 )
		fmt.colors();

	TriMesh mesh( source, fmt );
	AxisAlignedBox bbox = mesh.calcBoundingBox();
	mCameraLerpTarget = mesh.calcBoundingBox().getCenter();
	mCameraViewDirection = mCamera.getViewDirection();
	mRecenterCamera = true;

	if( mPhongShader )
		mPrimitive = gl::Batch::create( mesh, mPhongShader );

	if( mWireShader )
		mPrimitiveWire = gl::Batch::create( sourceWire, mWireShader );

	if( mWireframeShader )
		mPrimitiveWireframe = gl::Batch::create( mesh, mWireframeShader );

	vec3 size = bbox.getMax() - bbox.getMin();
	float scale = std::max( std::max( size.x, size.y ), size.z ) / 25.0f;
	mPrimitiveNormalLines = gl::Batch::create( mesh >> geom::VertexNormalLines( scale ), gl::getStockShader( gl::ShaderDef().color() ) );
	if( mesh.hasTangents() )
		mPrimitiveTangentLines = gl::Batch::create( mesh >> geom::VertexNormalLines( scale, geom::TANGENT ), gl::getStockShader( gl::ShaderDef().color() ) );
	else
		mPrimitiveTangentLines.reset();

	getWindow()->setTitle( "Geometry - " + to_string( mesh.getNumVertices() ) + " vertices - " + to_string( sourceWire.getNumVertices() / 2 ) + " lines " );
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

void GeometryApp::createWireShader()
{
	try {
		mWireShader = gl::context()->getStockShader( gl::ShaderDef().color() );
	}
	catch( Exception &exc ) {
		CI_LOG_E( "error loading wire shader: " << exc.what() );
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

CINDER_APP( GeometryApp, RendererGl( RendererGl::Options().msaa( 16 ) ), prepareSettings )
