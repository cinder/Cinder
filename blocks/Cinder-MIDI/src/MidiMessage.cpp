/*
 *  Message.cpp
 *  glitches
 *
 *  Created by hec on 5/20/10.
 *  Copyright 2010 aer studio. All rights reserved.
 *
 */

#include "MidiMessage.h"

namespace cinder { namespace midi {
	
	Message::Message(){
	
	}
	
	Message& Message::copy(const Message& other){
		port = other.port;
		channel = other.channel;
		status = other.status;
		byteOne = other.byteOne;
		byteTwo = other.byteTwo;
		
		return *this;
	}

}// namespace midi
}// namespace cinder