/*
 Copyright (c) 2010, Hector Sanchez-Pajares
 Aer Studio http://www.aerstudio.com
 All rights reserved.
 
 
 This is a block for OSC Integration for Cinder framework developed by The Barbarian Group, 2010
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this list of conditions and
 the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
 the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 */

/*
 Based on addon ofxOsc for openFrameworks
 by Damian Stewart
 http://addons.openframeworks.cc/projects/show/ofxosc
 http://frey.co.nz/old/projects/ofxosc/
 
 */

#include "cinder/app/AppNative.h"
#include "cinder/System.h"
#include "OscSender.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class OSCSenderApp : public AppNative {
  public:
	void setup();
	void update();
	void draw();
	void mouseMove( MouseEvent event );
	void mouseDrag( MouseEvent event );
	
	int 		mMouseLocX;
	osc::Sender sender;
	float 		positionX;
	std::string host;
	int 		port;
};

void OSCSenderApp::setup()
{
	mMouseLocX = getWindowCenter().x;
	port = 3000;
	// assume the broadcast address is this machine's IP address but with 255 as the final value
	// so to multicast from IP 192.168.1.100, the host should be 192.168.1.255
	host = System::getIpAddress();
	if( host.rfind( '.' ) != string::npos )
		host.replace( host.rfind( '.' ) + 1, 3, "255" );
	sender.setup( host, port, true );
}

void OSCSenderApp::update()
{
	float freq = mMouseLocX / (float)getWindowWidth() * 10.0f;
	positionX = cos(freq * getElapsedSeconds()) / 2.0f + .5f;
	
	osc::Message message;
	message.setAddress("/cinder/osc/1");
	message.addFloatArg(positionX);
	sender.sendMessage(message);
}

void OSCSenderApp::mouseMove( MouseEvent event )
{
	mMouseLocX = event.getX();
}

void OSCSenderApp::mouseDrag( MouseEvent event )
{
	mouseMove(event);
}

void OSCSenderApp::draw()
{
	gl::clear();
	gl::color( Color::gray( 0.5f ) );
	gl::drawSolidRect(Rectf(Vec2f(0, 0), Vec2f(positionX * getWindowWidth(), getWindowHeight())));
}

CINDER_APP_NATIVE( OSCSenderApp, RendererGl )