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

/*
 *	midi::Hub.cpp
 *	By Roger Sodre
 *
 *	Connects to ALL present MIDI devices
 */

#include "cinder/app/AppBasic.h"
#include "MidiHub.h"

namespace cinder { namespace midi {
	
	// --------------------------------------------------------------------------------------
	Hub::Hub() {
		this->connectAll();
	}
	
	// --------------------------------------------------------------------------------------
	Hub::~Hub() {
		this->disconnectAll();
	}
	
	// --------------------------------------------------------------------------------------
	void Hub::disconnectAll() {
		for ( int i = 0 ; i < midiInPool.size() ; i++ )
			delete midiInPool[i];
		midiInPool.clear();
		/*
		 for (int i = 0 ; i < midiOutPool.size() ; i++)
		 delete midiOutPool[i];
		 midiOutPool.clear();
		 */
	}
	
	// --------------------------------------------------------------------------------------
	void Hub::connectAll() {
		
		for ( int i = 0 ; i < midii.getPortCount() ; i++ )
		{
			if (this->isDeviceConnected(midii.getPortName(i)))
				continue;
			
			printf("MIDI HUB: connecting to %i: %s\n",i,midii.getPortName(i).c_str());
			
			// Connect IN
			midi::Input *in = new midi::Input();
			in->openPort(i);
			midiInPool.push_back(in);
			
			// Connect OUT
			/*
			 midi::Output *out = new midi::Output();
			 out->openPort(i);
			 out->setVerbose(false);
			 midiOutPool.push_back(out);
			 */
		}
	}
	
	// --------------------------------------------------------------------------------------
	void Hub::update()
	{
		/// check only once per second
		static int sec = 0;
		if ( sec == (int) app::getElapsedSeconds() )
			return;
		sec = (int) app::getElapsedSeconds();
		
		// new devices?
		//printf("MIDI CHECK...\n");
		if ( midiInPool.size() != midii.getPortCount() )
			this->connectAll();
	}
	
	// --------------------------------------------------------------------------------------
	bool Hub::isDeviceConnected(std::string _name) {
		for (int i = 0 ; i < midiInPool.size() ; i++)
			if (midiInPool[i]->getName() == _name)
				return true;
		/*
		 for (int i = 0 ; i < midiOutPool.size() ; i++)
		 if (midiOutPool[i]->getName() == _name)
		 return true;
		 */
		return false;
	}
	
	// --------------------------------------------------------------------------------------
	bool Hub::hasWaitingMessages() {
		this->update();
		for ( int n = 0 ; n < midiInPool.size() ; n++ )
			if ( midiInPool[n]->hasWaitingMessages() )
				return true;
		// No messages!
		return false;
	}
	
	// --------------------------------------------------------------------------------------
	bool Hub::getNextMessage( Message* msg ) {
		this->update();
		for ( int n = 0 ; n < midiInPool.size() ; n++ )
			if ( midiInPool[n]->getNextMessage( msg ) )
				return true;
		// No messages!
		return false;
	}
	
	
} // namespace midi
} // namespace cinder
