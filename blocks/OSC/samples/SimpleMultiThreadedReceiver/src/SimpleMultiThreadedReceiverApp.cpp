#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Log.h"
#include "cinder/Timeline.h"
#include "Osc.h"

#define USE_UDP 1

using namespace ci;
using namespace ci::app;
using namespace std;

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
	
	std::mutex mCirclePosMutex, mSquarePosMutex;
	
#if USE_UDP
	osc::ReceiverUdp mReceiver;
#else
	osc::ReceiverTcp mReceiver;
#endif
};

SimpleMultiThreadedReceiverApp::SimpleMultiThreadedReceiverApp()
: mIoService( new asio::io_service ), mWork( new asio::io_service::work( *mIoService ) ),
	mReceiver( 10001 )
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
	
	mReceiver.bind();
	mReceiver.listen();
	
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
