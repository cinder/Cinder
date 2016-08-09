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

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#include "cinder/tuio/Tuio.h"
#include "cinder/Log.h"

class TuioClientApp : public App {
  public:
	void setup() override;
	void draw() override;
	
	void added( const tuio::Cursor2d &cursor );
	void updated( const tuio::Cursor2d &cursor );
	void removed( const tuio::Cursor2d &cursor );
	
	std::shared_ptr<tuio::Receiver> tuio;
	std::map<std::string, std::vector<vec2>> mTouches;
};

void TuioClientApp::setup()
{
	tuio = std::shared_ptr<tuio::Receiver>( new tuio::Receiver() );
	tuio->connect();
	
	tuio->setAddedFn<tuio::Cursor2d>( std::bind( &TuioClientApp::added, this, std::placeholders::_1 ) );
	tuio->setUpdatedFn<tuio::Cursor2d>( std::bind( &TuioClientApp::updated, this, std::placeholders::_1 ) );
	tuio->setRemovedFn<tuio::Cursor2d>( std::bind( &TuioClientApp::removed, this, std::placeholders::_1 ) );
}

void TuioClientApp::added( const tuio::Cursor2d &cursor )
{
	auto windowPos = vec2( getWindowSize() ) * cursor.getPosition();
	mTouches.insert( { cursor.getSource() + std::to_string(cursor.getSessionId()), std::vector<vec2>( { windowPos } ) } );
}

void TuioClientApp::updated( const tuio::Cursor2d &cursor )
{
	auto windowPos = vec2( getWindowSize() ) * cursor.getPosition();
	auto source = cursor.getSource() + std::to_string(cursor.getSessionId());
	mTouches[source].push_back( windowPos );
}

void TuioClientApp::removed( const tuio::Cursor2d &cursor )
{
	auto source = cursor.getSource() + std::to_string(cursor.getSessionId());
	mTouches.erase( source );
}

void TuioClientApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatricesWindow( getWindowSize() );
	
	for( auto & touch : mTouches ) {
		gl::begin( GL_POINTS );
		for( auto & pos : touch.second ) {
			gl::color( ColorA( 1, 1, 1, 1 ) );
			gl::vertex( pos );
		}
		gl::end();
		gl::drawStrokedCircle( touch.second.back(), 20 );
	}
}

CINDER_APP( TuioClientApp, RendererGl )