#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "Osc.h"

#define USE_UDP 1

using namespace ci;
using namespace ci::app;
using namespace std;

const std::string destinationHost = "127.0.0.1";
const uint16_t destinationPort = 10001;

class SimpleSenderApp : public App {
  public:
	SimpleSenderApp();
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void mouseUp( MouseEvent event ) override;
	void mouseMove( MouseEvent event ) override;
	void draw() override;
	
	ivec2 mCurrentMousePositon;
	
#if USE_UDP
	osc::SenderUdp mSender;
#else
	osc::SenderTcp mSender;
#endif
};

SimpleSenderApp::SimpleSenderApp()
: mSender( 10000, destinationHost, destinationPort )
{
}

void SimpleSenderApp::setup()
{
	mSender.bind();
#if ! USE_UDP
	mSender.connect();
#endif
}

void SimpleSenderApp::mouseMove( cinder::app::MouseEvent event )
{
	mCurrentMousePositon = event.getPos();
	osc::Message msg( "/mousemove/1" );
	msg.append( mCurrentMousePositon.x );
	msg.append( mCurrentMousePositon.y );
	
	mSender.send( msg );
}

void SimpleSenderApp::mouseDown( MouseEvent event )
{
	osc::Message msg( "/mouseclick/1" );
	msg.append( (float)event.getPos().x / getWindowWidth() );
	msg.append( (float)event.getPos().y / getWindowHeight() );
	
	mSender.send( msg );
}

void SimpleSenderApp::mouseDrag( MouseEvent event )
{
	mouseMove( event );
}

void SimpleSenderApp::mouseUp( MouseEvent event )
{
	osc::Message msg( "/mouseclick/1" );
	msg.append( (float)event.getPos().x / getWindowWidth() );
	msg.append( (float)event.getPos().y / getWindowHeight() );
	
	mSender.send( msg );
}

void SimpleSenderApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatricesWindow( getWindowSize() );
	gl::drawSolidCircle( mCurrentMousePositon, 100 );
}

auto settingsFunc = []( App::Settings *settings ) {
#if defined( CINDER_MSW )
	settings->setConsoleWindowEnabled();
#endif
	settings->setMultiTouchEnabled( false );
};

CINDER_APP( SimpleSenderApp, RendererGl, settingsFunc )
