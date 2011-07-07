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
 *	midi::Hub.h
 *	By Roger Sodre
 *
 *	Connects to ALL midi devices
 */

#pragma once

#include "MidiIn.h"
//#include "MidiOut.h"
#include "MidiMessage.h"
#include "MidiConstants.h"

namespace cinder { namespace midi {	
	
	class Hub {
	public:
		Hub();
		~Hub();
		
		void	disconnectAll();
		void	connectAll();
		void	update();
		
		int		getConnectedDeviceCount()	{ return midiInPool.size(); };
		bool	isConnected()				{ return (midiInPool.size() > 0 ? true : false); };
		bool	isDeviceConnected( std::string _name );
		
		bool	hasWaitingMessages();
		bool	getNextMessage( Message *msg );
		
	protected:
		
		RtMidiIn	midii;
		
		std::vector<midi::Input*>	midiInPool;
		//vector<midi::Output*>		midiOutPool;
	};
	
	
} // namespace midi
} // namespace cinder
