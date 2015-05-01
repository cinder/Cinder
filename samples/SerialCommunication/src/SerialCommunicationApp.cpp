// Demonstrates using the ci::Serial class

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Serial.h"
#include "cinder/Text.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"

#include <sstream>

using namespace ci;
using namespace ci::app;
using namespace std;

#define BUFSIZE 80
#define READ_INTERVAL 0.25

class SerialCommunicationApp : public App {
 public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

	bool		mSendSerialMessage;
	SerialRef	mSerial;
	uint8_t		mCounter;
	string		mLastString;
	
	gl::TextureRef	mTexture;
	
	double mLastRead, mLastUpdate;
};

void SerialCommunicationApp::setup()
{
	mCounter = 0;
	mLastRead = 0;
	mLastUpdate = 0;
	mSendSerialMessage = false;

	// print the devices
	for( const auto &dev : Serial::getDevices() )
		console() << "Device: " << dev.getName() << endl;

	try {
		Serial::Device dev = Serial::findDeviceByNameContains( "tty.usbserial" );
		mSerial = Serial::create( dev, 9600 );
	}
	catch( SerialExc &exc ) {
		CI_LOG_EXCEPTION( "coult not initialize the serial device", exc );
		exit( -1 );
	}
	
	// wait for * as a sign for first contact
	char contact = 0;
	while( contact != '*' )	{
		contact = (char)mSerial->readByte();
	}
	
	// request actual data
	mSerial->writeByte( mCounter );
	
	// clear accumulated contact messages in buffer
	char b = '*';
	while( mSerial->getNumBytesAvailable() > -1 ) {
		b = mSerial->readByte();
		console() << b << "_";
	}
		
	mSerial->flush();
}

void SerialCommunicationApp::mouseDown( MouseEvent event )
{
//	mSendSerialMessage = true;
}

void SerialCommunicationApp::update()
{
//	console() << "Bytes available: " << mSerial->getNumBytesAvailable() << std::endl;
	
	double now = getElapsedSeconds();
	double deltaTime = now - mLastUpdate;
	mLastUpdate = now;
	mLastRead += deltaTime;
	
	if( mLastRead > READ_INTERVAL )	{
		mSendSerialMessage = true;
		mLastRead = 0.0;
	}

	if( mSendSerialMessage ) {
		// request next chunk
		mSerial->writeByte( mCounter );
		
		try{
			// read until newline, to a maximum of BUFSIZE bytes
			mLastString = mSerial->readStringUntil( '\n', BUFSIZE );

		}
		catch( SerialTimeoutExc &exc ) {
			CI_LOG_EXCEPTION( "timeout", exc );
		}
		
		mSendSerialMessage = false;
		mCounter += 8;

		console() << "last string: " << mLastString << endl;
		
		TextLayout simple;
		simple.setFont( Font( "Arial Black", 24 ) );
		simple.setColor( Color( .7, .7, .2 ) );
		simple.addLine( mLastString );
		simple.setLeadingOffset( 0 );
		mTexture = gl::Texture::create( simple.render( true, false ) );

		mSerial->flush();
	}
}

void SerialCommunicationApp::draw()
{
	gl::clear();
	
	if( mTexture )
		gl::draw( mTexture, vec2( 10, 10 ) );
}

CINDER_APP( SerialCommunicationApp, RendererGl )
