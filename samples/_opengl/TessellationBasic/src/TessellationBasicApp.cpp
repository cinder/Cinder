#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#if ! defined( CINDER_GL_ES )
#include "cinder/CinderImGui.h"
#endif

using namespace ci;
using namespace ci::app;
using namespace std;

// Reference article: http://antongerdelan.net/opengl/tessellation.html
//
// Android Notes
//   - Requires OpenGL ES 3.2 or 3.1 and Android Extensions Pack
//   - Since gl::enableWireFrame doesn't work on OpenGL ES, geometry
//     shader is used to generate line stripes.
//

class TessellationShaderApp : public App {
  public:
	void setup() override;
#if defined( CINDER_GL_ES )
	void mouseDown( MouseEvent event ) override;
#endif
	void update() override;
	void draw() override;

  private:
	gl::GlslProgRef			mGlsl;
	float					mRadius;
	gl::VertBatchRef		mBatch;

	int mTessLevelInner;
	int mTessLevelOuter;
};

void TessellationShaderApp::setup()
{
	mTessLevelInner = mTessLevelOuter = 4;
	int maxPatchVertices = 0;
	glGetIntegerv( GL_MAX_PATCH_VERTICES, &maxPatchVertices );
	app::console() << "Max supported patch vertices " << maxPatchVertices << std::endl;

#if ! defined( CINDER_GL_ES )
	fs::path glDir = "ogl";
	mRadius = 200.0f;
	ImGui::Initialize();
#else
	fs::path glDir = "es31a";
	mRadius = 400.0f;
#endif
	
	try {
		mGlsl = gl::GlslProg::create( gl::GlslProg::Format()
									 .vertex( loadAsset( glDir / "0_vert.glsl" ) )
									 .tessellationCtrl( loadAsset( glDir / "1_tess_ctrl.glsl" ) )
									 .tessellationEval( loadAsset( glDir / "2_tess_eval.glsl" ) )
#if defined( CINDER_GL_ES )
									 .geometry( loadAsset( glDir / "x_geom.glsl" ) )
#endif
									 .fragment( loadAsset( glDir / "3_frag.glsl" ) ) );
	}
	catch( const std::exception &ex ) {
		console() << ex.what() << endl;
		//quit();
	}

	mBatch = gl::VertBatch::create( GL_PATCHES );
	mBatch->color( 1.0f, 0.0f, 0.0f );
	mBatch->vertex( vec2( 1, -1 ) );
	mBatch->color( 0.0f, 1.0f, 0.0f );
	mBatch->vertex( vec2( 0 , 1 ) );
	mBatch->color( 0.0f, 0.0f, 1.0f );
	mBatch->vertex( vec2( -1, -1 ) );
	
	gl::patchParameteri( GL_PATCH_VERTICES, 3 );
}

#if defined( CINDER_GL_ES )
void TessellationShaderApp::mouseDown( MouseEvent event )
{
	ivec2 pos = event.getPos();
	if( pos.y < (getWindowHeight()/2) ) {
		++mTessLevelInner;
		mTessLevelInner %= 16;
	}
	else {
		++mTessLevelOuter;
		mTessLevelOuter %= 16;
	}
}
#endif

void TessellationShaderApp::update()
{
#if ! defined( CINDER_GL_ES )
	ImGui::DragFloat( "Radius", &mRadius );
	ImGui::DragInt( "Tess level inner", &mTessLevelInner );
	ImGui::DragInt( "Tess level outer", &mTessLevelOuter );
#endif
}

void TessellationShaderApp::draw()
{
	gl::clear();
#if ! defined( CINDER_GL_ES )
	gl::enableWireframe();
#endif
	
	gl::setMatricesWindow( getWindowSize() );
	gl::translate( getWindowCenter() );
	
	gl::ScopedGlslProg glslProg( mGlsl );
	mGlsl->uniform( "uTessLevelInner", (float)mTessLevelInner );
	mGlsl->uniform( "uTessLevelOuter", (float)mTessLevelOuter );
	mGlsl->uniform( "uRadius", mRadius );	
	
	mBatch->draw();

#if ! defined( CINDER_GL_ES )
	gl::disableWireframe();
#endif
}

#if defined( CINDER_GL_ES )
	CINDER_APP( TessellationShaderApp, RendererGl(), []( TessellationShaderApp::Settings* settings ) {
		settings->setMultiTouchEnabled( false );
	})
#else
	CINDER_APP( TessellationShaderApp, RendererGl( RendererGl::Options().version( 4, 0 ) ) )
#endif
