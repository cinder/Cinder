#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/CinderImGui.h"

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

class SimpleSenderApp : public App {
  public:
	SimpleSenderApp();
	void setup() override;
	void cleanup() override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void mouseUp( MouseEvent event ) override;
	void mouseMove( MouseEvent event ) override;
	void update() override;
	void draw() override;

	void connectSender();
	void disconnectSender();
	void onSendError( asio::error_code error );

	ivec2 mCurrentMousePositon;

	std::unique_ptr<Sender> mSender;
	bool					mIsConnected;

	// ImGui UI state
	char		mDestinationHost[256];
	int			mDestinationPort;
	int			mLocalPort;
	bool		mUseUdp;
	int			mMessagesSent;
	std::string mLastError;
};

SimpleSenderApp::SimpleSenderApp()
	: mIsConnected( false )
	, mDestinationPort( 10001 )
	, mLocalPort( 10000 )
	, mUseUdp( true )
	, mMessagesSent( 0 )
{
	strcpy( mDestinationHost, "127.0.0.1" );
}

void SimpleSenderApp::setup()
{
	// Initialize ImGui
	ImGui::Initialize();

	// Start with UDP by default
	connectSender();
}

void SimpleSenderApp::cleanup()
{
	disconnectSender();
}

void SimpleSenderApp::connectSender()
{
	try {
		// Create new sender with current settings
		mSender.reset( new Sender( mLocalPort, std::string( mDestinationHost ), mDestinationPort ) );

		// Bind the sender to the endpoint. This function may throw.
		mSender->bind();

#if ! USE_UDP
		if( ! mUseUdp ) {
			mSender->connect( [&]( asio::error_code error ) {
				if( error ) {
					mLastError = "Error connecting: " + error.message();
					CI_LOG_E( mLastError );
					mIsConnected = false;
				}
				else {
					CI_LOG_V( "Connected" );
					mIsConnected = true;
					mLastError.clear();
				}
			} );
		}
		else
#endif
		{
			// UDP doesn't "connect" the same way TCP does
			mIsConnected = true;
			mLastError.clear();
			CI_LOG_V( "UDP Sender bound and ready" );
		}
	}
	catch( const osc::Exception& ex ) {
		mLastError = "Error binding: " + std::string( ex.what() );
		CI_LOG_E( mLastError << " val: " << ex.value() );
		mIsConnected = false;
	}
}

void SimpleSenderApp::disconnectSender()
{
	if( mSender ) {
		try {
#if ! USE_UDP
			if( ! mUseUdp ) {
				mSender->shutdown();
			}
#endif
			mSender->close();
		}
		catch( const osc::Exception& ex ) {
			CI_LOG_EXCEPTION( "Cleaning up socket: val -" << ex.value(), ex );
		}
		mSender.reset();
	}
	mIsConnected = false;
}

void SimpleSenderApp::onSendError( asio::error_code error )
{
	if( error ) {
		mLastError = "Error sending: " + error.message();
		CI_LOG_E( mLastError << " val: " << error.value() );
		mIsConnected = false;
		disconnectSender();
	}
	else {
		mMessagesSent++;
	}
}

void SimpleSenderApp::mouseMove( cinder::app::MouseEvent event )
{
	mCurrentMousePositon = event.getPos();
	if( ! mIsConnected || ! mSender )
		return;

	osc::Message msg( "/mousemove/1" );
	msg.append( mCurrentMousePositon.x );
	msg.append( mCurrentMousePositon.y );
	mSender->send( msg, std::bind( &SimpleSenderApp::onSendError, this, std::placeholders::_1 ) );
}

void SimpleSenderApp::mouseDown( MouseEvent event )
{
	if( ! mIsConnected || ! mSender )
		return;

	osc::Message msg( "/mouseclick/1" );
	msg.append( (float)event.getPos().x / getWindowWidth() );
	msg.append( (float)event.getPos().y / getWindowHeight() );
	mSender->send( msg, std::bind( &SimpleSenderApp::onSendError, this, std::placeholders::_1 ) );
}

void SimpleSenderApp::mouseDrag( MouseEvent event )
{
	mouseMove( event );
}

void SimpleSenderApp::mouseUp( MouseEvent event )
{
	if( ! mIsConnected || ! mSender )
		return;

	osc::Message msg( "/mouseclick/1" );
	msg.append( (float)event.getPos().x / getWindowWidth() );
	msg.append( (float)event.getPos().y / getWindowHeight() );
	mSender->send( msg, std::bind( &SimpleSenderApp::onSendError, this, std::placeholders::_1 ) );
}

void SimpleSenderApp::update()
{
}

void SimpleSenderApp::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.15f ) );
	gl::setMatricesWindow( getWindowSize() );

	// Draw the circle at mouse position
	gl::color( mIsConnected ? ColorA( 0.0f, 1.0f, 0.0f, 0.8f ) : ColorA( 1.0f, 0.0f, 0.0f, 0.8f ) );
	gl::drawSolidCircle( mCurrentMousePositon, 50 );

	// ImGui UI
	ImGui::Begin( "OSC Sender Settings" );

	ImGui::Text( "Connection Settings" );
	ImGui::Separator();

	// Host and port configuration
	ImGui::InputText( "Destination Host", mDestinationHost, sizeof( mDestinationHost ) );
	ImGui::InputInt( "Destination Port", &mDestinationPort );
	ImGui::InputInt( "Local Port", &mLocalPort );

	// Connect/Disconnect button
	if( mIsConnected ) {
		if( ImGui::Button( "Disconnect" ) ) {
			disconnectSender();
		}
		ImGui::SameLine();
		ImGui::TextColored( ImVec4( 0.0f, 1.0f, 0.0f, 1.0f ), "Connected" );
	}
	else {
		if( ImGui::Button( "Connect" ) ) {
			connectSender();
		}
		ImGui::SameLine();
		ImGui::TextColored( ImVec4( 1.0f, 0.0f, 0.0f, 1.0f ), "Disconnected" );
	}

	ImGui::Spacing();
	ImGui::Text( "Statistics" );
	ImGui::Separator();
	ImGui::Text( "Messages Sent: %d", mMessagesSent );

	if( ! mLastError.empty() ) {
		ImGui::Spacing();
		ImGui::TextColored( ImVec4( 1.0f, 0.0f, 0.0f, 1.0f ), "Last Error:" );
		ImGui::TextWrapped( "%s", mLastError.c_str() );
	}

	ImGui::Spacing();
	ImGui::Text( "Instructions:" );
	ImGui::TextWrapped( "Move mouse to send /mousemove/1 messages" );
	ImGui::TextWrapped( "Click to send /mouseclick/1 messages" );

	ImGui::End();
}

auto settingsFunc = []( App::Settings* settings ) {
#if defined( CINDER_MSW )
	settings->setConsoleWindowEnabled();
#endif
	settings->setMultiTouchEnabled( false );
};

CINDER_APP( SimpleSenderApp, RendererGl, settingsFunc )
