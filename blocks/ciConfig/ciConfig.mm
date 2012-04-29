//
//  ciConfig.mm
//
//  Created by Roger Sodre on 08/04/2010
//  Copyright 2011 Studio Avante. All rights reserved.
//


////////////////////////////////////////////////////////////////////////
//
// BASE CONFIG CLASS
//
//  Config class v1.0
//  Manage configuration parameters + save as text file
//  by Roger Sodre
//
//  Install:
//
// - Add addons/ofxMidi
// - Add CoreMIDI.framework
//
//  Usage:
//
// - Inherit Config class
// - Create param enum
// - Initialize parameters
//
//  TODO:
//
// - send string types on sendOsc()
//

#include "ciConfig.h"
#include <exception>
#include <errno.h>

#ifdef CINDER
using namespace cinder;
using namespace ci::app;
#endif

using namespace std;

namespace cinder {
	
	// comment to disable versosity
#define VERBOSE
	//#define OSC_VERBOSE
	
	ciConfig::ciConfig(ciConfig *_parent)
	{
		// init
		parent = _parent;
		saveTime = "using defaults";
		inPostSetCallback = false;
		postSetCallback_fn = NULL;
		bStarted = false;
		
		// make default filename based on app name
		fileExt = "cfg";
		std::string mAppName = [[[NSBundle mainBundle] objectForInfoDictionaryKey:(NSString *)kCFBundleNameKey] UTF8String];
		currentFileName = this->getFullFilename(mAppName + "." + fileExt);
		
		// Register key events
#ifdef CINDER
#ifdef CFG_CATCH_LOOP_EVENTS
		// not for presets
		if (parent == NULL)
			App::get()->registerKeyDown( this, &ciConfig::onKeyDown );
#endif
#endif
		
		// init MIDI
		midiPort = 0;
		midiChannel = 1;
		midiHub = NULL;
		// Do not enable devices for presets (if has parent, it's a preset)
		if (parent == NULL)
		{
#ifdef CFG_USE_MIDI
#ifdef CINDER
			midiHub = new midi::Hub();
#else
			midiHub = new ofxMidiHub(this);
#endif
#endif
			
			// init OSC
#ifdef CFG_USE_OSC
			this->setOscSend(false);
			this->setOscReceive(false);
#ifdef CINDER
			this->setOscReceive(true);	// CINDER always receive OSC because have no MIDI
#endif
#ifdef CFG_CATCH_LOOP_EVENTS
			ofAddListener(ofEvents.update, this, &ciConfig::oscCallback);
#endif
#endif
		}
		
		// init XML
		//xmlInOut = new XMLInOut(applet);
	}
	ciConfig::~ciConfig()
	{
		// free params
		while ( params.size() > 0 )
		{
			ciConfigParam* p = params.back();
			if (p != NULL)
				delete p;
			params.pop_back();
		}
		
		// free presets
		std::map<char,ciConfig*>::const_iterator it;
		for ( it = presets.begin() ; it != presets.end(); it++ )
			if (it->second)
				free (it->second);

#ifdef CFG_USE_MIDI
		if (midiHub)
			delete midiHub;
#endif
		
#ifdef CFG_USE_OSC
#ifdef CFG_CATCH_LOOP_EVENTS
		ofRemoveListener(ofEvents.update, this, &ciConfig::oscCallback);
#endif
#endif
	}
	
	//
	// Connect MIDI
#ifdef CFG_USE_MIDI
	void ciConfig::connectMidi()
	{
		midiHub->connectAll();
	}
	bool ciConfig::isMidiInConnected()
	{
		return midiHub->isConnected();
	}
#endif
	
	//
	// Update state
	//
	void ciConfig::update()
	{
		bStarted = true;
		
		// not for children (presets)
		if (parent)
			return;
		
#ifdef CINDER
		// Watch if Params updated var pointers
		for (int id = 0 ; id < params.size() ; id++)
		{
			ciConfigParam *p = params[id];
			if (p == NULL)
				continue;
			if (p->isColor())
			{
				for (int i = 0 ; i < 3 ; i++)
				{
					if ( (p->watchColor[i] * 255.0f) != this->get(id, i))
					{
						//printf("WATCH COLOR %d %.3f  old %.3f\n",i,(float)p->watchColor[i],this->get(id, i));
						this->set( id, i, p->watchColor[i] * 255.0f );
					}
				}
			}
			else if (p->isVector())
			{
				for (int i = 0 ; i < 3 ; i++)
				{
					if (p->watchVector[i] != this->get(id, i))
						this->set( id, i, p->watchVector[i] );
				}
			}
			else if (p->isBool())
			{
				if (p->watchBool != (bool) this->get(id))
					this->set( id, p->watchBool );
			}
			else if (p->isByte())
			{
				if (p->watchByte != (char) this->get(id))
					this->set( id, p->watchByte );
			}
			else if (p->isInt())
			{
				if (p->watchInt != (int) this->get(id))
					this->set( id, p->watchInt );
			}
			else if (p->isString())
			{
			}
			else
			{
				if (p->watchFloat[0] != this->get(id))
					this->set( id, p->watchFloat[0] );
			}
		}
		
		//
		// MIDI
#ifdef CFG_USE_MIDI
		midi::Message message;
		while ( midiHub->getNextMessage(&message) )
		{
			int channel = message.channel;
			int note = message.byteOne;
			int val = message.byteTwo;
			this->parseMidiMessage(channel, note, val);
		}
#endif
#endif	// CINDER
		
#ifdef CFG_USE_OSC
#ifndef CFG_CATCH_LOOP_EVENTS
		this->oscCallback();
#endif
#endif
	}
	//
	// Keyboard / Presets
#ifdef CINDER
	bool ciConfig::onKeyDown( app::KeyEvent event )
	{
		char c = event.getChar();
		//printf("EVENT>> ciConfig::keyDown [%d] [%c]\n",c,c);
		switch( c ) {
			case 's':
			case 'S':
				if (event.isMetaDown()) // COMMAND
					this->save();
				break;
			case 'l':
			case 'L':
				if (event.isMetaDown()) // COMMAND
					this->load();
				break;
			case '1' :
			case '2' :
			case '3' :
			case '4' :
			case '5' :
			case '6' :
			case '7' :
			case '8' :
			case '9' :
			case '0' :
				{
					if ( presets.find(c) != presets.end() )
					{
						// Save : COMMAND + SHIFT + NUMBER
						if (event.isMetaDown() && event.isControlDown())
							this->savePreset(c);
						// Load : CONTROL + NUMBER
						else if (event.isMetaDown())
							this->loadPreset(c);
					}
				}
				break;
		}
		return false;
	}
#endif
	
	
	/////////////////////////////////////////////////////////////////////////
	//
	// KEYBOARD
	//
	//
	// CINDER PARAM keys..
	//	SPACE (or ‘ ‘)
	//	BACKSPACE (or BS)
	//	TAB
	//	CLEAR (or CLR)
	//	RETURN (or RET)
	//	PAUSE
	//	ESCAPE (or ESC)
	//	DELETE (or DEL), INSERT (or INS)
	//	UP, DOWN, RIGHT, LEFT
	//	HOME, END
	//	PGUP, PGDOWN
	//	F1, F2,..., F15
	//
	// Key modifiers can be:
	//	SHIFT
	//	ALT
	//	CTRL
	//
	// Examples:
	//	A, b, #, 5, =, &, F10, BACKSPACE, ALT+A, CTRL+SPACE, SHIFT+F5, ALT+SHIFT+F12
	//
	void ciConfig::resetKeys(int id)
	{
		for (int n = 0 ; n < 3 ; n++)
		{
			strcpy( params[id]->vec[n].keySwitch, "" );
			strcpy( params[id]->vec[n].keyInc, "" );
			strcpy( params[id]->vec[n].keyDec, "" );
		}
	}
	void ciConfig::swapKeys(int id1, int id2)
	{
		char keys[3][3][20];
		for (int n = 0 ; n < 3 ; n++)
		{
			strcpy( keys[n][0], params[id1]->vec[n].keySwitch );
			strcpy( keys[n][1], params[id1]->vec[n].keyInc );
			strcpy( keys[n][2], params[id1]->vec[n].keyDec );
		}
		for (int n = 0 ; n < 3 ; n++)
		{
			strcpy( params[id1]->vec[n].keySwitch, params[id2]->vec[n].keySwitch );
			strcpy( params[id1]->vec[n].keyInc, params[id2]->vec[n].keyInc );
			strcpy( params[id1]->vec[n].keyDec, params[id2]->vec[n].keyDec );
		}
		for (int n = 0 ; n < 3 ; n++)
		{
			strcpy( params[id2]->vec[n].keySwitch, keys[n][0] );
			strcpy( params[id2]->vec[n].keyInc, keys[n][1] );
			strcpy( params[id2]->vec[n].keyDec, keys[n][2] );
		}
	}
	void ciConfig::setKey(int id, string key)
	{
		for (int n = 0 ; n < 3 ; n++)
			strcpy( params[id]->vec[n].keySwitch, key.c_str() );
	}
	void ciConfig::setKey(int id, string key0, string key1, string key2)
	{
		strcpy( params[id]->vec[0].keySwitch, key0.c_str() );
		strcpy( params[id]->vec[1].keySwitch, key1.c_str() );
		strcpy( params[id]->vec[2].keySwitch, key2.c_str() );
	}
	void ciConfig::setKeyUpDown(int id, string up, string down)
	{
		for (int n = 0 ; n < 3 ; n++)
		{
			strcpy( params[id]->vec[n].keyInc, up.c_str() );
			strcpy( params[id]->vec[n].keyDec, down.c_str() );
		}
	}
	void ciConfig::setKeyRGBUpDown(int id, string up0, string down0, string up1, string down1, string up2, string down2)
	{
		strcpy( params[id]->vec[0].keyInc, up0.c_str() );
		strcpy( params[id]->vec[0].keyDec, down0.c_str() );
		strcpy( params[id]->vec[1].keyInc, up1.c_str() );
		strcpy( params[id]->vec[1].keyDec, down1.c_str() );
		strcpy( params[id]->vec[2].keyInc, up2.c_str() );
		strcpy( params[id]->vec[2].keyDec, down2.c_str() );
	}
	
	
	/////////////////////////////////////////////////////////////////////////
	//
	// MIDI
	//
	void ciConfig::resetMidi(int id)
	{
		for (int n = 0 ; n < 3 ; n++)
		{
			params[id]->vec[n].midiNote = MIDI_NONE;
			params[id]->vec[n].midiInc = MIDI_NONE;
			params[id]->vec[n].midiDec = MIDI_NONE;
			params[id]->vec[n].midiSwitch = MIDI_NONE;
		}
	}
	void ciConfig::swapMidi(int id1, int id2)
	{
		int notes[3][5];
		for (int n = 0 ; n < 3 ; n++)
		{
			notes[n][0] = params[id1]->vec[n].midiChannel;
			notes[n][1] = params[id1]->vec[n].midiNote;
			notes[n][2] = params[id1]->vec[n].midiInc;
			notes[n][3] = params[id1]->vec[n].midiDec;
			notes[n][4] = params[id1]->vec[n].midiSwitch;
		}
		for (int n = 0 ; n < 3 ; n++)
		{
			params[id1]->vec[n].midiChannel	= params[id2]->vec[n].midiChannel;
			params[id1]->vec[n].midiNote	= params[id2]->vec[n].midiNote;
			params[id1]->vec[n].midiInc		= params[id2]->vec[n].midiInc;
			params[id1]->vec[n].midiDec	= params[id2]->vec[n].midiDec;
			params[id1]->vec[n].midiSwitch	= params[id2]->vec[n].midiSwitch;
		}
		for (int n = 0 ; n < 3 ; n++)
		{
			params[id2]->vec[n].midiChannel	= notes[n][0];
			params[id2]->vec[n].midiNote	= notes[n][1];
			params[id2]->vec[n].midiInc		= notes[n][2];
			params[id2]->vec[n].midiDec	= notes[n][3];
			params[id2]->vec[n].midiSwitch	= notes[n][4];
		}
	}
	void ciConfig::setMidiChannel(int id, int ch)
	{
		for (int n = 0 ; n < 3 ; n++)
			params[id]->vec[n].midiChannel = ch;
	}
	void ciConfig::setMidiChannelTrigger(int id, int ch)
	{
		this->setMidiChannel(id, ch);
		for (int n = 0 ; n < 3 ; n++)
			params[id]->vec[n].midiNote = MIDI_TRIGGER;
	}
	void ciConfig::setMidi(int id, short note, int ch)
	{
		this->setMidiChannel(id, ch);
		for (int n = 0 ; n < 3 ; n++)
			params[id]->vec[n].midiNote = note;
		// Can I push a value to MIDI device? I don't think so...
		//midiOut->stimulate(midiChannel, note);
		//midiOut->sendControlChange(midiChannel, note, 0);
	}
	void ciConfig::setMidi(int id, short note0, short note1, short note2, int ch)
	{
		this->setMidiChannel(id, ch);
		params[id]->vec[0].midiNote = note0;
		params[id]->vec[1].midiNote = note1;
		params[id]->vec[2].midiNote = note2;
	}
	void ciConfig::setMidiUpDown(int id, short up, short down, int ch)
	{
		this->setMidiChannel(id, ch);
		for (int n = 0 ; n < 3 ; n++)
		{
			params[id]->vec[n].midiInc = up;
			params[id]->vec[n].midiDec = down;
		}
	}
	void ciConfig::setMidiRGBUpDown(int id, short up0, short down0, short up1, short down1, short up2, short down2, int ch)
	{
		this->setMidiChannel(id, ch);
		params[id]->vec[0].midiInc = up0;
		params[id]->vec[0].midiDec = down0;
		params[id]->vec[1].midiInc = up1;
		params[id]->vec[1].midiDec = down1;
		params[id]->vec[2].midiInc = up2;
		params[id]->vec[2].midiDec = down2;
	}
	void ciConfig::setMidiSwitch(int id, short note, int ch)
	{
		this->setMidiChannel(id, ch);
		for (int n = 0 ; n < 3 ; n++)
			params[id]->vec[n].midiSwitch = note;
	}
	//
	// ofxMidi virtual callback
#ifdef CFG_USE_MIDI
#ifdef OPENFRAMEWORKS
	void ciConfig::newMidiMessage(ofxMidiEventArgs& eventArgs)
	{
		int channel = eventArgs.channel;
		int note = eventArgs.byteOne;
		int val = eventArgs.byteTwo;
		this->parseMidiMessage(channel, note, val);
	}
#endif
#endif
	//
	// PARSE MIDI message
	// used by OSC as well
	void ciConfig::parseMidiMessage(int channel, int note, int val)
	{
		if ( note < 0 || note > 127 )
		{
			printf("CFG MIDI IN ch[%d] note[%d] = [%d] INVALID NOTE!!!\n",channel,note,val);
			return;
		}
		if ( val < 0 || val > 127 )
		{
			printf("CFG MIDI IN ch[%d] note[%d] = [%d] INVALID VALUE!!!\n",channel,note,val);
			return;
		}
		float newProg = ((float)val / 127.0);
		printf("CFG MIDI IN ch[%d] note[%d] = [%d/%.2f]",channel,note,val,newProg);
		for (int id = 0 ; id < params.size() ; id++)
		{
			if (params[id] == NULL)
				continue;
			ciConfigParam *p = params.at(id);
			for (int i = 0 ; i < 3 ; i++)
			{
				if (p->vec[i].midiChannel != channel)
					continue;
				// The whole channel is triggering a Param
				// The note is actually the value to set
				if (p->vec[i].midiNote == MIDI_TRIGGER)
				{
					this->set(id, i, note);
					printf(" >> CFG id[%d/%d] = [%.6f] (%s)\n",id,i,p->vec[i].get(),p->name.c_str());
					return;
				}
				// Assign MIDI value to param
				else if (p->vec[i].midiNote == note)
				{
					this->setProg(id, i, newProg);
					printf(" >> CFG id[%d/%d] = [%.6f] (%s)\n",id,i,p->vec[i].get(),p->name.c_str());
					return;
				}
				// Increase param
				else if (p->vec[i].midiInc == note)
				{
					if (newProg == 1.0)
					{
						this->inc(id, i, true);
						printf(" >> CFG id[%d/%d] = [%.6f] (%s)\n",id,i,p->vec[i].get(),p->name.c_str());
					}
					else
						printf(" >> ignored\n");
					return;
				}
				// Decrease param
				else if (p->vec[i].midiDec == note)
				{
					if (newProg == 1.0)
					{
						this->dec(id, i, true);
						printf(" >> CFG id[%d/%d] = [%.6f] (%s)\n",id,i,p->vec[i].get(),p->name.c_str());
					}
					else
						printf(" >> ignored\n");
					return;
				}
				// Switch param / inc() in loop
				else if (p->vec[i].midiSwitch == note)
				{
					if (newProg == 1.0)
					{
						if (this->getProg(id, i) < 1.0)
							this->inc(id, i, true);
						else
							this->setToMin(id);
						printf(" >> CFG id[%d/%d] = [%.6f] (%s)\n",id,i,p->vec[i].get(),p->name.c_str());
					}
					else
						printf(" >> ignored\n");
					return;
				}
			}
		}
		printf(" >> unassigned\n");
	}
	
	//////////////////////////////////////////////////
	//
	// OSC
	//
#ifdef CFG_USE_OSC
	//
	// Enable/disable OSC
	// Send every param change thu OSC to another app with ciConfig
	void ciConfig::setOscSend(bool b, int port)
	{
		bOscSend = b;
		if (bOscSend)
		{
			bOscReceive = !b;
			this->switchOsc(port);
		}
	}
	//
	// Enable/disable OSC
	// Receive params change from another app with ciConfig
	// Receive MIDI as OSC from MidiAsOsc.app
	void ciConfig::setOscReceive(bool b, int port)
	{
		bOscReceive = b;
		if (bOscReceive)
		{
			bOscSend = !b;
			this->switchOsc(port);
		}
	}
	// Turn OSC on/off
	void ciConfig::switchOsc(int port)
	{
		try {
			if (bOscReceive)
			{
#ifndef CINDER
				oscSender.shutdown();
#endif
				oscReceiver.setup( port );
				sprintf(errmsg, "OSC RECEIVING PORT %d...",port);
			}
			else if (bOscSend)
			{
#ifndef CINDER
				oscReceiver.shutdown();
#endif
				oscSender.setup( DEFAULT_OSC_HOST, port );
				sprintf(errmsg, "OSC SENDING...");
			}
		} catch (exception &error) {
			sprintf(errmsg, "OSC SETUP FAILED!!!!");
			printf("OSC SETUP FAILED!!!\n");
		}
	}
	//
	// Send params change to another app with ciConfig
	void ciConfig::sendOsc(short id, short i)
	{
		// OSC Receive Disabled?
		if (bOscSend == false)
			return;
		// Do not send string
		if (params[id]->type == CFG_TYPE_STRING)
			return;
		// Send OSC
		string addr = "/";
		addr += this->getName(id);
		ofxOscMessage m;
		m.setAddress( addr );
		m.addIntArg( (int)i );
		if (params[id]->vec[i].preserveProg)
		{
			m.addStringArg( "P" );
			m.addFloatArg( this->getProg(id, i) );
		}
		else
		{
			m.addStringArg( "V" );
			m.addFloatArg( this->get(id, i) );
		}
		oscSender.sendMessage( m );
#ifdef OSC_VERBOSE
		printf("OSC OUT >> id[%d/%d] = [%.3f] (%s)\n",id,i,this->getProg(id, i),this->getName(id));
#endif
	}
	//
	// PUSH ALL PARAMS to another app with ciConfig
	void ciConfig::pushOsc()
	{
		for (int id = 0 ; id < params.size() ; id++)
		{
			if (params[id] == NULL)
				continue;
			for (int i = 0 ; i < 3 ; i++)
				sendOsc(id, i);
		}
	}
	//
	// ofxOsc callback
	// - Reads MIDI messages disguised as OSC forwarded from MidiAsOsc.app
	// - Reads PARAMS sent from another app using odxConfig
#ifdef CFG_CATCH_LOOP_EVENTS
	void ciConfig::oscCallback(ofEventArgs &e)
	{
		this->oscCallback();
	}
#endif
	void ciConfig::oscCallback()
	{
		// OSC Receive Disabled?
		if (bOscReceive == false)
			return;
#ifdef OSC_VERBOSE
		sprintf(errmsg, "OSC CALLBACK");
#endif
		//printf("OSC CALLBACK\n");
		// check for waiting messages
		while( oscReceiver.hasWaitingMessages() )
		{
			// get the next message
			ofxOscMessage m;
			if (oscReceiver.getNextMessage( &m ) == false)
				continue;
			
#ifdef OSC_VERBOSE
			sprintf(errmsg, "OSC MSG [%s]", m.getAddress().c_str());
			//printf("OSC MSG [%s] %.2f\n", m.getAddress().c_str(), m.getArgAsFloat( 0 ));
#endif
			// Receive from MisiAsOsc.app
			// Format:	/midi channel note value
			if ( m.getAddress() == "/midi" )
			{
				// both the arguments are int32's
				int channel	= m.getArgAsInt32( 0 );
				int note	= m.getArgAsInt32( 1 );
				float val	= m.getArgAsFloat( 2 );
#ifdef OSC_VERBOSE
				printf("OSC IN: /midi channel[%d] note[%d] value[%f]\n",channel,note,val);
#endif
				this->parseMidiMessage(channel, note, val);
			}
			// Receive from another ciConfig
			// Format:	/PARAM_NAME ix [P|V] value
			else
			{
				const char *name = m.getAddress().substr(1).c_str();
				for (int id = 0 ; id < params.size() ; id++)
				{
					if (params[id] == NULL)
						continue;
					ciConfigParam *p = params.at(id);
					if (name == p->name)
					{
						int i		= 0;
						string pv	= "P";
						float val;
						// iPad / ToushOSC
						if (m.getNumArgs() == 1)
							val	= m.getArgAsFloat( 0 );
						// MidiAsOSC
						else
						{
							i	= m.getArgAsInt32( 0 );
							pv	= m.getArgAsString( 1 );
							val	= m.getArgAsFloat( 2 );
						}
						
						if (pv[0] == 'P')
							this->setProg(id, i, val);
						else // value
							this->set(id, i, val);
#ifdef OSC_VERBOSE
						printf("OSC IN >> id[%d/%d] = [%.3f] (%s)\n",id,i,p->vec[i].get(),p->name.c_str());
#endif
						sprintf(errmsg, "OSC TO %s=%.3f", p->name.c_str(), p->vec[i].get());
						return;
					}
				}
			}
		}
		
	}
#endif
	
	
	/////////////////////////////////////////////////////////////////////////
	//
	// PRIVATE
	//
	
	// store new param
	void ciConfig::pushParam(int id, ciConfigParam* p)
	{
		// id is always the vector index
		// so we fill the vector with NULLs until it has <id> elements
		while (params.size() <= id)
			params.push_back(NULL);
		// replace param
		params.erase(params.begin()+id);
		params.insert(params.begin()+id, p);
	}
	// Is it a number?
	bool ciConfig::isNumber(int id)
	{
		switch (params[id]->type)
		{
			case CFG_TYPE_FLOAT:
			case CFG_TYPE_DOUBLE:
			case CFG_TYPE_INTEGER:
			case CFG_TYPE_LONG:
			case CFG_TYPE_BYTE:
			case CFG_TYPE_COLOR:
			case CFG_TYPE_VECTOR:
				return true;
				break;
		}
		return false;
	}
	// Is it a vector?
	bool ciConfig::isVector(int id)
	{
		switch (params[id]->type)
		{
			case CFG_TYPE_COLOR:
			case CFG_TYPE_VECTOR:
				return true;
				break;
		}
		return false;
	}
	
	
	/////////////////////////////////////////////////////////////////////////
	//
	// INITTERS
	//
	// generic (float)
	void ciConfig::add(short id, const string name, float val, float vmin, float vmax)
	{
		this->addFloat(id, name, val);
	}
	// specific
	void ciConfig::addFloat(short id, const string name, float val, float vmin, float vmax)
	{
		this->pushParam(id, new ciConfigParam(id, CFG_TYPE_FLOAT, name));
		this->setLimits(id, vmin, vmax);
		this->set(id, val);
		this->init(id);
	}
	void ciConfig::addDouble(short id, const string name, double val, double vmin, double vmax)
	{
		this->pushParam(id, new ciConfigParam(id, CFG_TYPE_DOUBLE, name));
		this->setLimits(id, (float)vmin, (float)vmax);
		this->set(id, val);
		this->init(id);
	}
	void ciConfig::addInt(short id, const string name, int val, int vmin, int vmax)
	{
		this->pushParam(id, new ciConfigParam(id, CFG_TYPE_INTEGER, name));
		this->setLimits(id, (float)vmin, (float)vmax);
		this->set(id, val);
		this->init(id);
	}
	void ciConfig::addLong(short id, const string name, long val, long vmin, long vmax)
	{
		this->pushParam(id, new ciConfigParam(id, CFG_TYPE_LONG, name));
		this->setLimits(id, (float)vmin, (float)vmax);
		this->set(id, val);
		this->init(id);
	}
	void ciConfig::addBool(short id, const string name, bool val)
	{
		this->pushParam(id, new ciConfigParam(id, CFG_TYPE_BOOLEAN, name));
		this->setLimits(id, 0.0f, 1.0f);
		this->set(id, val );
		this->init(id);
	}
	void ciConfig::addByte(short id, const string name, unsigned char val, unsigned char vmin, unsigned char vmax)
	{
		this->pushParam(id, new ciConfigParam(id, CFG_TYPE_BYTE, name));
		this->setLimits(id, (float)vmin, (float)vmax);
		this->set(id, (float) val);
		this->init(id);
	}
	void ciConfig::addString(short id, const string name, const string val)
	{
		this->pushParam(id, new ciConfigParam(id, CFG_TYPE_STRING, name));
		this->set(id, val);
		this->init(id);
	}
#ifndef CINDER
	void ciConfig::addColor(short id, const string name, int hex)
	{
		this->addColor(id, name, Color8u(hex));
	}
#endif
	void ciConfig::addColor(short id, const string name, Color8u c)
	{
		this->addColor(id, name, c.r, c.g, c.b);
	}
	void ciConfig::addColor(short id, const string name, float r, float g, float b)
	{
		this->pushParam(id, new ciConfigParam(id, CFG_TYPE_COLOR, name));
		this->setLimitsR(id, 0.0f, 255.0f);
		this->setLimitsG(id, 0.0f, 255.0f);
		this->setLimitsB(id, 0.0f, 255.0f);
		this->set(id, r, g, b);
		this->init(id);
	}
	void ciConfig::addVector(short id, const string name, Vec3f p, float vmin, float vmax)
	{
		this->addVector(id, name, p.x, p.y, p.z, vmin, vmax);
		this->init(id);
	}
	void ciConfig::addVector(short id, const string name, float x, float y, float z, float vmin, float vmax)
	{
		this->pushParam(id, new ciConfigParam(id, CFG_TYPE_VECTOR, name));
		this->setLimitsX(id, vmin, vmax);
		this->setLimitsY(id, vmin, vmax);
		this->setLimitsZ(id, vmin, vmax);
		/*
		 #ifdef CINDER
		 this->setLimitsX(id, 0.0f, getWindowWidth());
		 this->setLimitsY(id, 0.0f, getWindowHeight());
		 #else
		 this->setLimitsX(id, 0.0f, ofGetWidth());
		 this->setLimitsY(id, 0.0f, ofGetHeight());
		 #endif
		 */
		this->set(id, x, y, z);
		this->init(id);
	}
	// reset to initial values
	void ciConfig::init(int id)
	{
		if (params[id] != NULL)
			params[id]->init();
	}
	void ciConfig::reset()
	{
		for ( short id = 0 ; id < params.size() ; id++ )
		{
			ciConfigParam *p = params[id];
			if (p != NULL)
				for (int i = 0 ; i < 3 ; i++)
					this->set( id, i, p->vec[i].getInitialValue() );
		}
	}
	
	//
	// PRESETS
	//
	// Copy param from another cfg
	void ciConfig::makePresets(int presetCount, bool _auto )
	{
		char file[1024];
		for (int n = 0 ; ( n < presetCount && n < 10 ) ; n++)
		{
			char key = PRESET_KEY(n);
			ciConfig *pr = new ciConfig(this);
			presets[key] = pr;
			// set file
			int extsz = ( fileExt.length() + 1);
			string fmt = currentFileName;
			fmt.replace( fmt.size()-extsz, extsz, "_preset_%c."+fileExt );
			sprintf( file, fmt.c_str(), key );
			pr->setFile( file );
			//printf("made preset [%s]\n",file);
		}
		// copy params
		if (_auto)
			for ( short i = 0 ; i < params.size() ; i++ )
				if (params[i] != NULL)
					this->addParamToPreset(i);
	}
	void ciConfig::addParamToPreset(int id)
	{
		std::map<char,ciConfig*>::const_iterator it;
		for ( it = presets.begin() ; it != presets.end(); it++ )
		{
			//char key = (it->first);
			ciConfig* pr = (it->second);
			int t = this->getType(id);
			switch (t)
			{
				case CFG_TYPE_FLOAT:
					pr->addFloat(id, this->getName(id).c_str(), this->get(id), this->getMin(id), this->getMax(id));
					break;
				case CFG_TYPE_DOUBLE:
					pr->addDouble(id, this->getName(id).c_str(), this->getDouble(id), (double)this->getMin(id), (double)this->getMax(id));
					break;
				case CFG_TYPE_INTEGER:
					pr->addInt(id, this->getName(id).c_str(), this->getInt(id), (int)this->getMin(id), (int)this->getMax(id));
					break;
				case CFG_TYPE_LONG:
					pr->addLong(id, this->getName(id).c_str(), this->getLong(id), (long)this->getMin(id), (long)this->getMax(id));
					break;
				case CFG_TYPE_BOOLEAN:
					pr->addBool(id, this->getName(id).c_str(), this->getBool(id));
					break;
				case CFG_TYPE_BYTE:
					pr->addByte(id, this->getName(id).c_str(), this->getByte(id), (char)this->getMin(id), (char)this->getMax(id));
					break;
				case CFG_TYPE_COLOR:
					pr->addColor(id, this->getName(id).c_str(), this->getColor(id));
					break;
				case CFG_TYPE_VECTOR:
					pr->addVector(id, this->getName(id).c_str(), this->getVector(id), (char)this->getMin(id), (char)this->getMax(id));
					break;
				case CFG_TYPE_STRING:
					pr->addString(id, this->getName(id).c_str(), this->getString(id));
					break;
				default:
					printf("Config::presetAddParamFrom() ERROR invalid id[%d] type[%d]\n",id,t);
					break;
			};
		}
	}
	//
	// Save / load
	// i : preset number (1,2,...)
	int ciConfig::savePreset(char c)
	{
		std::map<char,ciConfig*>::const_iterator it = presets.find(c);
		if ( it != presets.end() )
		{
			ciConfig* pr = (it->second);
			// copy from source
			for (int id = 0 ; id < pr->params.size() ; id++ )
				if (pr->params[id])
					pr->set( id, this->get(id) );
			// save!
			return pr->save();
		}
		return 0;
	}
	int ciConfig::loadPreset(char c)
	{
		int r = 0;
		std::map<char,ciConfig*>::const_iterator it = presets.find(c);
		if ( it != presets.end() )
		{
			ciConfig* pr = (it->second);
			// load!
			r = pr->load();
			// copy back to source
			if (r > 0)
				for (int id = 0 ; id < pr->params.size() ; id++ )
					if (pr->params[id])
						this->set( id, pr->get(id) );
		}
		return r;
	}
	
	
	/////////////////////////////////////////////////////////////////////////
	//
	// SETTERS
	//
	void ciConfig::setLimits(int id, Vec3f vmin, Vec3f vmax)
	{
		this->setLimitsX(id, vmin.x, vmax.x);
		this->setLimitsY(id, vmin.y, vmax.y);
		this->setLimitsZ(id, vmin.z, vmax.z);
	}
	void ciConfig::setLimitsDegrees(int id)
	{
		this->setLimitsX(id, 0, 360);
		this->setLimitsY(id, 0, 360);
		this->setLimitsZ(id, 0, 360);
	}
	void ciConfig::setLimitsRadians(int id)
	{
		this->setLimitsX(id, 0, TWO_PI);
		this->setLimitsY(id, 0, TWO_PI);
		this->setLimitsZ(id, 0, TWO_PI);
	}
	void ciConfig::setLimitsScreen(int id)
	{
#ifdef CINDER
		this->setLimitsX(id, 0, getWindowWidth());
		this->setLimitsY(id, 0, getWindowHeight());
		this->setLimitsZ(id, 0, 0);
#else
		this->setLimitsX(id, 0, ofGetWidth());
		this->setLimitsY(id, 0, ofGetHeight());
		this->setLimitsZ(id, 0, 0);
#endif
		
		
	}
	//
	// Called after any set
	void ciConfig::post_set(int id, int i)
	{
#ifdef CINDER
		// update Param pointers
		params[id]->updatePointers(i);
#endif
		// Set freshness
		bool f = false;
		switch (params[id]->type)
		{
			case CFG_TYPE_FLOAT:
			case CFG_TYPE_DOUBLE:
			case CFG_TYPE_COLOR:
			case CFG_TYPE_VECTOR:
				if ( this->get(id, i) != this->getLastValue(id, i) )
					f = true;
				break;
			case CFG_TYPE_INTEGER:
			case CFG_TYPE_LONG:
			case CFG_TYPE_BOOLEAN:
			case CFG_TYPE_BYTE:
				if ( (int) this->get(id, i) != (int) this->getLastValue(id, i) )
					f = true;
				break;
			default:
				f = true;
				break;
		};
		// If fresh...
		if (f)
		{
			// set as fresh
			freshness = true;
			params[id]->vec[i].freshness = true;
			// save current value
			this->updateLastValue(id, i);
			// Virtual callback
			if ( bStarted && !inPostSetCallback )
			{
				inPostSetCallback = true;
				if ( postSetCallback_fn )
					postSetCallback_fn(this, id, i);
				else
					this->postSetCallback(id, i);
				inPostSetCallback = false;
			}
#ifdef CFG_USE_OSC
			// Send OSC
			this->sendOsc(id, i);
#endif
		}
	}
	//
	// PROG
	//
	// Preserve Prog: prog is priority
	// Prog will be saved and read form file instead of value
	void ciConfig::preserveProg(int id, bool b)
	{
		params[id]->vec[0].preserveProg = b;
		params[id]->vec[1].preserveProg = b;
		params[id]->vec[2].preserveProg = b;
	}
	// Set Prog
	void ciConfig::setProg(int id, int i, float p)		// Private
	{
		params[id]->vec[i].setProg(p);
		this->post_set(id, i);
	}
	void ciConfig::setProg(int id, float val0, float val1, float val2)
	{
		this->setProgX(id,val0);
		this->setProgY(id,val1);
		this->setProgZ(id,val2);
	}
	// generic (float)
	// val1, val2 will be used only on CFG_TYPE_VECTOR and CFG_TYPE_COLOR
	void ciConfig::set(int id, int i, float val)		// Private
	{
		ciConfigParam *p = params[id];
		switch (p->type)
		{
			case CFG_TYPE_FLOAT:
			case CFG_TYPE_DOUBLE:
			case CFG_TYPE_INTEGER:
			case CFG_TYPE_LONG:
			case CFG_TYPE_BOOLEAN:
			case CFG_TYPE_BYTE:
			case CFG_TYPE_COLOR:
			case CFG_TYPE_VECTOR:
				p->vec[i].set(val);
				break;
			case CFG_TYPE_STRING:
			{
				std::ostringstream os;
				os << val;
				p->strval = os.str();
				break;
			}
			default:
				printf("Config.set(int) ERROR invalid id[%d] type[%d] val[%d]\n",id,p->type,(int)val);
				return;
				break;
		};
		this->post_set(id, i);
	}
	// If CFG_TYPE_STRING, just set
	// Else, convert to float
	// pp = Prog?
	void ciConfig::set(int id, const char *val, bool pp)
	{
		// Set string!
		if (params[id]->type == CFG_TYPE_STRING)
		{
			params[id]->strval = string( val );
			params[id]->updatePointers(0);
			//this->post_set(id, 0);
		}
		// Set vector values
		// val = "number,number,number"
		else if (this->isVector(id))
		{
			char *p1, *p2;
			char vals[3][16];
			memset(vals,0,sizeof(vals));
			// val1
			if ((p1 = strchr(val,',')) != NULL)
			{
				strncpy(vals[0],val,(p1-val));
				if ((p2 = strchr(p1+1,',')) != NULL)
				{
					strncpy(vals[1],p1+1,(p2-p1));
					strcpy(vals[2],p2+1);
				}
			}
			// set!
			if (pp)
				this->setProg(id, atof(vals[0]), atof(vals[1]), atof(vals[2]));
			else
				this->set(id, atof(vals[0]), atof(vals[1]), atof(vals[2]));
		}
		// defualt: float
		else if (pp)
			this->setProg(id, 0, val);
		else
			this->set(id, 0, val);
	}
	void ciConfig::set(int id, float val0, float val1, float val2)
	{
		this->set(id,0,val0);
		this->set(id,1,val1);
		this->set(id,2,val2);
	}
	//
	// Vaule labels for radio buttons
	// Works for CFG_TYPE_INTEGER
	// TERMINATE WITH NULL
	// example: this->setValueLabels(THE_SHAPE, "Sphere", "Cube", NULL);
	void ciConfig::setValueLabels(short id, const char *val0, ...)
	{
		ciConfigParam *param = params[id];
		va_list args;
		va_start (args, val0);
		char *p = (char*) val0;			// get first
		for (int key = 0 ; p ; key++)
		{
			param->valueLabels[key] = string(p);
			p = va_arg (args, char *);	// next
			//printf("label %s\n",p);
		}
		va_end (args);
		// virtual
		this->guiUpdateValueLabels(id);
	}
	// accept labels like this...
	//	char labels[][64] = 
	//	{
	//		"640 x 480",
	//		"1024 x 768",
	//		"1280 x 1024",
	//		"1920 x 1080",
	//		NULL
	//	};
	void ciConfig::updateLimitsByValueLabels(short id)
	{
		ciConfigParam *param = params[id];
		if (param->valueLabels.size() == 0)
			this->setLimits(id, 0, 0);
		else
		{
			std::map<int,std::string>::const_iterator it_min = param->valueLabels.begin();
			std::map<int,std::string>::const_reverse_iterator it_max = param->valueLabels.rbegin();
			this->setLimits(id, it_min->first, it_max->first);
		}
	}
	void ciConfig::clearValueLabels(short id)
	{
		params[id]->valueLabels.clear();
		this->updateLimitsByValueLabels(id);
		// virtual
		this->guiUpdateValueLabels(id);
	}
	void ciConfig::setValueLabels(short id, const char labels[][64])
	{
		for (int key = 0 ; labels[key][0] != '\0' ; key++)
			params[id]->valueLabels[key] = string(labels[key]);
		this->updateLimitsByValueLabels(id);
		// virtual
		this->guiUpdateValueLabels(id);
	}
	void ciConfig::setValueLabels(short id, const std::map<int,std::string> & labels)
	{
		std::map<int,std::string>::const_iterator it;
		for ( it = labels.begin() ; it != labels.end(); it++ )
		{
			int key = (it->first);
			std::string name = (it->second);
			params[id]->valueLabels[key] = name;
		}
		this->updateLimitsByValueLabels(id);
		// virtual
		this->guiUpdateValueLabels(id);
	}
	// SETTER / GETER
	void ciConfig::setValueLabel(short id, int key, string label, bool indexToo)
	{
		//printf("SET LABEL [%d] = [%s]\n",key,label.c_str());
		ciConfigParam *param = params[id];
		if (indexToo)
		{
			std::ostringstream os;
			os << key << ": " << label;
			param->valueLabels[key] = os.str();
		}
		else
			param->valueLabels[key] = label;
		this->updateLimitsByValueLabels(id);
		// virtual
		this->guiUpdateValueLabels(id);
	}
	string ciConfig::getValueLabel(short id, int key)
	{
		ciConfigParam *param = params[id];
		string str = param->valueLabels[key];
		if (str.length())
			return str;
		else
			return this->getString(id);
	}
	
	
	/////////////////////////////////////////////////////////////////////////
	//
	// OPERATIONS
	//
	// Invert a value
	void ciConfig::invert(int id, short i)		// Private
	{
		params[id]->vec[i].invert();
		this->post_set(id, i);
	}
	void ciConfig::invert(int id)
	{
		if (this->isVector(id))
		{
			this->invert(id, 0);
			this->invert(id, 1);
			this->invert(id, 2);
		}
		else
			this->invert(id, 0);
	}
	//
	// Subtract a value
	void ciConfig::sub(int id, int i, float val, bool clamp)	// Private
	{
		params[id]->vec[i].sub(val, clamp);
		this->post_set(id, i);
	}
	void ciConfig::sub(int id, float val, bool clamp)
	{
		if (this->isVector(id))
		{
			this->sub(id, 0, val, clamp);
			this->sub(id, 1, val, clamp);
			this->sub(id, 2, val, clamp);
		}
		else
			this->sub(id, 0, val, clamp);
	}
	//
	// Add  a value
	void ciConfig::add(int id, int i, float val, bool clamp)	// Private
	{
		params[id]->vec[i].add(val, clamp);
		this->post_set(id, i);
	}
	void ciConfig::add(int id, float val, bool clamp)
	{
		if (this->isVector(id))
		{
			this->add(id, 0, val, clamp);
			this->add(id, 1, val, clamp);
			this->add(id, 2, val, clamp);
		}
		else
			this->add(id, 0, val, clamp);
	}
	// inc + loop para o primeiro
	void ciConfig::incLoop(int id)
	{
		this->inc(id, false);	// inc, NO clamp
		if ( this->getInt(id) > this->getMax(id) )
			this->setToMin(id);
	}
	
	
	
	/////////////////////////////////////////////////////////////////////////
	//
	// GETTERS
	//
	// Get generic value (float)
	float ciConfig::get(int id, int i)		// Private
	{
		ciConfigParam *p = params[id];
		switch (p->type)
		{
			case CFG_TYPE_FLOAT:
			case CFG_TYPE_DOUBLE:
			case CFG_TYPE_INTEGER:
			case CFG_TYPE_LONG:
			case CFG_TYPE_BOOLEAN:
			case CFG_TYPE_BYTE:
			case CFG_TYPE_COLOR:
			case CFG_TYPE_VECTOR:
				return p->vec[i].get();
				break;
			case CFG_TYPE_STRING:
				return atof(p->strval.c_str());
				break;
			default:
				printf("Config.get(int) ERROR invalid id[%d] type[%d]\n",id,p->type);
				return 0.0f;
				break;
		};
	}
	const char* ciConfig::getString(int id, bool raw)
	{
		ciConfigParam *param = params[id];
		// String type: no conversion
		if (param->type == CFG_TYPE_STRING)
			return (param->strval).c_str();
		// Init string
		bool pp = param->vec[0].preserveProg;
		param->strval = (pp ? "P" : "");
		// Convert to string...
		for (int i = 0 ; i < (this->isVector(id)?3:1) ; i++)
		{
			char val[16];
			if (i > 0)
				param->strval += ",";
			// prog value
			if (pp)
				sprintf(val,"%f",this->getProg(id,i));
			// normal value
			else
			{
				// Format value
				switch(param->type)
				{
					case CFG_TYPE_INTEGER:
						if (raw)
							sprintf(val,"%d",(int)this->get(id));
						else
						{
							//int v = (int)this->get(id,i) - (int)this->getMin(id,i);
							string l = param->valueLabels[this->getInt(id)];
							if (l.length())
								return l.c_str();
							else
								sprintf(val,"%d",(int)this->get(id));
						}
						break;
					case CFG_TYPE_COLOR:
						sprintf(val,"%d",(int)this->get(id,i));
						break;
					case CFG_TYPE_BYTE:
						if (raw)
							sprintf(val,"%d",(int)this->get(id,i));
						else
							sprintf(val,"%c",(int)this->get(id,i));
						break;
					case CFG_TYPE_LONG:
						sprintf(val,"%ld",(long)this->get(id,i));
						break;
					case CFG_TYPE_BOOLEAN:
						if (raw)
							sprintf(val,"%d",(this->get(id,i) == 0.0 ? 0 : 1));
						else
							sprintf(val,"%s",(this->get(id,i) == 0.0 ? "OFF" : "ON"));
						break;
					default:
						sprintf(val,"%f",this->get(id,i));
						break;
				}
			}
			param->strval += val;
		}
		// return string
		return (param->strval).c_str();
	}
	//
	// Normalized getters
	float ciConfig::getDegrees(int id, int i)
	{
		float v = this->get(id, i);
		while (v < 0.0f)
			v += 360.0f;
		while (v >= 360.0f)
			v -= 360.0f;
		return v;
	}
	float ciConfig::getRadians(int id, int i)
	{
		float v = this->get(id, i);
		while (v < 0.0f)
			v += TWO_PI;
		while (v >= TWO_PI)
			v -= TWO_PI;
		return v;
	}
	
	
	/////////////////////////////////////////////////////////////////////////
	//
	// SAVE / READ utils
	//
	// Set the default file extension
	void ciConfig::setFileExtension(const std::string e)
	{
		// Change current filename extension
		if ( currentFileName.length() )
			currentFileName.replace( currentFileName.size()-fileExt.length(), fileExt.length(), e );
		// Set new extension
		fileExt = e;
	}
	//
	// Sets default file and load it
	void ciConfig::setFile(const std::string & f, const std::string & path)
	{
		if (f[0] == '/')
			currentFileName = f;
		else
			currentFileName = this->getFullFilename(f, path);
	}
	int ciConfig::useFile(const std::string & f, const std::string & path)
	{
		this->setFile(f, path);
		return this->load();
	}
	//
	// Get file name
	string ciConfig::getFullFilename(const std::string & f, const std::string & path)
	{
		// Use current
		if ( f.length() == 0 )
			return currentFileName;
		
		// file has path
		if ( strchr(f.c_str(), '/') != NULL )
			return f;
		
		// Make filename
		std::ostringstream os;
		// Cinder file
#ifdef CINDER_COCOA_TOUCH
		os << getHomeDirectory() << f;
#else
		if ( path.length() == 0 )
		{
			//os << "../../../" << f;
			//os << "./" << f;

			// Save besides your ".app"
			std::string appPath = getPathDirectory( app::getAppPath().string() );
			os << appPath << f;
		}
		else
			os << path << f;
#endif
		return os.str();
	}
	
	
	
	
	/////////////////////////////////////////////////////////////////////////
	//
	// SAVE / READ config to file
	//
	// SAVE to file on data folder
	// Return saved params
	int ciConfig::readFile(const std::string & f)
	{
		// Open file
		string fullFilename = this->getFullFilename(f);
		FILE *fp = fopen (fullFilename.c_str(),"r");
		if (fp == NULL)
		{
			printf("ERROR reading config file [%s] errno [%d/%s]\n",fullFilename.c_str(),errno,strerror(errno));
			sprintf(errmsg, "READ ERROR! [%s]", fullFilename.c_str());
			return 0;
		}
		
		// Read file
		int pid;
		bool pp;						// is prog?
		char *p;						// aux pointer
		char data[CFG_MAX_DATA_LEN];	// full line
		string key;						// param key
		string val;						// param value
		for (int n = 0 ; fgets(data, sizeof(data) ,fp) != NULL ; n++)
		{
			// Comment?
			if (data[0] == '#')
				continue;
			// erase '\n';
			data[strlen(data)-1] = '\0';
			// find separator
			p = strchr(data,':');
			if (p == NULL)
				continue;
			// get val
			if ( (pp = (*(p+1)=='P')) == true)
				val = string(p+2);
			else
				val = string(p+1);
			// get key
			*p = '\0';
			key = data;
#ifdef VERBOSE
			printf ("READ CFG: %s = %s",key.c_str(),val.c_str());
#endif
			
			// Read params
			for (pid = 0 ; pid < params.size() ; pid++ )
			{
				ciConfigParam *param = params[pid];
				if (param == NULL)
					continue;
				if ( key == param->name )
				{
					this->preserveProg(pid, pp);
					this->set(pid, val.c_str(), pp);
					break;
				}
			}
			
#ifdef VERBOSE
			// finish output
			if (pid == params.size() && key != "SAVE_TIME")
				printf ("  !!!!!!! param do not exist!\n");
			else
				printf ("\n");
#endif
			
			// Is saved time?
			if (key == "SAVE_TIME")
				saveTime = val;
		}
		// Virtual Callback
		this->postLoadCallback();
		
		// Close file
		fclose (fp);
		printf("READ config [%s] OK! %d params\n",fullFilename.c_str(),(int)params.size());
		sprintf(errmsg, "READ [%s]", fullFilename.c_str());
		
		// return param count
		return params.size();
	}
	//
	// SAVE to file on data folder
	// Return saved params
	int ciConfig::saveFile(const std::string & f)
	{
		// Open file
		string fullFilename = this->getFullFilename(f);
		FILE *fp = fopen (fullFilename.c_str(),"w");
		if (fp == NULL)
		{
			printf("ERROR saving config [%s] errno [%d/%s]\n",fullFilename.c_str(),errno,strerror(errno));
			sprintf(errmsg, "SAVE ERROR!!! [%s]", fullFilename.c_str());
			return 0;
		}
		
		// Virtual Callback
		this->preSaveCallback();
		
		// Save comments
		fputs("#\n",fp);
		fputs("# ciConfig file\n",fp);
		fputs("#\n",fp);
		fputs("# Format:\n",fp);
		fputs("#\tPARAM_NAME[.channel]:[P]value\n",fp);
		fputs("# Examples:\n",fp);
		fputs("#\tPARAM_VALUE:10.0\t(parameter with value)\n",fp);
		fputs("#\tPARAM_PROG:P1.0\t\t(parameter with prog: from 0.0(vmin) to 1.0(vmax))\n",fp);
		fputs("#\tPARAM_VECTOR:150.0,150.0,0.0\t(vector: x,y,z)\n",fp);
		fputs("#\tPARAM_VECTOR:P0.5,0.5,0.0\t\t(vector with prog: x,y,z)\n",fp);
		fputs("#\tPARAM_COLOR:255,255,255\t\t\t(color: r,g,b)\n",fp);
		fputs("#\tPARAM_COLOR:P0.0,0.5,1.0\t\t(color with prog: r,g,b)\n",fp);
		fputs("#\n",fp);
		
		// generate save time
		char data[CFG_MAX_DATA_LEN];
		time_t now;
		time ( &now );
		saveTime = string(&(ctime(&now)[4]));
		sprintf(data,"SAVE_TIME:%s",saveTime.c_str());
		fputs(data,fp);
		
		// Save params
		for (int id = 0 ; id < params.size() ; id++ )
		{
			ciConfigParam *param = params[id];
			if ( param == NULL )
				continue;
			if ( param->dummy )
				continue;
			// make line (name+value)
			sprintf(data,"%s:%s\n",this->getName(id).c_str(),this->getString(id, true));
			// save line
			fputs  (data,fp);
		}
		
		// Close file
		fclose (fp);
		
		// Ok!
		printf("SAVE config [%s] OK!\n",fullFilename.c_str());
		sprintf(errmsg, "SAVED [%s]", fullFilename.c_str());
		
		// return param count
		return params.size();
	}
	
	
	
	
	
	
	///////////////////////////////////////////////
	//
	// COCOA INTEGRATION
	//
	// Cocoa Slider
	void ciConfig::cocoaSetupSlider(int id, NSSlider *slider)
	{
		[slider setMinValue:this->getMin(id)];
		[slider setMaxValue:this->getMax(id)];
		switch (params[id]->type)
		{
			case CFG_TYPE_FLOAT:
			case CFG_TYPE_DOUBLE:
				[slider setFloatValue:this->get(id)];
				break;
			case CFG_TYPE_INTEGER:
			case CFG_TYPE_LONG:
			case CFG_TYPE_BYTE:
				[slider setIntValue:this->getInt(id)];
				break;
		}
		
	}
	void ciConfig::cocoaReadSlider(int id, NSSlider *slider)
	{
		this->set(id, [slider floatValue]);
	}
	//
	// Cocoa button
	void ciConfig::cocoaSetupButton(int id, NSButton *button)
	{
		[button setState: ( this->getBool(id) ? NSOnState : NSOffState )];
	}
	void ciConfig::cocoaReadButton(int id, NSButton *button)
	{
		this->set(id, (bool)( [button state] == NSOnState ? true : false ));
	}
	//
	// Cocoa Popup (combo box)
	void ciConfig::cocoaSetupPopup(int id, NSPopUpButton *pop)
	{
		int v = this->getInt(id);
		int min = (int)this->getMin(id);
		int max = (int)this->getMax(id);
		[pop removeAllItems];
		for (int n = min ; n <= max ; n++)
		{
			int ix = (n - min);
			if ( ix < params[id]->valueLabels.size())
				[pop addItemWithTitle:STR2NSS(params[id]->valueLabels[ix].c_str())];
			else
				[pop addItemWithTitle:[NSString stringWithFormat:@"%d",n]];
			if (v == n)
				[pop selectItemAtIndex:ix];
			//printf("GUI POP id(%d) ix %d = %d, current %d\n",id,ix,n,v);
		}
	}
	void ciConfig::cocoaReadPopup(int id, NSPopUpButton *pop)
	{
		int v = (int)[pop indexOfSelectedItem] + (int)this->getMin(id);
		this->set(id, v);
	}
	//
	// Cocoa Segmented
	void ciConfig::cocoaSetupSeg(int id, NSSegmentedControl *seg)
	{
		int v = this->getInt(id);
		int min = (int)this->getMin(id);
		int max = (int)this->getMax(id);
		int count = (max-min+1);
		[seg setSegmentCount:count];
		float w = ( ([seg frame].size.width-10) / count );
		for (int n = min ; n <= max ; n++)
		{
			int ix = (n - min);
			[seg setWidth:w forSegment:ix];
			if ( ix < params[id]->valueLabels.size())
				[seg setLabel:STR2NSS(params[id]->valueLabels[ix].c_str()) forSegment:ix];
			else
				[seg setLabel:[NSString stringWithFormat:@"%d",n] forSegment:ix];
			if (v == n)
				[seg setSelectedSegment:ix];
			//printf("GUI SEG id(%d) ix %d = %d, current %d\n",id,ix,n,v);
		}
	}
	void ciConfig::cocoaReadSeg(int id, NSSegmentedControl *seg)
	{
		int v = (int)[seg selectedSegment] + (int)this->getMin(id);
		this->set(id, v);
	}
	
	
	
} // cinder::
