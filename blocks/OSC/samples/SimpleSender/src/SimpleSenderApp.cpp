#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"

#include "cinder/osc/Osc.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define USE_UDP 1

#if USE_UDP
using Sender = osc::SenderUdp;
#else
using Sender = osc::SenderTcp;
#endif

const std::string destinationHost = "127.0.0.1";
const uint16_t destinationPort = 10001;
const uint16_t localPort = 10000;

class SimpleSenderApp : public App {
  public:
	SimpleSenderApp();
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void mouseUp( MouseEvent event ) override;
	void mouseMove( MouseEvent event ) override;
	void draw() override;
	
	void onSendError( asio::error_code error );
	
	ivec2	mCurrentMousePositon;
	
	Sender	mSender;
	bool	mIsConnected;
};

SimpleSenderApp::SimpleSenderApp()
: mSender( localPort, destinationHost, destinationPort ), mIsConnected( false )
{
}

void SimpleSenderApp::setup()
{
	try {
		// Bind the sender to the endpoint. This function may throw. The exception will
		// contain asio::error_code information.
		mSender.bind();
	}
	catch ( const osc::Exception &ex ) {
		CI_LOG_E( "Error binding: " << ex.what() << " val: " << ex.value() );
		quit();
	}
	
#if ! USE_UDP
	mSender.connect(
	// Set up the OnConnectFn. If there's no error, you can consider yourself connected to
	// the endpoint supplied.
	[&]( asio::error_code error ){
		if( error ) {
			CI_LOG_E( "Error connecting: " << error.message() << " val: " << error.value() );
			quit();
		}
		else {
			CI_LOG_V( "Connected" );
			mIsConnected = true;
		}
	});
#else
	// Udp doesn't "connect" the same way Tcp does. If bind doesn't throw, we can
	// consider ourselves connected.
	mIsConnected = true;
#endif
}

// Unified error handler. Easiest to have a bound function in this situation,
// since we're sending from many different places.
void SimpleSenderApp::onSendError( asio::error_code error )
{
	if( error ) {
		CI_LOG_E( "Error sending: " << error.message() << " val: " << error.value() );
		// If you determine that this error is fatal, make sure to flip mIsConnected. It's
		// possible that the error isn't fatal.
		mIsConnected = false;
		try {
#if ! USE_UDP
			// If this is Tcp, it's recommended that you shutdown before closing. This
			// function could throw. The exception will contain asio::error_code
			// information.
			mSender.shutdown();
#endif
			// Close the socket on exit. This function could throw. The exception will
			// contain asio::error_code information.
			mSender.close();
		}
		catch( const osc::Exception &ex ) {
			CI_LOG_EXCEPTION( "Cleaning up socket: val -" << ex.value(), ex );
		}
		quit();
	}
}

void SimpleSenderApp::mouseMove( cinder::app::MouseEvent event )
{
	mCurrentMousePositon = event.getPos();
	// Make sure you're connected before trying to send.
	if( ! mIsConnected )
		return;
	
	osc::Message msg( "/mousemove/1" );
	msg.append( mCurrentMousePositon.x );
	msg.append( mCurrentMousePositon.y );
	// Send the msg and also provide an error handler. If the message is important you
	// could store it in the error callback to dispatch it again if there was a problem.
	mSender.send( msg, std::bind( &SimpleSenderApp::onSendError,
								  this, std::placeholders::_1 ) );
}

void SimpleSenderApp::mouseDown( MouseEvent event )
{
	// Make sure you're connected before trying to send.
	if( ! mIsConnected )
		return;
	
	osc::Message msg( "/mouseclick/1" );
	msg.append( (float)event.getPos().x / getWindowWidth() );
	msg.append( (float)event.getPos().y / getWindowHeight() );
	// Send the msg and also provide an error handler. If the message is important you
	// could store it in the error callback to dispatch it again if there was a problem.
	mSender.send( msg, std::bind( &SimpleSenderApp::onSendError,
								  this, std::placeholders::_1 ) );
}

void SimpleSenderApp::mouseDrag( MouseEvent event )
{
	mouseMove( event );
}

void SimpleSenderApp::mouseUp( MouseEvent event )
{
	// Make sure you're connected before trying to send.
	if( ! mIsConnected )
		return;
	
	osc::Message msg( "/mouseclick/1" );
	msg.append( (float)event.getPos().x / getWindowWidth() );
	msg.append( (float)event.getPos().y / getWindowHeight() );
	// Send the msg and also provide an error handler. If the message is important you
	// could store it in the error callback to dispatch it again if there was a problem.
	mSender.send( msg, std::bind( &SimpleSenderApp::onSendError,
								  this, std::placeholders::_1 ) );
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
