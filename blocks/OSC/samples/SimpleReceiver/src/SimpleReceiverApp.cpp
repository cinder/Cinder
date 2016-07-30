#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Log.h"
#include "cinder/Timeline.h"
#include "cinder/osc/Osc.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define USE_UDP 1

#if USE_UDP
using Receiver = osc::ReceiverUdp;
#else
using Receiver = osc::ReceiverTcp;
#endif

class SimpleReceiverApp : public App {
  public:
	SimpleReceiverApp();
	void setup() override;
	void draw() override;
	
	ivec2	mCurrentCirclePos;
	vec2	mCurrentSquarePos;
	bool	mMouseDown = false;
	
	Receiver mReceiver;
};

SimpleReceiverApp::SimpleReceiverApp()
: mReceiver( 10001 )
{
}

void SimpleReceiverApp::setup()
{
	mReceiver.setListener( "/mousemove/1",
	[&]( const osc::Message &msg ){
		mCurrentCirclePos.x = msg[0].int32();
		mCurrentCirclePos.y = msg[1].int32();
	});
	mReceiver.setListener( "/mouseclick/1",
	[&]( const osc::Message &msg ){
		mCurrentSquarePos = vec2( msg[0].flt(), msg[1].flt() ) * vec2( getWindowSize() );
	});
	mReceiver.bind();
	mReceiver.listen();
}


void SimpleReceiverApp::draw()
{
	gl::clear( GL_COLOR_BUFFER_BIT );
	gl::setMatricesWindow( getWindowSize() );
	
	gl::drawStrokedCircle( mCurrentCirclePos, 100 );
	gl::drawSolidRect( Rectf( mCurrentSquarePos - vec2( 50 ), mCurrentSquarePos + vec2( 50 ) ) );
}

auto settingsFunc = []( App::Settings *settings ) {
#if defined( CINDER_MSW )
	settings->setConsoleWindowEnabled();
#endif
	settings->setMultiTouchEnabled( false );
};

CINDER_APP( SimpleReceiverApp, RendererGl, settingsFunc )
