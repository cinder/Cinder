#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class GeomSourceModsApp : public App {
  public:
	void setup() override;
	void update() override;
	void mouseDown( MouseEvent ) override;
	void draw() override;
	
	size_t								mVisualTestIndex;
	vector<function<gl::BatchRef()>>	mVisualTestSetups;
	
	CameraPersp				mCam;
	gl::BatchRef			mBatch;
	mat4					mRotation;
};

geom::Cube makeCube()
{
	return geom::Cube().size( 1, 2, 3 );
}

geom::Cube& makeCubeRef()
{
	static geom::Cube sCube = geom::Cube().size( 1, 2, 3 );
	return sCube;
}

geom::Cube* makeCubePtr()
{
	static geom::Cube sCube = geom::Cube().size( 1, 2, 3 );
	return &sCube;
}

geom::SourceMods makeChain()
{
	return geom::Cube() >> geom::Translate( vec3( 1 ) );
}

// This demonstrates why cloning the Source is necessary even when it's an l-value.
// In this example, 'cube' is an l-value, but it will have gone out of scope when the SourceMods is returned.
geom::SourceMods makeChain2()
{
	geom::Cube cube;
	return cube >> geom::Translate( vec3( 1 ) );
}

void test( const geom::SourceMods &sourceMods )
{
	if( sourceMods.getSource() == sourceMods.getSourceStorage().get() )
		std::cout << "Cloned ";
	
	TriMesh tm( sourceMods );
	std::cout << tm.calcBoundingBox().getMax().x - tm.calcBoundingBox().getExtents().x;
	std::cout << std::endl;
}

gl::BatchRef visualTest1()
{
	std::cout << "non-indexed LINE_STRIP + non-indexed LINE_STRIP" << std::endl;
	return gl::Batch::create( geom::WireCircle().center( vec2( -1 ) ).subdivisions( 21 )
		& geom::WireCircle().center( vec2( 1 ) ).subdivisions( 3 ), gl::getStockShader( gl::ShaderDef().color() ) );
}

gl::BatchRef visualTest2()
{
	std::cout << "non-indexed LINES + non-indexed LINE_STRIP" << std::endl;
	return gl::Batch::create( geom::WireCube()
		& geom::WireCircle().center( vec2( 1 ) ).subdivisions( 3 ), gl::getStockShader( gl::ShaderDef().color() ) );
}

gl::BatchRef visualTest3()
{
	std::cout << "indexed LINES + non-indexed LINE_STRIP" << std::endl;
	return gl::Batch::create( geom::Sphere() >> geom::Lines()
		& geom::WireCircle().center( vec2( 1 ) ).subdivisions( 3 ), gl::getStockShader( gl::ShaderDef().color() ) );
}

gl::BatchRef visualTest4()
{
	std::cout << "indexed LINES + indexed LINES + indexed LINES" << std::endl;
	auto sphere1 = geom::Sphere().center( vec3( -1 ) ) >> geom::Lines();
	auto sphere2 = geom::Sphere().center( vec3( 1 ) ) >> geom::Lines();
	auto sphere3 = sphere1 >> geom::Translate( 0, 1, 0 );
	return gl::Batch::create( sphere1 & sphere2 & sphere3, gl::getStockShader( gl::ShaderDef().color() ) );
}

gl::BatchRef visualTest5()
{
	std::cout << "non-indexed TRIANGLE_STRIP + non-indexed TRIANGLE_FAN" << std::endl;
	return gl::Batch::create( geom::Rect()
		& geom::Circle().center( vec2( 1 ) ).subdivisions( 3 ), gl::getStockShader( gl::ShaderDef().color() ) );
}

gl::BatchRef visualTest6()
{
	std::cout << "indexed TRIANGLES + indexed TRIANGLES + indexed TRIANGLES" << std::endl;
	auto sphere1 = geom::Sphere().center( vec3( -1 ) );
	auto sphere2 = geom::Sphere().center( vec3( 1 ) );
	auto sphere3 = sphere1 >> geom::Translate( 0, 1, 0 );
	return gl::Batch::create( sphere1 & sphere2 & sphere3, gl::getStockShader( gl::ShaderDef().lambert() ) );
}

gl::BatchRef visualTest7()
{
	std::cout << "indexed TRIANGLES + indexed TRIANGLES + indexed TRIANGLES" << std::endl;
	auto cube1 = geom::Cube() >> geom::Constant( geom::COLOR, vec3( 0.22, 0.33f, 0.77f ) );
	auto cube2 = geom::Cube() >> geom::Translate( 0, 1.5f, 0 ) >> geom::Constant( geom::COLOR, vec4( 1, 0.5f, 0.25f, 0.5f ) );
	return gl::Batch::create( (cube1 >> geom::Translate( 0, 0, 3 ) & cube2) >> geom::Translate( 0, 1.5, 0 ), gl::getStockShader( gl::ShaderDef().lambert().color() ) );
}

gl::BatchRef visualTest8()
{
	geom::Circle circle2( geom::Circle().center( vec2( 1 ) ) );
	geom::SourceMods result = geom::Circle().center( vec2( 0 ) ).subdivisions( 50 ) >> geom::Constant( geom::COLOR, vec3( Color( CM_HSV, randFloat(), 1, 1 ) ) );
	
	for( float a = 0; a < 1.0f; a += 0.05f ) {
		vec3 color = vec3( Color( CM_HSV, randFloat(), 1, 1 ) );
		result.append(
			geom::Circle().center( vec2( cos(a * M_PI*2), sin(a * M_PI*2) ) * 2.5f ).radius( 0.3f ).subdivisions( int( 3 + a * 20 ) )
			>> geom::Constant( geom::COLOR, color ) );
	}
	
	return gl::Batch::create( result, gl::getStockShader( gl::ShaderDef().color() ) );
}

// Optimiziation to pass &-of instead of value
gl::BatchRef visualTest9()
{
	TriMesh tm = geom::Sphere();
	
	return gl::Batch::create( &tm & &tm >> geom::Translate( 0, 1, 0 ), gl::getStockShader( gl::ShaderDef().lambert().color() ) );
}

// &= version of 8
gl::BatchRef visualTest10()
{
	geom::SourceMods combination;
	
	for( float a = 0; a < 1.0f; a += 0.025f ) {
		vec3 color = vec3( Color( CM_HSV, a, 1, 1 ) );
		combination &= geom::Circle().center( vec2( cos(a * M_PI*2), sin(a * M_PI*2) ) * 2.5f ).radius( 0.17f ).subdivisions( 7 )
						>> geom::Constant( geom::COLOR, color );
	}
	
	return gl::Batch::create( combination, gl::getStockShader( gl::ShaderDef().color() ) );
}

void GeomSourceModsApp::setup()
{
	// temporary geom::Source; should create a clone
	test( geom::Cube() >> geom::Translate( vec3( 1 ) ) ); // 1
	test( geom::Cube() >> geom::Translate( vec3( 1 ) ) >> geom::Translate( vec3( 2 ) ) ); // 3

	// l-value geom::Source; should create a clone. See note on makeChain2() for why should clone
	geom::Cube cube;
	test( cube >> geom::Translate( vec3( 1 ) ) ); // 1
	test( cube >> geom::Translate( vec3( 1 ) ) >> geom::Translate( vec3( 2 ) ) ); // 3

	// l-value geom::Source	& l-value geom::Modifier; should still create a clone
	geom::Translate translate1( vec3( 1 ) ), translate2( vec3( 2 ) ), translate3( vec3( 3 ) );
	test( cube >> translate1 ); // 1
	test( cube >> translate1 >> translate1 ); // 2

	// pointer geom::Source	& geom::Modifier; should NOT create a clone
	test( &cube >> translate1 ); // 1
	test( &cube >> translate1 >> translate1 ); // 2

	// this should create a clone since the result of makeCube() is a temporary
	test( makeCube() >> translate1 ); // 1
	// this should create a clone since the result of makeCube() is a reference
	test( makeCubeRef() >> translate1 ); // 1
	// this should not create a clone since the result of makeCubeRef() is a pointer
	test( makeCubePtr() >> translate1 ); // 1
	
	auto chain0 = makeCube() >> translate1; // 1
	// this should create a clone
	test( chain0 >> translate1 >> translate2 ); // 1+1+2=4
	test( chain0 >> translate1 >> translate2 >> translate3 ); // 1+1+2+3=7
	
	test( makeChain() ); // 1
	test( makeChain() >> translate1 ); // 2

	test( makeChain2() ); // 1
	test( makeChain2() >> translate1 ); // 2
	
	mCam.lookAt( vec3( 3, 2, 4 ), vec3( 0 ) );
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
	gl::enableDepthWrite();
	gl::enableDepthRead();

	// triangles visual tests
	mVisualTestSetups.push_back( visualTest10 );
	mVisualTestSetups.push_back( visualTest9 );
	mVisualTestSetups.push_back( visualTest8 );
	mVisualTestSetups.push_back( visualTest7 );
	mVisualTestSetups.push_back( visualTest6 );
	mVisualTestSetups.push_back( visualTest5 );

	// lines visual tests
	mVisualTestSetups.push_back( visualTest1 );
	mVisualTestSetups.push_back( visualTest2 );
	mVisualTestSetups.push_back( visualTest3 );
	mVisualTestSetups.push_back( visualTest4 );

	mVisualTestIndex = 0;
	mBatch = mVisualTestSetups[mVisualTestIndex]();
}

void GeomSourceModsApp::update()
{
	// Rotate the cube by 0.2 degrees around the y-axis
	mRotation *= rotate( toRadians( 0.2f ), normalize( vec3( 0, 1, 0 ) ) );
}

void GeomSourceModsApp::mouseDown( MouseEvent )
{
	mVisualTestIndex = ( mVisualTestIndex + 1 ) % mVisualTestSetups.size();
	mBatch = mVisualTestSetups[mVisualTestIndex]();
}

void GeomSourceModsApp::draw()
{
	gl::clear();

	gl::setMatrices( mCam );

	gl::ScopedModelMatrix modelScope;
	gl::multModelMatrix( mRotation );
	if( mBatch )
		mBatch->draw();
}

CINDER_APP( GeomSourceModsApp, RendererGl )
