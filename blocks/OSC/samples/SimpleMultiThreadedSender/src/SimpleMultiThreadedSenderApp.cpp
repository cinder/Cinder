#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"

#include <thread>

#include "cinder/osc/Osc.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define USE_UDP 1

#if USE_UDP
using Sender = osc::SenderUdp;
using protocol = asio::ip::udp;
#else
using Sender = osc::SenderTcp;
using protocol = asio::ip::tcp;
#endif


const std::string destinationHost = "127.0.0.1";
const uint16_t destinationPort = 10001;

class SimpleMultiThreadedSenderApp : public App {
public:
	SimpleMultiThreadedSenderApp();
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void mouseUp( MouseEvent event ) override;
	void mouseMove( MouseEvent event ) override;
	void draw() override;
	void cleanup() override;
	
	void onSendError( asio::error_code error );
	
	ivec2 mCurrentMousePositon;
	
	std::shared_ptr<asio::io_service>		mIoService;
	std::shared_ptr<asio::io_service::work>	mWork;
	std::thread								mThread;
	
	Sender				mSender;
	std::atomic_bool	mIsConnected;
};

SimpleMultiThreadedSenderApp::SimpleMultiThreadedSenderApp()
: mIoService( new asio::io_service ), mWork( new asio::io_service::work( *mIoService ) ),
	mSender( 10000, destinationHost, destinationPort, protocol::v4(), *mIoService ),
	mIsConnected( false )
{
}

void SimpleMultiThreadedSenderApp::setup()
{
	// For a description of the below setup, take a look at SimpleReceiver. The only difference
	// is the usage of the atomic_bool around the connection map.
	try {
		mSender.bind();
	}
	catch ( const osc::Exception &ex ) {
		CI_LOG_E( "Error binding: " << ex.what() << " val: " << ex.value() );
		quit();
	}
	
#if ! USE_UDP
	mSender.connect(
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
	mIsConnected = true;
#endif

	mThread = std::thread( std::bind(
	[]( std::shared_ptr<asio::io_service> &service ){
		service->run();
	}, mIoService ));
}

void SimpleMultiThreadedSenderApp::onSendError( asio::error_code error )
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

void SimpleMultiThreadedSenderApp::mouseMove( cinder::app::MouseEvent event )
{
	mCurrentMousePositon = event.getPos();
	if( ! mIsConnected )
		return;
	
	osc::Message msg( "/mousemove/1" );
	msg.append( mCurrentMousePositon.x );
	msg.append( mCurrentMousePositon.y );
	
	mSender.send( msg );
}

void SimpleMultiThreadedSenderApp::mouseDown( MouseEvent event )
{
	if( ! mIsConnected )
		return;
	
	osc::Message msg( "/mouseclick/1" );
	msg.append( (float)event.getPos().x / getWindowWidth() );
	msg.append( (float)event.getPos().y / getWindowHeight() );
	
	mSender.send( msg );
}

void SimpleMultiThreadedSenderApp::mouseDrag( MouseEvent event )
{
	mouseMove( event );
}

void SimpleMultiThreadedSenderApp::mouseUp( MouseEvent event )
{
	if( ! mIsConnected )
		return;
	
	osc::Message msg( "/mouseclick/1" );
	msg.append( (float)event.getPos().x / getWindowWidth() );
	msg.append( (float)event.getPos().y / getWindowHeight() );
	
	mSender.send( msg );
}

void SimpleMultiThreadedSenderApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatricesWindow( getWindowSize() );
	gl::drawSolidCircle( mCurrentMousePositon, 100 );
}

void SimpleMultiThreadedSenderApp::cleanup()
{
	mWork.reset();
	mIoService->stop();
	mThread.join();
}

auto settingsFunc = []( App::Settings *settings ) {
#if defined( CINDER_MSW )
	settings->setConsoleWindowEnabled();
#endif
	settings->setMultiTouchEnabled( false );
};

CINDER_APP( SimpleMultiThreadedSenderApp, RendererGl, settingsFunc )
