/*
 Copyright (c) 2010, Hector Sanchez-Pajares
 Aer Studio http://www.aerstudio.com
 All rights reserved.
 
 
 This is a block for MIDI Integration for Cinder framework developed by The Barbarian Group, 2010
 
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

#include "cinder/app/AppBasic.h"
#include <list>
using namespace ci;
using namespace ci::app;
using namespace std;

#include "MidiIn.h"
#include "MidiMessage.h"
#include "MidiConstants.h"

#define SLIDER_NOTE 1


class MidiExampleApp : public AppBasic {
 public:
	void prepareSettings(Settings* settings);
	void setup();
	void update();
	void draw();
	
	void processMidiMessage(midi::Message* message);
	
	midi::Input mMidiIn;
	
	float sliderValue;
};

void MidiExampleApp::prepareSettings(Settings* settings){
	settings->setFrameRate(50.0f);
	settings->setWindowSize(640, 480);
}

void MidiExampleApp::setup(){
	
	if (mMidiIn.getNumPorts() > 0){
		mMidiIn.listPorts();
		mMidiIn.openPort(0);
		console() << "Opening MIDI port 0" << std::endl;
	}else {
		console() << "No MIDI Ports found!!!!" << std::endl;
	}
}

void MidiExampleApp::update(){
	while (mMidiIn.hasWaitingMessages()) {
		midi::Message message;
		mMidiIn.getNextMessage(&message);
		
		processMidiMessage(&message);
		
	}
}

void MidiExampleApp::draw(){
	gl::clear(Color(0,0,0), true);
	gl::color(Color(1, 1, 1));
	gl::drawSolidRect(Rectf(Vec2f(0, 0), Vec2f(sliderValue * getWindowWidth(), getWindowHeight())));
}

void MidiExampleApp::processMidiMessage(midi::Message* message){
	console() << "midi port: " << message->port << " ch: " << message->channel << " status: " << message->status;
	console() << " byteOne: " << message->byteOne << " byteTwo: " << message->byteTwo << std::endl;
	
	switch (message->status) {
		case MIDI_CONTROL_CHANGE:
			if (message->byteOne == SLIDER_NOTE){
				sliderValue = message->byteTwo / 127.0f;
			}
			break;
	}
}



// This line tells Cinder to actually create the application
CINDER_APP_BASIC( MidiExampleApp, RendererGl )
