#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include <thread>

#include "Osc.h"

#define USE_UDP 1

using namespace ci;
using namespace ci::app;
using namespace std;

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
	
	ivec2 mCurrentMousePositon;
	
	std::shared_ptr<asio::io_service>		mIoService;
	std::shared_ptr<asio::io_service::work>	mWork;
	std::thread								mThread;
	
#if USE_UDP
	osc::SenderUdp mSender;
#else
	osc::SenderTcp mSender;
#endif
};

SimpleMultiThreadedSenderApp::SimpleMultiThreadedSenderApp()
: mIoService( new asio::io_service ), mWork( new asio::io_service::work( *mIoService ) ),
	mSender( 10000, destinationHost, destinationPort )
{
}

void SimpleMultiThreadedSenderApp::setup()
{
	mSender.bind();
#if ! USE_UDP
	mSender.connect();
#endif
	mThread = std::thread( std::bind(
	[]( std::shared_ptr<asio::io_service> &service ){
		service->run();
	}, mIoService ));
}

void SimpleMultiThreadedSenderApp::mouseMove( cinder::app::MouseEvent event )
{
	mCurrentMousePositon = event.getPos();
	osc::Message msg( "/mousemove/1" );
	msg.append( mCurrentMousePositon.x );
	msg.append( mCurrentMousePositon.y );
	
	mSender.send( msg );
}

void SimpleMultiThreadedSenderApp::mouseDown( MouseEvent event )
{
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
