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
using protocol = asio::ip::udp;
#else
using Receiver = osc::ReceiverTcp;
using protocol = asio::ip::tcp;
#endif

class SimpleMultiThreadedReceiverApp : public App {
public:
	SimpleMultiThreadedReceiverApp();
	void setup() override;
	void draw() override;
	void cleanup() override;
	
	std::shared_ptr<asio::io_service>		mIoService;
	std::shared_ptr<asio::io_service::work>	mWork;
	std::thread								mThread;
	
	ivec2	mCurrentCirclePos;
	vec2	mCurrentSquarePos;
	bool	mUpdatedTarget;
	
	std::mutex mCirclePosMutex, mSquarePosMutex, mConnectionsMutex;
	
	Receiver mReceiver;
	std::map<uint64_t, protocol::endpoint> mConnections;
};

SimpleMultiThreadedReceiverApp::SimpleMultiThreadedReceiverApp()
: mIoService( new asio::io_service ), mWork( new asio::io_service::work( *mIoService ) ),
    mReceiver( 10001, protocol::v4(), *mIoService )
{
}

void SimpleMultiThreadedReceiverApp::setup()
{
	mReceiver.setListener( "/mousemove/1",
	[&]( const osc::Message &msg ){
		std::lock_guard<std::mutex> lock( mCirclePosMutex );
		mCurrentCirclePos.x = msg[0].int32();
		mCurrentCirclePos.y = msg[1].int32();
	});
	mReceiver.setListener( "/mouseclick/1",
	[&]( const osc::Message &msg ){
		std::lock_guard<std::mutex> lock( mSquarePosMutex );
		mCurrentSquarePos = vec2( msg[0].flt(), msg[1].flt() ) * vec2( getWindowSize() );
	});
	
	// For a description of the below setup, take a look at SimpleReceiver. The only difference
	// is the usage of the mutex around the connection map.
	try {
		mReceiver.bind();
	}
	catch( const osc::Exception &ex ) {
		CI_LOG_E( "Error binding: " << ex.what() << " val: " << ex.value() );
		quit();
	}
	
#if USE_UDP
	mReceiver.listen(
	[]( asio::error_code error, protocol::endpoint endpoint ) -> bool {
		if( error ) {
			CI_LOG_E( "Error Listening: " << error.message() << " val: "
					 << error.value() << " endpoint: " << endpoint );
			return false;
		}
		else
			return true;
	});
#else
	mReceiver.setConnectionErrorFn(
	[&]( asio::error_code error, uint64_t identifier ) {
		if ( error ) {
			std::lock_guard<std::mutex> lock( mConnectionsMutex );
			auto foundIt = mConnections.find( identifier );
			if( foundIt != mConnections.end() ) {
				CI_LOG_E( "Error Reading from Socket: " << error.message()
						 << " val: " << error.value() << " endpoint: "
						 << foundIt->second.address().to_string() );
				mConnections.erase( foundIt );
			}
		}
	});
	auto expectedOriginator = protocol::endpoint( asio::ip::address::from_string( "127.0.0.1" ), 10000 );
	mReceiver.accept(
	[]( asio::error_code error, protocol::endpoint endpoint ) -> bool {
		if( error ) {
			CI_LOG_E( "Error Accepting: " << error.message()
					 << " val: " << error.value() << " endpoint: "
					 << endpoint.address().to_string() );
			return false;
		}
		else
			return true;
	},
	[&, expectedOriginator]( osc::TcpSocketRef socket, uint64_t identifier ) -> bool {
		std::lock_guard<std::mutex> lock( mConnectionsMutex );
		mConnections.emplace( identifier, socket->remote_endpoint() );
		return socket->remote_endpoint() == expectedOriginator;
	} );
#endif
	
	// Now that everything is setup, run the io_service on the other thread.
	mThread = std::thread( std::bind(
	[]( std::shared_ptr<asio::io_service> &service ){
		service->run();
	}, mIoService ));
}

void SimpleMultiThreadedReceiverApp::draw()
{
	gl::clear( GL_COLOR_BUFFER_BIT );
	gl::setMatricesWindow( getWindowSize() );
	
	{
		std::lock_guard<std::mutex> lock( mCirclePosMutex );
		gl::drawStrokedCircle( mCurrentCirclePos, 100 );
	}
	
	{
		std::lock_guard<std::mutex> lock( mSquarePosMutex );
		gl::drawSolidRect( Rectf( mCurrentSquarePos - vec2( 50 ), mCurrentSquarePos + vec2( 50 ) ) );
	}
}

void SimpleMultiThreadedReceiverApp::cleanup()
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

CINDER_APP( SimpleMultiThreadedReceiverApp, RendererGl, settingsFunc )
