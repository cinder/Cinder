#include "cinder/app/AppBasic.h"
#include "cinder/Serial.h"
#include "cinder/Text.h"
#include "cinder/gl/Texture.h"

#include <sstream>

using namespace ci;
using namespace ci::app;
using namespace std;

#define BUFSIZE 80
#define READ_INTERVAL 0.25


// We'll create a new Cinder Application by deriving from the BasicApp class
class SerialCommunicationApp : public AppBasic {
 public:
	// Cinder calls this function 30 times per second by default
	void draw();
	void update();
	void setup();
	void mouseDown(MouseEvent event);
	
	bool		bSendSerialMessage, bTextureComplete;			// a flag for sending serial
	Serial serial;
	uint8_t			ctr;
	std::string lastString;
	
	gl::Texture	mTexture;
	
	double sinceLastRead, lastUpdate;
};

void SerialCommunicationApp::setup()
{
	ctr = 0;
	lastString = "";
	sinceLastRead = 0.0;
	lastUpdate = 0.0;
	
	bSendSerialMessage = false;
	bTextureComplete = false;

	// print the devices
	const vector<Serial::Device> &devices( Serial::getDevices() );
	for( vector<Serial::Device>::const_iterator deviceIt = devices.begin(); deviceIt != devices.end(); ++deviceIt ) {
		console() << "Device: " << deviceIt->getName() << endl;
	}
	
	try {
		Serial::Device dev = Serial::findDeviceByNameContains("tty.usbserial");
		serial = Serial( dev, 9600);
	}
	catch( ... ) {
		console() << "There was an error initializing the serial device!" << std::endl;
		exit( -1 );
	}
	
	// wait for * as a sign for first contact
	char contact = 0;
	while(contact != '*')
	{
		contact = (char) serial.readByte();
	}
	
	// request actual data
	serial.writeByte(ctr);
	
	// clear accumulated contact messages in buffer
	char b = '*';
	while(serial.getNumBytesAvailable() > -1)
	{
		b = serial.readByte();
		console() << b << "_";
	}
		
	serial.flush();
}

void SerialCommunicationApp::update()
{
//	console() << "Bytes available: " << serial.getNumBytesAvailable() << std::endl;
	
	double now = getElapsedSeconds();
	double deltaTime = now - lastUpdate;
	lastUpdate = now;
	sinceLastRead += deltaTime;
	
	if(sinceLastRead > READ_INTERVAL)
	{
		bSendSerialMessage = true;
		sinceLastRead = 0.0;
	}
		
	
	if (bSendSerialMessage)
	{
		// request next chunk
		serial.writeByte(ctr);
		
		try{
			// read until newline, to a maximum of BUFSIZE bytes
			lastString = serial.readStringUntil('\n', BUFSIZE );

		} catch(SerialTimeoutExc e) {
			console() << "timeout" << endl;
		}
		
		
		bSendSerialMessage = false;
		
		ctr+=8;
		console() << lastString << endl;
		
		
		TextLayout simple;
		simple.setFont( Font( "Arial Black", 24 ) );
		simple.setColor( Color( .7, .7, .2 ) );
		simple.addLine( lastString );
		simple.setLeadingOffset( 0 );
		mTexture = gl::Texture( simple.render( true, false ) );
		bTextureComplete = true;
		
		serial.flush();
	}
}

void SerialCommunicationApp::mouseDown(MouseEvent event){
//	bSendSerialMessage = true;
}


void SerialCommunicationApp::draw()
{
	// this pair of lines is the standard way to clear the screen in OpenGL
	//printf("click to test serial:\nnBytes read %i\nnTimes read %i\nread: %s\n(at time %0.3f)", nBytesRead, nTimesRead, lastString, readTime);
	
	// this pair of lines is the standard way to clear the screen in OpenGL
	glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
	
	gl::enableAlphaBlending( false );
	
	if(bTextureComplete){
		glColor3f( 1.0f, 1.0f, 1.0f );
		gl::draw( mTexture, Vec2f( 10, 10 ) );
	}
	
	
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( SerialCommunicationApp, RendererGl )
