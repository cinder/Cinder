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

#include "cinder/app/AppBasic.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#include "TuioClient.h"
#include "TuioCursor.h"
#include "OscMessage.h"

class TuioClientApp : public AppBasic {
 public:
	void setup();
	void update();
	void draw();
	
	void cursorAdded( tuio::Cursor cursor );
	void cursorUpdated( tuio::Cursor cursor );
	void cursorRemoved( tuio::Cursor cursor );
	void oscMessage( const osc::Message *message );

	tuio::Client tuio;

	CallbackId	mUpdatedCallbackIndex;
};

void draw2( tuio::Cursor cursor )
{	
	gl::color(ColorA(1.0f, 1.0f, 1.0f, 0.6f));
	gl::drawSolidCircle( cursor.getPos() * Vec2f( app::getWindowSize() ), 30 );
}

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
	//console() << "OSC Message " << message->getAddress() << std::endl;
}

void TuioClientApp::update()
{
	if( getElapsedSeconds() > 10.0f && mUpdatedCallbackIndex > -1 ) {
		console() << "Unregistering cursor updated." << std::endl;
		tuio.unregisterCursorUpdated( mUpdatedCallbackIndex );
		mUpdatedCallbackIndex = -1;
	}
}

void TuioClientApp::draw()
{
	gl::enableDepthRead();
	gl::enableDepthWrite();	
	gl::enableAlphaBlending();
	gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
	
	gl::setMatricesWindow( getWindowSize() );
	if( tuio.isConnected() ) {
		vector<tuio::Cursor> cursors = tuio.getCursors();
		for( vector<tuio::Cursor>::const_iterator cursor = cursors.begin(); cursor != cursors.end(); ++cursor )
			draw2( *cursor );
	}
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( TuioClientApp, RendererGl )
