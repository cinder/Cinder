/*
 *  MidiIn.cpp
 *  glitches
 *
 *  Created by hec on 5/20/10.
 *  Copyright 2010 aer studio. All rights reserved.
 *
 */

#include "MidiIn.h"

namespace cinder { namespace midi {

	void MidiInCallback(double deltatime, std::vector<unsigned char> *message, void *userData){
		((Input*)userData)->processMessage(deltatime, message);
	}
	
	
	Input::Input(){
		mNumPorts = mMidiIn.getPortCount();
	}
	
	Input::~Input(){
		closePort();
	}
	
	void Input::listPorts(){
		std::cout << "MidiIn: " << mNumPorts << " available." << std::endl;
		for (size_t i = 0; i < mNumPorts; ++i){
			std::cout << i << ": " << mMidiIn.getPortName(i).c_str() << std::endl;
			mPortNames.push_back(mMidiIn.getPortName(i));
		}
	}
	
	void Input::openPort(unsigned int port){
		if (mNumPorts == 0){
			throw MidiExcNoPortsAvailable();
		}
		
		if (port + 1 > mNumPorts){
			throw MidiExcPortNotAvailable();
		}
		
		mPort = port;
		mName = mMidiIn.getPortName(port);
		
		mMidiIn.openPort(mPort);
		
		mMidiIn.setCallback(&MidiInCallback, this);
		
		mMidiIn.ignoreTypes(false, false, false);
	}
	
	void Input::closePort(){
		mMidiIn.closePort();
	}
	
	void Input::processMessage(double deltatime, std::vector<unsigned char> *message){
		unsigned int numBytes = message->size();
		 
		if (numBytes > 0){
			Message* msg = new Message();
			msg->port = mPort;
			msg->channel = ((int)(message->at(0)) % 16) + 1;
			msg->status = ((int)message->at(0)) - (msg->channel - 1);
			msg->timeStamp = deltatime;
			
			if (numBytes == 2){
				msg->byteOne = (int)message->at(1);
			}else if (numBytes == 3){
				msg->byteOne = (int)message->at(1);
				msg->byteTwo = (int)message->at(2);
			}
			
			//mSignal(msg);
			
			mMessages.push_back(msg);
		}
		
	}
	
	bool Input::hasWaitingMessages(){
		int queue_length = (int)mMessages.size();
		return queue_length > 0;
	}
	
	bool Input::getNextMessage(Message* message){
		if (mMessages.size() == 0){
			return false;
		}
		
		Message* src_message = mMessages.front();
		message->copy(*src_message);
		delete src_message;
		mMessages.pop_front();
		
		return true;
	}
	
	unsigned int Input::getPort()const{
		return mPort;
	
	}
	

} // namespace midi
} // namespace cinder