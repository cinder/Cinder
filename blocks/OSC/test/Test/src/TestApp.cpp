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
	
	osc::PacketFramingRef	mPacketFraming;
	Receiver				mReceiver;
	Sender					mSender;
	osc::Message			mMessage;
    /// For testing variadic templated functions
    osc::Message			mMessage2;
	
	bool mIsConnected = false;
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
	log::manager()->makeLogger<log::LoggerBreakpoint>();
	mReceiver.bind();
	mReceiver.listen();
	mReceiver.setListener( "/app/?",
	[]( const osc::Message &message ){
		cout << "Integer: " << message[0].int32() << endl;
		cout << "Received From: " << message.getSenderIpAddress() << endl;
	} );
	mReceiver.setListener( "/app/?",
	[]( const osc::Message &message ) {
		cout << "String: " << message[1].string() << endl;
	});
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
	mReceiver.setListener( "/app/?",
	[]( const osc::Message &message ) {
		cout << "Character: " << message[4].character() << endl;
	});
	mReceiver.setListener( "/app/?",
	[]( const osc::Message &message ) {
		cout << "Midi: " << message << endl;
	});
	mReceiver.setListener( "/app/?",
	[]( const osc::Message &message ) {
		cout << "Int64: " << message[6].int64() << endl;
	});
	mReceiver.setListener( "/app/?",
	[]( const osc::Message &message ) {
		cout << "Float: " << message[7].flt() << endl;
	});
	mReceiver.setListener( "/app/?",
	[]( const osc::Message &message ) {
		cout << "Double: " << message[8].dbl() << endl;
	});
	mReceiver.setListener( "/app/?",
	[]( const osc::Message &message ) {
		cout << "Boolean: " << message[9].boolean() << endl;
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
}

void TestApp::update()
{
	if( ! mIsConnected ) {
		mSender.bind();
#if ! TEST_UDP
		mSender.connect();
#endif
		mIsConnected = true;
	}
	else {
		
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
		
		mSender.send( message );
		mMessage = std::move( message );
		
        {
			const char* something = "Something";
			mMessage2 = osc::Message( "/message2" ) << 3 << 4 << 2.0 << 3.0f << "string literal" << something;
            mSender.send(mMessage2);
        }
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
