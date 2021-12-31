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
#include "cinder/CinderImGui.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

void prepareSettings( App::Settings* settings );

class GeometryApp : public App {
public:
	GeometryApp();

	enum Primitive { CAPSULE, CONE, CUBE, CUBESPHERE, CYLINDER, HELIX, ICOSAHEDRON, ICOSPHERE, SPHERE, TEAPOT, TORUS, TORUSKNOT, PLANE, RECT, ROUNDEDRECT, CIRCLE, RING, PRIMITIVE_COUNT };
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
	void updateGui();

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

	bool				mCubeSphereEqualSpacing;

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
	, mCubeSphereEqualSpacing( true )
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

	ImGui::Initialize();
}

void GeometryApp::update()
{
	updateGui();

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

void GeometryApp::updateGui()
{
#if ! defined( CINDER_GL_ES )
	static vector<string> primitives = { "Capsule", "Cone", "Cube", "Cube Sphere", "Cylinder", "Helix", "Icosahedron", "Icosphere", "Sphere", "Teapot", "Torus", "Torus Knot", "Plane", "Rectangle", "Rounded Rectangle", "Circle", "Ring" };
	static vector<string> qualities = { "Low", "Default", "High" };
	static vector<string> viewModes = { "Shaded", "Wireframe" };
	static vector<string> texturingModes = { "None", "Procedural", "Sampler" };

	ImGui::Begin( "Geometry Demo" );
	
	ImGui::Combo( "Primitive", (int*)&mPrimitiveSelected, primitives );
	ImGui::Combo( "Quality", (int*)&mQualitySelected, qualities );
	ImGui::Combo( "Viewing Mode", (int*)&mViewMode, viewModes );
	ImGui::Combo( "Texturing Mode", (int*)&mTexturingMode, texturingModes );

	ImGui::Separator();

	ImGui::Checkbox( "Show Grid", &mShowGrid );
	ImGui::Checkbox( "Show Normals", &mShowNormals );
	ImGui::Checkbox( "Show Tangents", &mShowTangents );
	if( ImGui::Checkbox( "Show Colors", &mShowColors ) ) createGeometry();
	if( ImGui::Checkbox( "Face Culling", &mEnableFaceFulling ) ) gl::enableFaceCulling( mEnableFaceFulling );

	ImGui::Separator();

	ImGui::Checkbox( "Show Wire Primitive", &mShowWirePrimitive );
	ImGui::Checkbox( "Show Solid Primitive", &mShowSolidPrimitive );

	ImGui::Separator();

	bool changed = false;

	switch( mPrimitiveSelected ) {
	case GeometryApp::CAPSULE:
		changed |= ImGui::DragFloat( "Capsule: Radius", &mCapsuleRadius, 0.01f );
		changed |= ImGui::DragFloat( "Capsule: Length", &mCapsuleLength, 0.05f );
		break;
	case GeometryApp::CONE:
		changed |= ImGui::DragFloat( "Cone: Ratio", &mConeRatio, 0.01f );
		break;
	case GeometryApp::CUBESPHERE:
		changed |= ImGui::Checkbox( "Cube Sphere: Equal Spacing", &mCubeSphereEqualSpacing );
		break;
	case GeometryApp::HELIX:
		changed |= ImGui::DragFloat( "Helix: Ratio", &mHelixRatio, 0.01f );
		changed |= ImGui::DragFloat( "Helix: Coils", &mHelixCoils, 0.1f );
		changed |= ImGui::DragInt( "Helix: Twist", (int*)&mHelixTwist );
		changed |= ImGui::DragFloat( "Helix: Twist Offset", &mHelixOffset, 0.05f );
		break;
	case GeometryApp::TORUS:
		changed |= ImGui::DragFloat( "Torus: Ratio", &mTorusRatio, 0.01f );
		changed |= ImGui::DragInt( "Torus: Twist", (int*)&mTorusTwist );
		changed |= ImGui::DragFloat( "Torus: Twist Offset", &mTorusOffset, 0.05f );
		break;
	case GeometryApp::TORUSKNOT:
		changed |= ImGui::DragInt( "Torus Knot: Parameter P", &mTorusKnotP );
		changed |= ImGui::DragInt( "Torus Knot: Parameter Q", &mTorusKnotQ );
		changed |= ImGui::DragFloat3( "Torus Knot: Scale", &mTorusKnotScale, 0.1f );
		changed |= ImGui::DragFloat( "Torus Knot: Radius", &mTorusKnotRadius, 0.05f );
		break;
	case GeometryApp::ROUNDEDRECT:
		changed |= ImGui::DragFloat( "Corner Radius", &mRoundedRectRadius, 0.01f );
		break;
	case GeometryApp::RING:
		changed |= ImGui::DragFloat( "Ring: Width", &mRingWidth, 0.01f );
		break;
	default:
		break;
	}
	ImGui::End();

	if( changed ) createGeometry();
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
				case HIGH:		loadGeomSource( geom::Cube().subdivisions( 64 ), geom::WireCube() ); break;
			}
			break;
		case CUBESPHERE:
			switch( mQualityCurrent ) {
				case DEFAULT:	loadGeomSource( geom::CubeSphere().equalSpacing( mCubeSphereEqualSpacing ), geom::WireSphere() ); break;
				case LOW:		loadGeomSource( geom::CubeSphere().equalSpacing( mCubeSphereEqualSpacing ).subdivisions( 4 ), geom::WireSphere() ); break;
				case HIGH:		loadGeomSource( geom::CubeSphere().equalSpacing( mCubeSphereEqualSpacing ).subdivisions( 64 ), geom::WireSphere() ); break;
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
			loadGeomSource( geom::Rect(), geom::WireRect() ); break;
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
