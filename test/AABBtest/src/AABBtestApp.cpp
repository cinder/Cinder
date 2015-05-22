#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class AABBtestApp : public App {
public:
	void setup() override;
	void update() override;
	void draw() override;

	void mouseMove( MouseEvent event ) override;

	AxisAlignedBox3f mBox;
	gl::BatchRef     mCube;
	CameraPersp      mCam;
	CameraUi         mCamUi;
	ci::ivec2        mMousePos;
};

void AABBtestApp::setup()
{
	// Create box and wireframe cube.
	mBox.set( vec3( -3, -2, -1 ), vec3( 3, 2, 1 ) );
	mCube = gl::Batch::create( geom::WireCube().size( 1, 1, 1 ), gl::getStockShader( gl::ShaderDef().color() ) );

	// Setup the camera.
	mCam.setPerspective( 50.0f, getWindowAspectRatio(), 0.01f, 100.0f );
	mCam.lookAt( vec3( 0, 5, 10 ), vec3( 0 ) );

	mCamUi.setCamera( &mCam );
	mCamUi.connect( getWindow() );
}

void AABBtestApp::update()
{
}

void AABBtestApp::draw()
{
	gl::clear();

	// Animate box.
	float t = (float) getElapsedSeconds() * 0.2f;
	mat4 transform = glm::translate( vec3( 7.0f * glm::sin( t ), 0, 0 ) );
	transform *= glm::rotate( t, glm::normalize( vec3( 1, 2, 3 ) ) );

	// Start drawing.
	gl::pushMatrices();
	gl::setMatrices( mCam );

	gl::drawCoordinateFrame();

	// Draw the animated box.
	gl::color( 1, 1, 1 );
	gl::setModelMatrix( transform );
	gl::translate( mBox.getCenter() );
	gl::scale( mBox.getSize() );

	mCube->draw();

	// Calculate and draw axis-aligned bounding box.
	auto aabb = mBox.transformed( transform );

	gl::color( 0, 1, 1 );
	gl::setModelMatrix( mat4() );
	gl::translate( aabb.getCenter() );
	gl::scale( aabb.getSize() );

	mCube->draw();

	// Draw intersection with a red line.
	vec3 src = vec3( 5 );
	vec3 dir = vec3( -1 );

	gl::setModelMatrix( mat4() );

	float min, max;
	Ray ray( src, dir );
	if( aabb.intersect( ray, &min, &max ) > 0 ) {
		gl::color( 1, 0, 0 );
		gl::drawLine( src, ray.calcPosition( min ) );
		gl::drawLine( src + 10.0f * dir, ray.calcPosition( max ) );
		gl::color( 1, 1, 0 );
		gl::drawSphere( ray.calcPosition( min ), 0.1f );
		gl::drawSphere( ray.calcPosition( max ), 0.1f );
		gl::drawLine( ray.calcPosition( min ), ray.calcPosition( max ) );
	}
	else {
		gl::color( 1, 0, 0 );
		gl::drawLine( src, src + 10.0f * dir );
	}

	// Done drawing.
	gl::popMatrices();
}

void AABBtestApp::mouseMove( MouseEvent event )
{
	mMousePos = event.getPos();
}

CINDER_APP( AABBtestApp, RendererGl )
