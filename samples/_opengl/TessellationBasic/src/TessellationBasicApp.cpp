#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// Reference article: http://antongerdelan.net/opengl/tessellation.html

class TessellationShaderApp : public App {
  public:
	void setup() override;
	void draw() override;

  private:
	gl::GlslProgRef			mGlsl;
	float					mRadius;
	params::InterfaceGlRef	mParams;
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
	
	mParams = params::InterfaceGl::create( "Settings", ivec2( 200, 200 ) );
	mRadius = 200.0f;
	mParams->addParam( "Radius", &mRadius, "step=1.0" );
	mParams->addParam( "Tess level inner", &mTessLevelInner, "min=0" );
	mParams->addParam( "Tess level outer", &mTessLevelOuter, "min=0" );
		
	try {
		mGlsl = gl::GlslProg::create( gl::GlslProg::Format()
									 .vertex( loadAsset( "0_vert.glsl" ) )
									 .tessellationCtrl( loadAsset( "1_tess_ctrl.glsl" ) )
									 .tessellationEval( loadAsset( "2_tess_eval.glsl" ) )
									 .fragment( loadAsset( "3_frag.glsl" ) ) );
	}
	catch( const gl::GlslProgCompileExc &ex ) {
		cout << ex.what() << endl;
		quit();
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

void TessellationShaderApp::draw()
{
	gl::clear();
	gl::enableWireframe();
	
	gl::setMatricesWindow( getWindowSize() );
	gl::translate( getWindowCenter() );
	
	gl::ScopedGlslProg glslProg( mGlsl );
	mGlsl->uniform( "uTessLevelInner", (float)mTessLevelInner );
	mGlsl->uniform( "uTessLevelOuter", (float)mTessLevelOuter );
	mGlsl->uniform( "uRadius", mRadius );	
	
	mBatch->draw();
	
	gl::disableWireframe();
	mParams->draw();
}

CINDER_APP( TessellationShaderApp, RendererGl( RendererGl::Options().version( 4, 0 ) ) )
