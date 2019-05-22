#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Query.h"
#include "cinder/Timer.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class QueryTestApp : public App {
public:
	static void prepareSettings( Settings * settings );
	void setup() override;
	void update() override;
	void draw() override;
private:
	gl::GlslProgRef			mNoiseShader;
	gl::QueryTimeSwappedRef	mQuery;
	
	gl::QueryRef			mQueryPrimitive;
	
	Timer					mCpuTimer;
};

void QueryTestApp::prepareSettings( Settings * settings )
{
	settings->setWindowSize( ivec2( 1500, 900 ) );
}

void QueryTestApp::setup()
{
	mNoiseShader = gl::GlslProg::create( loadAsset( "pass.vert" ), loadAsset( "noise.frag" ) );
	mQueryPrimitive = gl::Query::create( GL_PRIMITIVES_GENERATED );
	
	mQuery = gl::QueryTimeSwapped::create();
}
void QueryTestApp::update()
{
}

void QueryTestApp::draw()
{
	gl::clear();

	mCpuTimer.start();
	mQuery->begin();

	// Expensive CPU operation
	for( size_t i = 0; i < 75000; ++i ) {
		Rand::randVec3();
	}

	// Expensive GPU pass
	{
		gl::ScopedGlslProg s( mNoiseShader );
		mNoiseShader->uniform( "uTime", (float)app::getElapsedSeconds() );
		gl::drawSolidRect( app::getWindowBounds() );
	}

	mQuery->end();
	mCpuTimer.stop();
		
	mQueryPrimitive->begin();
	gl::drawSphere( vec3( 0 ), 2.0f, 4 );
	mQueryPrimitive->end();
	
	if( app::getElapsedFrames() % 20 == 1 ) {
		app::console() << "GPU time : " << mQuery->getElapsedSeconds() << std::endl;
		app::console() << "CPU time : " << mCpuTimer.getSeconds() << std::endl;
		app::console() << "Average fps :" << getAverageFps() << std::endl;
		
		mCpuTimer.start();
		app::console() << "Num primitives: " << mQueryPrimitive->getValueInt() << std::endl;
		mCpuTimer.stop();
		app::console() << "Primitive block call time: " << mCpuTimer.getSeconds() << std::endl;
	}
	
}

CINDER_APP( QueryTestApp, RendererGl, &QueryTestApp::prepareSettings )
