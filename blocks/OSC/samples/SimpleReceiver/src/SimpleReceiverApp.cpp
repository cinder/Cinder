#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/Timeline.h"
#include "cinder/CinderImGui.h"
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

class SimpleReceiverApp : public App {
  public:
	SimpleReceiverApp();
	void setup() override;
	void cleanup() override;
	void draw() override;

	void startListening();
	void stopListening();

	ivec2 mCurrentCirclePos;
	vec2  mCurrentSquarePos;

	std::unique_ptr<Receiver>			   mReceiver;
	std::map<uint64_t, protocol::endpoint> mConnections;

	// ImGui UI state
	int			mLocalPort;
	bool		mIsListening;
	int			mMouseMoveCount;
	int			mMouseClickCount;
	std::string mLastError;
	std::string mLastMessageAddress;
};

SimpleReceiverApp::SimpleReceiverApp()
	: mLocalPort( 10001 )
	, mIsListening( false )
	, mMouseMoveCount( 0 )
	, mMouseClickCount( 0 )
{
}

void SimpleReceiverApp::setup()
{
	// Initialize ImGui
	ImGui::Initialize();

	// Start listening automatically
	startListening();
}

void SimpleReceiverApp::cleanup()
{
	stopListening();
}

void SimpleReceiverApp::startListening()
{
	try {
		mReceiver.reset( new Receiver( mLocalPort ) );

		mReceiver->setListener( "/mousemove/1", [&]( const osc::Message& msg ) {
			mCurrentCirclePos.x = msg[0].int32();
			mCurrentCirclePos.y = msg[1].int32();
			mMouseMoveCount++;
			mLastMessageAddress = msg.getAddress();
		} );

		mReceiver->setListener( "/mouseclick/1", [&]( const osc::Message& msg ) {
			mCurrentSquarePos = vec2( msg[0].flt(), msg[1].flt() ) * vec2( getWindowSize() );
			mMouseClickCount++;
			mLastMessageAddress = msg.getAddress();
		} );

		mReceiver->bind();

#if USE_UDP
		mReceiver->listen( [&]( asio::error_code error, protocol::endpoint endpoint ) -> bool {
			if( error ) {
				mLastError = "Error Listening: " + error.message();
				CI_LOG_E( mLastError << " val: " << error.value() << " endpoint: " << endpoint );
				return false;
			}
			else
				return true;
		} );
#else
		mReceiver->setConnectionErrorFn( [&]( asio::error_code error, uint64_t identifier ) {
			if( error ) {
				auto foundIt = mConnections.find( identifier );
				if( foundIt != mConnections.end() ) {
					if( error == asio::error::eof ) {
						CI_LOG_W( "Other side closed the connection: " << error.message() );
					}
					else {
						mLastError = "Error Reading from Socket: " + error.message();
						CI_LOG_E( mLastError );
					}
					mConnections.erase( foundIt );
				}
			}
		} );

		auto expectedOriginator = protocol::endpoint( asio::ip::make_address( "127.0.0.1" ), 10000 );
		mReceiver->accept(
			[&]( asio::error_code error, protocol::endpoint endpoint ) -> bool {
				if( error ) {
					mLastError = "Error Accepting: " + error.message();
					CI_LOG_E( mLastError );
					return false;
				}
				else
					return true;
			},
			[&, expectedOriginator]( osc::TcpSocketRef socket, uint64_t identifier ) -> bool {
				mConnections.emplace( identifier, socket->remote_endpoint() );
				return socket->remote_endpoint() == expectedOriginator;
			} );
#endif

		mIsListening = true;
		mLastError.clear();
		CI_LOG_V( "Started listening on port " << mLocalPort );
	}
	catch( const osc::Exception& ex ) {
		mLastError = "Error binding: " + std::string( ex.what() );
		CI_LOG_E( mLastError << " val: " << ex.value() );
		mIsListening = false;
	}
}

void SimpleReceiverApp::stopListening()
{
	if( mReceiver ) {
		try {
			mReceiver->close();
		}
		catch( const osc::Exception& ex ) {
			CI_LOG_EXCEPTION( "Error closing receiver", ex );
		}
		mReceiver.reset();
	}
	mIsListening = false;
}

void SimpleReceiverApp::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.15f ) );
	gl::setMatricesWindow( getWindowSize() );

	// Draw visuals based on received messages
	gl::color( mIsListening ? ColorA( 0.0f, 1.0f, 0.0f, 0.8f ) : ColorA( 1.0f, 0.0f, 0.0f, 0.8f ) );
	gl::drawStrokedCircle( mCurrentCirclePos, 100 );
	gl::drawSolidRect( Rectf( mCurrentSquarePos - vec2( 50 ), mCurrentSquarePos + vec2( 50 ) ) );

	// ImGui UI
	ImGui::Begin( "OSC Receiver Settings" );

	ImGui::Text( "Connection Settings" );
	ImGui::Separator();

	// Port configuration
	ImGui::BeginDisabled( mIsListening );
	ImGui::InputInt( "Local Port", &mLocalPort );
	ImGui::EndDisabled();

	// Start/Stop button
	if( mIsListening ) {
		if( ImGui::Button( "Stop Listening" ) ) {
			stopListening();
		}
		ImGui::SameLine();
		ImGui::TextColored( ImVec4( 0.0f, 1.0f, 0.0f, 1.0f ), "Listening" );
	}
	else {
		if( ImGui::Button( "Start Listening" ) ) {
			startListening();
		}
		ImGui::SameLine();
		ImGui::TextColored( ImVec4( 1.0f, 0.0f, 0.0f, 1.0f ), "Stopped" );
	}

	ImGui::Spacing();
	ImGui::Text( "Statistics" );
	ImGui::Separator();
	ImGui::Text( "Mouse Move Messages: %d", mMouseMoveCount );
	ImGui::Text( "Mouse Click Messages: %d", mMouseClickCount );
	ImGui::Text( "Total Messages: %d", mMouseMoveCount + mMouseClickCount );

	if( ! mLastMessageAddress.empty() ) {
		ImGui::Spacing();
		ImGui::Text( "Last Message:" );
		ImGui::Text( "  Address: %s", mLastMessageAddress.c_str() );
	}

	if( ! mLastError.empty() ) {
		ImGui::Spacing();
		ImGui::TextColored( ImVec4( 1.0f, 0.0f, 0.0f, 1.0f ), "Last Error:" );
		ImGui::TextWrapped( "%s", mLastError.c_str() );
	}

	ImGui::Spacing();
	ImGui::Text( "Instructions:" );
	ImGui::TextWrapped( "Run SimpleSender to see received messages" );
	ImGui::TextWrapped( "Circle follows /mousemove/1 messages" );
	ImGui::TextWrapped( "Square jumps to /mouseclick/1 messages" );

	ImGui::End();
}

auto settingsFunc = []( App::Settings* settings ) {
#if defined( CINDER_MSW )
	settings->setConsoleWindowEnabled();
#endif
	settings->setMultiTouchEnabled( false );
};

CINDER_APP( SimpleReceiverApp, RendererGl, settingsFunc )
