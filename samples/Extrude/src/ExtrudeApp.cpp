#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/Font.h"
#if ! defined( CINDDER_GL_ES )
	#include "cinder/CinderImGui.h"
#endif
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;

class ExtrudeApp : public App {
  public:	
	void	setup() override;
	void	keyDown( KeyEvent event ) override;
	void	resize() override;
	void	update() override;
	void	draw() override;
	
	void	randomSpline();
	void	makeGeom();
	
	CameraPersp				mCam;
	bool					mDrawNormals, mDrawWireframe, mCaps, mUseSpline;
	gl::BatchRef			mBatch, mNormalsBatch;
	gl::BatchRef			mSplineBatch;
	gl::GlslProgRef			mGlsl;
	ci::Font				mFont;
	mat4					mRotation;
	char					mCurrentChar;
	float					mApproximation, mDepth;
	int						mSubdivisions;
	BSpline3f				mSpline;
};

void ExtrudeApp::setup()
{
	gl::enableDepthRead();
	gl::enableDepthWrite();
	gl::enableFaceCulling();

	mDrawNormals = false;
	mDrawWireframe = false;
	mUseSpline = true;
	mCaps = true;
	mApproximation = 2.5f;
	mDepth = 2.2f;
	mSubdivisions = 30;
#if ! defined( CINDER_GL_ES )
	ImGui::Initialize();
#endif

	mCam.lookAt( vec3( 30, 20, 40 ), vec3( 0 ) );

	mGlsl = gl::getStockShader( gl::ShaderDef().color().lambert() );
	
	mFont = ci::Font( "Georgia", 32 );
	mCurrentChar = '&';
	randomSpline();
	makeGeom();
	
	mGlsl->bind();
}

void ExtrudeApp::keyDown( KeyEvent event )
{
	mCurrentChar = event.getChar();
	randomSpline();
	makeGeom();
}

void ExtrudeApp::randomSpline()
{
	// make a random B-Spline
	std::vector<vec3> points;
	float len = 10;
	float offset = 3;
	points.push_back( vec3( randFloat( -20, 20 ), randFloat( -30, 30 ), offset + -1 * len) );
	points.push_back( vec3( randFloat( -20, 20 ), randFloat( -30, 30 ), offset + 0 * len) );
	points.push_back( vec3( randFloat( -20, 20 ), 0, offset + 1 * len) );
	points.push_back( vec3( randFloat( -20, 20 ), randFloat( -30, 30 ), offset + 2 * len ) );
	mSpline = BSpline3f( points, 3, false, true );
}

void ExtrudeApp::makeGeom()
{
	// get the shape for this character
	Shape2d shape = mFont.getGlyphShape( mFont.getGlyphChar( mCurrentChar ) );
	auto bounds = shape.calcPreciseBoundingBox();
	shape.transform( glm::translate( mat3(), -bounds.getCenter() ) );

	// this remaps the TEX_COORD_0s to color; it has to be so explicit because VC++ balks about ambiguous calls
	std::function<Colorf(vec3)> texCoordToColor = []( vec3 v ) ->Colorf { return Colorf( v.x, v.y, v.z ); };
	
	if( mUseSpline ) {
		auto extrudeSource = geom::ExtrudeSpline( shape, mSpline, mSubdivisions, mApproximation ).caps( mCaps );
		mBatch = gl::Batch::create( extrudeSource >> geom::ColorFromAttrib( geom::TEX_COORD_0,
											texCoordToColor ), mGlsl );

		mNormalsBatch = gl::Batch::create( extrudeSource >> geom::VertexNormalLines( 3.0f ), gl::getStockShader( gl::ShaderDef().color() ) );
		mSplineBatch = gl::Batch::create( geom::BSpline( mSpline, 100 ), gl::getStockShader( gl::ShaderDef().color() ) );
	}
	else {
		auto extrudeSource = geom::Extrude( shape, mDepth, mApproximation ).caps( mCaps ).subdivisions( mSubdivisions );
		// this remaps the TEX_COORD_0s to color
		mBatch = gl::Batch::create( extrudeSource >> geom::ColorFromAttrib( geom::TEX_COORD_0,
											texCoordToColor ), mGlsl );
		
		mNormalsBatch = gl::Batch::create( extrudeSource >> geom::VertexNormalLines( 3.0f ), gl::getStockShader( gl::ShaderDef().color() ) );
	}
}

void ExtrudeApp::resize()
{
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
	gl::setMatrices( mCam );
}

void ExtrudeApp::update()
{
#if ! defined( CINDER_GL_ES )
	ImGui::Begin( "App Parameters " );
	bool changed = false;
	changed |= ImGui::DragFloat( "Approximation", &mApproximation, 0.1f, 0.1f, 20.0f );
	changed |= ImGui::DragFloat( "Depth", &mDepth, 0.25f, 0.01f, 7.0f );
	changed |= ImGui::DragInt( "Subdivisions", &mSubdivisions, 1, 1, 30 );
	changed |= ImGui::Checkbox( "Spline", &mUseSpline );
	changed |= ImGui::Checkbox( "Caps", &mCaps );
	changed |= ImGui::Checkbox( "Wireframe", &mDrawWireframe );
	changed |= ImGui::Checkbox( "Draw Normals", &mDrawNormals );
	ImGui::End();
	if( changed ) {
		makeGeom();
	}
#endif
	// Rotate the cube by 2 degrees around an arbitrary axis
	mRotation *= rotate( toRadians( 0.8f ), normalize( vec3( sin( getElapsedSeconds() * 0.5 ), 1, -0.3 ) ) );
}

void ExtrudeApp::draw()
{
	gl::clear();

	gl::setMatrices( mCam );
	gl::pushMatrices();
		gl::multModelMatrix( mRotation );
		gl::color( Color( 1, 1, 1 ) );
#if defined( CINDER_GL_ES )
		mBatch->draw();
#else
		if( mDrawWireframe )
			gl::enableWireframe();
		mBatch->draw();
		gl::disableWireframe();
#endif
		gl::color( Color( 1.0f, 0.5f, 0.25f ) );
		if( mDrawNormals && mNormalsBatch )
			mNormalsBatch->draw();
		if( mUseSpline && mSplineBatch ) {
			gl::disableDepthRead();
			mSplineBatch->draw();
			gl::enableDepthRead();
		}
	gl::popMatrices();
}

CINDER_APP( ExtrudeApp, RendererGl )
