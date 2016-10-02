#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Camera.h"
#include "cinder/params/Params.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

void prepareSettings( App::Settings *settings )
{
	settings->setHighDensityDisplayEnabled();
}

class TweakBarApp : public App {
  public:
	void setup();
	void resize();
	void update();
	void draw();
	void button();

  private:
	CameraPersp				mCam;
	params::InterfaceGlRef	mParams;
	float					mObjSize;
	quat					mObjOrientation;
	ColorA					mColor;
	string					mString;
	bool					mPrintFps;
	
	void					setLightDirection( vec3 direction );
	vec3					getLightDirection() { return mLightDirection; }
	vec3					mLightDirection;
	uint32_t				mSomeValue;
	float					mA, mB, mC, mD;

	vector<string>			mEnumNames;
	int						mEnumSelection;
};

void TweakBarApp::setLightDirection( vec3 direction )
{
	console() << "Light direction: " << direction << endl;
	mLightDirection = direction;
}

void TweakBarApp::setup()
{
	mObjSize = 4;
	mLightDirection = vec3( 0, 0, -1 );
	mColor = ColorA( 0.25f, 0.5f, 1, 1 );
	mSomeValue = 2;
	mA = mB = mC = mD = 0;
	mPrintFps = false;

	// Setup our default camera, looking down the z-axis.
	mCam.lookAt( vec3( -20, 0, 0 ), vec3( 0 ) );

	// Create the interface and give it a name.
	mParams = params::InterfaceGl::create( getWindow(), "App parameters", toPixels( ivec2( 200, 400 ) ) );

	// Setup some basic parameters.
	mParams->addParam( "Cube Size", &mObjSize ).min( 0.1f ).max( 20.5f ).keyIncr( "z" ).keyDecr( "Z" ).precision( 2 ).step( 0.02f );
	mParams->addParam( "Cube Rotation", &mObjOrientation );
	mParams->addParam( "Cube Color", &mColor );
	mParams->addParam( "String", &mString );
	mParams->addParam( "print fps", &mPrintFps ).keyIncr( "p" );

	mParams->addSeparator();

	// Attach a callback that is fired after a target is updated.
	mParams->addParam( "some value", &mSomeValue ).updateFn( [this] { console() << "new value: " << mSomeValue << endl; } );

	// Add a param with no target, but instead provide setter and getter functions.
	function<void( vec3 )> setter	= bind( &TweakBarApp::setLightDirection, this, placeholders::_1 );
	function<vec3 ()> getter		= bind( &TweakBarApp::getLightDirection, this );
	mParams->addParam( "Light Direction", setter, getter );

	// Other types of controls that can be added to the interface.
	mParams->addButton( "Button!", bind( &TweakBarApp::button, this ) );
	mParams->addText( "text", "label=`This is a label without a parameter.`" );

	mParams->addSeparator();

	// A parameter name must be unique, but you can override it with a 'label', which does not have to be unique.
	mParams->addParam( "float1", &mA ).group( "Group 1" ).label( "Item X" );
	mParams->addParam( "float2", &mB ).group( "Group 1" ).label( "Item Y" );
	mParams->addParam( "float3", &mC ).group( "Group 2" ).label( "Item X" );
	mParams->addParam( "float4", &mD ).group( "Group 2" ).label( "Item Y" );

	mParams->addSeparator();

	// Add an enum (list) selector.
	mEnumSelection = 0;
	mEnumNames = { "apple", "banana", "orange" };

	mParams->addParam( "an enum", mEnumNames, &mEnumSelection )
		.keyDecr( "[" )
		.keyIncr( "]" )
		.updateFn( [this] { console() << "enum updated: " << mEnumNames[mEnumSelection] << endl; } );
}

void TweakBarApp::button()
{
	console() << "Clicked!" << endl;
	mParams->setOptions( "text", "label=`Clicked!`" );
}

void TweakBarApp::resize()
{
	mCam.setAspectRatio( getWindowAspectRatio() );
}

void TweakBarApp::update()
{
	if( mPrintFps && getElapsedFrames() % 60 == 0 )
		console() << getAverageFps() << endl;
}

void TweakBarApp::draw()
{
	// this pair of lines is the standard way to clear the screen in OpenGL
	gl::enableDepthRead();
	gl::enableDepthWrite();
	gl::clear( Color::gray( 0.1f ) );

	gl::setMatrices( mCam );
	gl::rotate( mObjOrientation );
	gl::color( mColor );
	gl::drawCube( vec3( 0 ), vec3( mObjSize ) );

	// Draw the interface
	mParams->draw();
}

CINDER_APP( TweakBarApp, RendererGl, prepareSettings )
