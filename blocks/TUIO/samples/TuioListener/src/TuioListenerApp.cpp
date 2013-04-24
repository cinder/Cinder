/*
 Copyright (c) 2010, Hector Sanchez-Pajares
 Aer Studio http://www.aerstudio.com
 All rights reserved.
 
 
 This is a block for TUIO Integration for the Cinder framework (http://libcinder.org)
 
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

#include "cinder/app/AppNative.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#include "TuioClient.h"
#include "TuioCursor.h"
#include "OscMessage.h"

class TuioClientApp : public AppNative {
  public:
	void setup();
	void update();

	void draw2( tuio::Cursor cursor, int sourcenum );
	void draw2b( tuio::Cursor cursor );
	void draw25d( tuio::Cursor25d cursor );
	void draw();
	
	void cursorAdded( tuio::Cursor cursor );
	void cursorUpdated( tuio::Cursor cursor );
	void cursorRemoved( tuio::Cursor cursor );
	void oscMessage( const osc::Message *message );

	tuio::Client tuio;

	CallbackId	mUpdatedCallbackIndex;
};


void TuioClientApp::setup()
{
	tuio.connect( 3333 );
	
	tuio.registerCursorAdded( this, &TuioClientApp::cursorAdded );
	mUpdatedCallbackIndex = tuio.registerCursorUpdated( this, &TuioClientApp::cursorUpdated );
	tuio.registerCursorRemoved( this, &TuioClientApp::cursorRemoved );
	tuio.registerOscMessageReceived( this, &TuioClientApp::oscMessage );
}

void TuioClientApp::cursorAdded( tuio::Cursor cursor ) {
	console() << "Cursor added " << cursor.getSessionId() << std::endl;
}

void TuioClientApp::cursorUpdated( tuio::Cursor cursor ) {
	console() << "Cursor updated " << cursor.getSessionId() << std::endl;
}

void TuioClientApp::cursorRemoved( tuio::Cursor cursor ) {
	console() << "Cursor removed " << cursor.getSessionId() << std::endl;
}

void TuioClientApp::oscMessage( const osc::Message *message) {
	console() << "OSC Message " << message->getAddress() << std::endl;
}

void TuioClientApp::update()
{
	if( getElapsedSeconds() > 10.0f && mUpdatedCallbackIndex > -1 ) {
		console() << "Unregistering cursor updated." << std::endl;
		tuio.unregisterCursorUpdated( mUpdatedCallbackIndex );
		mUpdatedCallbackIndex = -1;
	}
}

void TuioClientApp::draw2( tuio::Cursor cursor, int sourcenum )
{	
	switch( sourcenum % 6 ) {
		case 0: gl::color(ColorA(1.0f, 0.0f, 0.0f, 0.6f)); break;
		case 1: gl::color(ColorA(0.0f, 1.0f, 0.0f, 0.6f)); break;
		case 2: gl::color(ColorA(0.0f, 0.0f, 1.0f, 0.6f)); break;
		case 3: gl::color(ColorA(1.0f, 1.0f, 0.0f, 0.6f)); break;
		case 4: gl::color(ColorA(0.0f, 1.0f, 1.0f, 0.6f)); break;
		case 5: gl::color(ColorA(1.0f, 0.0f, 1.0f, 0.6f)); break;
	}
	gl::drawSolidCircle( cursor.getPos() * Vec2f(getWindowSize()), 30 );
}

void TuioClientApp::draw2b( tuio::Cursor cursor )
{
	gl::color( Color::white() );
	gl::drawSolidCircle( cursor.getPos() * Vec2f(getWindowSize()), 5.0f );
}

void TuioClientApp::draw25d( tuio::Cursor25d cursor )
{
	gl::color(ColorA(0.0f, 1.0f, 0.0f, 1.0f));
	float radius = 75.0f * cursor.getPos25().z;
	gl::drawSolidCircle( cursor.getPos() * Vec2f(getWindowSize()), radius );
}

void TuioClientApp::draw()
{
	if( tuio.isConnected() ) {
		gl::clear( Color( 0, 0, 0 ) );
			
		// Draw a center dot in all the cursors, to test the ability
		// of tuio.getCursors() to get all cursors in one vector.
		vector<tuio::Cursor> cursors = tuio.getCursors();
		for( auto cursor = cursors.begin(); cursor != cursors.end(); ++cursor )
			draw2b( *cursor );

		// Draw each source's cursors in a different color, to test the ability
		// of tuio.getCursors() to get each source's cursors independently.
		set<string> sources = tuio.getSources();
		int sourcenum = 0;
		for( auto source = sources.begin(); source != sources.end(); ++source,++sourcenum ) {
			vector<tuio::Cursor> cursors = tuio.getCursors(*source);
			for( auto cursor = cursors.begin(); cursor != cursors.end(); ++cursor ) {
				draw2( *cursor, sourcenum );
			}
		}

		// If there any 25d cursors, draw them with radius proportional to the z value
		vector<tuio::Cursor25d> cursors25d = tuio.getCursors25d();
		for( auto cursor25d = cursors25d.begin(); cursor25d != cursors25d.end(); ++cursor25d ) {
			draw25d( *cursor25d );
		}
	}
	else
		gl::clear( Color( 0.4f, 0, 0 ) );
}

CINDER_APP_NATIVE( TuioClientApp, RendererGl )