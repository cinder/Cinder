#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "Osc.h"

#define USE_UDP 1

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace asio;
using namespace asio::ip;

const uint16_t destinationPort = 10001;

class BroadcastSenderApp : public App {
public:
	BroadcastSenderApp();
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void mouseUp( MouseEvent event ) override;
	void mouseMove( MouseEvent event ) override;
	void draw() override;
	
	ivec2 mCurrentMousePositon;

	osc::UdpSocketRef mSocket;
	osc::SenderUdp mSender;
};

BroadcastSenderApp::BroadcastSenderApp()
: mSocket( new udp::socket( App::get()->io_service(), udp::endpoint( udp::v4(), 10000 ) ) ),
	mSender( mSocket, udp::endpoint( address_v4::broadcast(), destinationPort ) )
{
	mSocket->set_option( asio::socket_base::broadcast(true) );
}

void BroadcastSenderApp::setup()
{
	// No need to bind as the socket constructor opens and binds the socket
}

void BroadcastSenderApp::mouseMove( cinder::app::MouseEvent event )
{
	mCurrentMousePositon = event.getPos();
	osc::Message msg( "/mousemove/1" );
	msg.append( mCurrentMousePositon.x );
	msg.append( mCurrentMousePositon.y );
	
	mSender.send( msg );
}

void BroadcastSenderApp::mouseDown( MouseEvent event )
{
	osc::Message msg( "/mouseclick/1" );
	msg.append( (float)event.getPos().x / getWindowWidth() );
	msg.append( (float)event.getPos().y / getWindowHeight() );
	
	mSender.send( msg );
}

void BroadcastSenderApp::mouseDrag( MouseEvent event )
{
	mouseMove( event );
}

void BroadcastSenderApp::mouseUp( MouseEvent event )
{
	osc::Message msg( "/dev/1" );
	msg.append( (float)event.getPos().x / getWindowWidth() );
	msg.append( (float)event.getPos().y / getWindowHeight() );
	
	mSender.send( msg );
}

void BroadcastSenderApp::draw()
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

CINDER_APP( BroadcastSenderApp, RendererGl, settingsFunc )
