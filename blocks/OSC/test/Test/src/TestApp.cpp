#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"

#include "cinder/osc/Osc.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define TEST_UDP 1

#define USE_SLIP_PACKET_FRAMING 1

#if TEST_UDP
using Receiver = osc::ReceiverUdp;
using Sender = osc::SenderUdp;
using protocol = asio::ip::udp;
#else
using Receiver = osc::ReceiverTcp;
using Sender = osc::SenderTcp;
using protocol = asio::ip::tcp;
#endif

struct TestStruct {
	int myInt;
	float myFloat;
	double myDouble;
};

class TestApp : public App {
  public:
	TestApp();
	void update() override;
	void cleanup() override;
	
	void setupUdp();
	void setupTcp();
	
	void constructMessageA();
	void constructMessageB();
	
	osc::PacketFramingRef	mPacketFraming;
	Receiver				mReceiver;
	Sender					mSender;
	osc::Message			mMessage;
    /// For testing variadic templated functions
    osc::Message			mMessage2;
	
	bool mIsConnected = false, mShouldSendMessageA = false, mShouldSendMessageB = false;
};

TestApp::TestApp()
#if TEST_UDP
: App(), mReceiver( 10000 ), mSender( 12345, "127.0.0.1", 10000 )
#else
: App(),
#if USE_SLIP_PACKET_FRAMING
	mPacketFraming( new osc::SLIPPacketFraming ),
#endif
	mReceiver( 10000, protocol::v4(), io_service(), mPacketFraming ),
	mSender( 12345, "127.0.0.1", 10000, protocol::v4(), io_service(), mPacketFraming )
#endif
{
//	log::manager()->makeLogger<log::LoggerBreakpoint>();
	
#if TEST_UDP
	setupUdp();
#else
	setupTcp();
#endif
	
	mReceiver.setListener( "/app/?",
	[]( const osc::Message &message ){
		cout << "Integer: " << message[0].int32() << endl;
		cout << "Received From: " << message.getSenderIpAddress() << " Port: " << message.getSenderPort() << endl;
	} );
	mReceiver.setListener( "/app/*",
	[]( const osc::Message &message ) {
		auto blob0 = message[2].blob();
		TestStruct myStruct0( *reinterpret_cast<TestStruct*>( blob0.getData() ) );
		cout << "Test Struct 0: " <<  endl <<
		"\tint: " << myStruct0.myInt << endl <<
		"\tfloat: " << myStruct0.myFloat << endl <<
		"\tdouble: " << myStruct0.myDouble << endl;
		auto blob1 = message.getArgBlob( 3 );
		TestStruct myStruct1( *reinterpret_cast<TestStruct*>( blob1.getData() ) );
		cout << "Test Struct 1: " <<  endl <<
		"\tint: " << myStruct1.myInt << endl <<
		"\tfloat: " << myStruct1.myFloat << endl <<
		"\tdouble: " << myStruct1.myDouble << endl;
	});
	mReceiver.setListener( "/app/10",
	[&]( const osc::Message &message ) {
		cout << "Address: " << message.getAddress() << endl;
		cout << "Integer: " << message[0].int32() << endl;
		cout << "String: " << message[1].string() << endl;
		auto blob0 = message[2].blob();
		TestStruct myStruct0( *reinterpret_cast<TestStruct*>( blob0.getData() ) );
		cout << "Test Struct 0: " <<  endl <<
		"\tint: " << myStruct0.myInt << endl <<
		"\tfloat: " << myStruct0.myFloat << endl <<
		"\tdouble: " << myStruct0.myDouble << endl;
		auto blob1 = message.getArgBlob( 3 );
		TestStruct myStruct1( *reinterpret_cast<TestStruct*>( blob1.getData() ) );
		cout << "Test Struct 1: " <<  endl <<
		"\tint: " << myStruct1.myInt << endl <<
		"\tfloat: " << myStruct1.myFloat << endl <<
		"\tdouble: " << myStruct1.myDouble << endl;
		cout << "Character: " << message[4].character() << endl;
		osc::ByteArray<4> midi;
		message[5].midi( &midi[0], &midi[1], &midi[2], &midi[3] );
		cout << "Midi: " <<  (int)midi[0] << " " <<  (int)midi[1] << " " <<  (int)midi[2] << " " <<  (int)midi[3] << endl;
		cout << "Int64: " << message[6].int64() << endl;
		cout << "Float: " << message[7].flt() << endl;
		cout << "Double: " << message[8].dbl() << endl;
		cout << "Boolean: " << message[9].boolean() << endl;
		auto messagesTheSame = (mMessage == message);
		cout << "Messages are the same: " << std::boolalpha << messagesTheSame << endl;
	});
    mReceiver.setListener("/message2",
    [&]( const osc::Message& message ) {
        cout << message << endl;
        auto messagesTheSame = (mMessage2 == message);
        cout << "Message2 the same: " << std::boolalpha << messagesTheSame << endl;
    });
	gl::clear();
	
	constructMessageA();
	constructMessageB();
}

void TestApp::setupUdp()
{
#if TEST_UDP
	try{
		mReceiver.bind();
	}
	catch( const osc::Exception &ex ) {
		CI_LOG_EXCEPTION("Receiver Bind: ", ex);
		quit();
	}
	
	mReceiver.listen(
	[]( const asio::error_code &error, const asio::ip::udp::endpoint &originator ) -> bool {
		// Decide whether we want to keep listening, is this an originator error or our socket's
		// error. Return false to make the socket stop listenings
		if( error ) {
			CI_LOG_E("Error: " << error.message() << ", val: " << error.value()
					<< ", originator: " << originator.address().to_string() );
			return false;
		}
		else
			return true;
	});
	
	try{
		mSender.bind();
		mIsConnected = true;
		mShouldSendMessageA = true;
		mShouldSendMessageB = true;
	}
	catch( const osc::Exception &ex ) {
		CI_LOG_EXCEPTION("Sender bind: ", ex);
		quit();
	}
#endif
}

void TestApp::setupTcp()
{
#if ! TEST_UDP
	try{
		mReceiver.bind();
	}
	catch( const osc::Exception &ex ) {
		CI_LOG_EXCEPTION("Receiver Bind: ", ex);
		quit();
	}
	
	auto socketTransportErrorFn = [&]( asio::error_code error, uint64_t identifier ) {
		if( error ) {
			CI_LOG_E( "Acceptor Error: " << error.message()
					 << " - Val: " << error.value()
					 << " -- endpoint: " << identifier );
		}
	};
	
	mReceiver.setSocketTransportErrorFn( socketTransportErrorFn );
	
	auto onAcceptFn = [&]( osc::TcpSocketRef socket, uint64_t identifier ) -> bool {
		// Decide whether this connection is correct and return whether
		// the socket should be cached and read from
		bool shouldAccept = true;
		if( shouldAccept )
			return true;
		else
			return false;
	};
	
	auto onAcceptErrorFn = [&]( asio::error_code error, osc::ReceiverTcp::protocol::endpoint endpoint ) -> bool {
		// if it's decided that the error can't be recovered from, stop
		// accepting by returning false. otherwise do the fix and return
		// true. possible fix could be closing and rebinding the acceptor
		if( error ) {
			CI_LOG_E( "Acceptor Error: " << error.message()
					 << " - Val: " << error.value()
					 << " -- endpoint: " << endpoint );
			return false;
		}
		else
			return true;
	};
	
	mReceiver.accept( onAcceptErrorFn, onAcceptFn );
	
	try{
		mSender.bind();
	}
	catch( const osc::Exception &ex ) {
		CI_LOG_EXCEPTION("Sender bind: ", ex);
		quit();
	}
	
	mSender.connect(
	[&]( asio::error_code error ){
		if( ! error ) {
			mIsConnected = true;
			mShouldSendMessageA = true;
			mShouldSendMessageB = true;
		}
		else
			CI_LOG_E( "error: " << error.message() << ", val: " << error.value() );
	});
#endif
}

void TestApp::constructMessageA()
{
	static int i = 245;
	i++;
	static std::string test("testing");
	test += ".";
	static TestStruct mTransmitStruct{ 0, 0, 0 };
	mTransmitStruct.myInt += 45;
	mTransmitStruct.myFloat += 32.4f;
	mTransmitStruct.myDouble += 128.09;
	
	osc::Message message( "/app/10" );
	message.append( i );
	message.append( test );
	
	auto buffer = ci::Buffer::create( &mTransmitStruct, sizeof(TestStruct) );
	message.append( *buffer );
	mTransmitStruct.myInt += 33;
	mTransmitStruct.myFloat += 45.4f;
	mTransmitStruct.myDouble += 1.01;
	message.appendBlob( &mTransmitStruct, sizeof(TestStruct) );
	
	message.append( 'X' );
	message.appendMidi( 10, 20, 30, 40 );
	message.append( int64_t( 1000000000000 ) );
	message.append( 1.4f );
	message.append( 28.4 );
	message.append( true );
	
	auto onErrorFn = [&]( asio::error_code error ) {
		if( error ) {
			CI_LOG_E( "error: " << error.message() << ", val: " << error.value() );
			quit();
		}
		else
			constructMessageA();
	};
	
	auto onCompleteFn = [&]() {
		mShouldSendMessageA = true;
	};
	
	mSender.send( message, onErrorFn, onCompleteFn );
	mMessage = std::move( message );
	mShouldSendMessageA = false;
}

void TestApp::constructMessageB()
{
	auto onErrorFn = [&]( asio::error_code error ) {
		if( error ) {
			CI_LOG_E( "error: " << error.message() << ", val: " << error.value() );
			quit();
		}
	};
	
	auto onCompleteFn = [&]() {
		mShouldSendMessageB = true;
	};
	const char* something = "Something";
	mMessage2 = osc::Message( "/message2" ) << 3 << 4 << 2.0 << 3.0f << "string literal" << something;
	mSender.send( mMessage2, onErrorFn, onCompleteFn );
	mShouldSendMessageB = false;
}

void TestApp::update()
{
	if( mIsConnected ) {
		if( mShouldSendMessageA )
			constructMessageA();
		if( mShouldSendMessageB )
			constructMessageB();
	}
	gl::clear();
}

void TestApp::cleanup()
{
#if ! TEST_UDP
	mSender.shutdown();
#endif
	mReceiver.close();
	mSender.close();
}

#if defined( CINDER_MSW )
CINDER_APP(TestApp, RendererGl, [](App::Settings *settings) { settings->setConsoleWindowEnabled(); })
#else
CINDER_APP(TestApp, RendererGl);
#endif
