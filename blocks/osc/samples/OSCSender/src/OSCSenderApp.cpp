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

#include "cinder/app/AppBasic.h"
using namespace ci;
using namespace ci::app;

#include "OscSender.h"

// We'll create a new Cinder Application by deriving from the BasicApp class
class OSCSenderApp : public AppBasic {
 public:
	void prepareSettings(Settings *settings);
	void setup();
	void update();
	void draw();
	void mouseMove(MouseEvent event);
	void mouseDrag(MouseEvent event);
	
	int mMouseLocX;
	osc::Sender sender;
	float positionX;
	std::string host;
	int port;
};

void OSCSenderApp::prepareSettings(Settings *settings){
	settings->setFrameRate(60.0f);
	settings->setWindowSize(640, 480);
}

void OSCSenderApp::setup(){
	host = "localhost";
	port = 3000;
	sender.setup(host, port);
	
}

void OSCSenderApp::update(){
	float freq = mMouseLocX / (float)getWindowWidth() * 10.0f;
	positionX = cos(freq * getElapsedSeconds()) / 2.0f + .5f;
	
}

void OSCSenderApp::mouseMove(MouseEvent event){
	mMouseLocX = event.getX();
}

void OSCSenderApp::mouseDrag(MouseEvent event){
	mouseMove(event);
}

void OSCSenderApp::draw()
{
	gl::clear(Color(1,1,1), true);
	gl::color(Color(.5, .5, .5));
	gl::drawSolidRect(Rectf(Vec2f(0, 0), Vec2f(positionX * getWindowWidth(), getWindowHeight())));
	
	osc::Message message;
	message.addFloatArg(positionX);
	message.setAddress("/cinder/osc/1");
	message.setRemoteEndpoint(host, port);
	sender.sendMessage(message);
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( OSCSenderApp, RendererGl )
