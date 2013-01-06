#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/params/Params.h"

using namespace ci;
using namespace ci::app;

class TweakBarApp : public AppBasic {
 public:
	void setup();
	void resize();
	void draw();
	void button();
	
	CameraPersp				mCam;
	params::InterfaceGl		mParams;
	float					mObjSize;
	Quatf					mObjOrientation;
	Vec3f					mLightDirection;
	ColorA					mColor;
	std::string				mString;
};

void TweakBarApp::setup()
{
	mObjSize = 4;
	mLightDirection = Vec3f( 0, 0, -1 );
	mColor = ColorA( 0.25f, 0.5f, 1.0f, 1.0f );

	// setup our default camera, looking down the z-axis
	mCam.lookAt( Vec3f( -20, 0, 0 ), Vec3f::zero() );

	// Setup the parameters
	mParams = params::InterfaceGl( "App parameters", Vec2i( 200, 400 ) );
	mParams.addParam( "Cube Size", &mObjSize, "min=0.1 max=20.5 step=0.5 keyIncr=z keyDecr=Z" );
	mParams.addParam( "Cube Rotation", &mObjOrientation );
	mParams.addParam( "Cube Color", &mColor, "" );	
	mParams.addSeparator();	
	mParams.addParam( "Light Direction", &mLightDirection, "" );
	mParams.addButton( "Button!", std::bind( &TweakBarApp::button, this ) );
	mParams.addText( "text", "label=`This is a label without a parameter.`" );
	mParams.addParam( "String ", &mString, "" );
}

void TweakBarApp::button()
{
	app::console() << "Clicked!" << std::endl;
	mParams.setOptions( "text", "label=`Clicked!`" );
}

void TweakBarApp::resize()
{
	mCam.setAspectRatio( getWindowAspectRatio() );
}

void TweakBarApp::draw()
{
	// this pair of lines is the standard way to clear the screen in OpenGL
	gl::enableDepthRead();
	gl::enableDepthWrite();
	gl::clear( Color( 0.1f, 0.1f, 0.1f ) );

	glLoadIdentity();
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );	
	GLfloat lightPosition[] = { -mLightDirection.x, -mLightDirection.y, -mLightDirection.z, 0.0f };
	glLightfv( GL_LIGHT0, GL_POSITION, lightPosition );
	glMaterialfv( GL_FRONT, GL_DIFFUSE,	mColor );

	gl::setMatrices( mCam );
	gl::rotate( mObjOrientation );
	gl::color( mColor );
	gl::drawCube( Vec3f::zero(), Vec3f( mObjSize, mObjSize, mObjSize ) );

	// Draw the interface
	params::InterfaceGl::draw();
}

CINDER_APP_BASIC( TweakBarApp, RendererGl )
