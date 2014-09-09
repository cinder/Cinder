
#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/Shader.h"
#include "cinder/params/Params.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// Reference article: http://antongerdelan.net/opengl/tessellation.html

class TessellationShaderApp : public AppNative {
  public:
	void setup() override;
	void draw() override;
  private:
	gl::GlslProgRef			mGlsl;
	float					mRadius;
	params::InterfaceGlRef	mParams;
	
	int mTessLevelInner;
	int mTessLevelOuter;
};

void TessellationShaderApp::setup()
{
	mTessLevelInner = mTessLevelOuter = 4;
	int max_patch_vertices = 0;
	glGetIntegerv( GL_MAX_PATCH_VERTICES, &max_patch_vertices );
	app::console() << "Max supported patch vertices " << max_patch_vertices << std::endl;
	
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
	catch( gl::GlslProgCompileExc ex ) {
		cout << ex.what() << endl;
		shutdown();
	}
	
	glPatchParameteri( GL_PATCH_VERTICES, 3 );
}

void TessellationShaderApp::draw()
{
	gl::clear();
	gl::enableWireframe();
	
	gl::setMatricesWindow( getWindowSize() );
	gl::translate( getWindowCenter() );
	
	gl::ScopedGlslProg glslProg( mGlsl );
	mGlsl->uniform( "tessLevelInner", (float) mTessLevelInner );
	mGlsl->uniform( "tessLevelOuter", (float) mTessLevelOuter );
	
	auto triangle = gl::VertBatch::create( GL_PATCHES );
	triangle->color( 1.0f, 0.0f, 0.0f );
	triangle->vertex( vec2( mRadius, -mRadius ) );
	triangle->color( 0.0f, 1.0f, 0.0f );
	triangle->vertex( vec2( 0 , mRadius ) );
	triangle->color( 0.0f, 0.0f, 1.0f );
	triangle->vertex( vec2( -mRadius, -mRadius ) );
	triangle->draw();
	
	gl::disableWireframe();
	mParams->draw();
}

CINDER_APP_NATIVE( TessellationShaderApp, RendererGl( RendererGl::Options().version( 4, 0 ) ) )
