/*
 Copyright (c) 2010-2012, Paul Houx - All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

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

#include "cinder/Quaternion.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class TestQuatApp : public AppBasic {
public:
	void prepareSettings( Settings *settings );
	
	void setup();
	void update();
	void draw();
	
	void resize( ResizeEvent event );
	
	void mouseMove( MouseEvent event );	
	void mouseDown( MouseEvent event );	
	void mouseDrag( MouseEvent event );	
	void mouseUp( MouseEvent event );	
	
	void keyDown( KeyEvent event );
	void keyUp( KeyEvent event );
private:
	float	mRadians;
	Quatf	mQuaternion;
};

void TestQuatApp::prepareSettings(Settings *settings)
{
	settings->setTitle("Cinder Sample");
}

void TestQuatApp::setup()
{
	mRadians = 0.0f;
	mQuaternion.set( Vec3f::zAxis(), mRadians );
}

void TestQuatApp::update()
{
	// wait until both the yellow (float) and red (quaternion) circles
	// have gone round once, then notice how the red one simply stops,
	// unless the new code is used.

	mRadians += 0.02f;

	// the following line is where things don't work as expected:
	mQuaternion.set( Vec3f::zAxis(), mQuaternion.getAngle() + 0.02f );
}

void TestQuatApp::draw()
{
	gl::clear(); 

	gl::color( Color(0.5f, 0.5f, 0.5f) );
	gl::drawStrokedCircle( 0.5f * getWindowSize(), 100.0f, 120 );
	gl::drawStrokedCircle( 0.5f * getWindowSize(), 120.0f, 120 );

	gl::color( Color(1, 0.75f, 0) );
	float x = 0.5f * getWindowWidth() + 100.0f * math<float>::sin(mRadians);
	float y = 0.5f * getWindowHeight() - 100.0f * math<float>::cos(mRadians);
	gl::drawSolidCircle( Vec2f(x, y), 4.0f );
	
	gl::color( Color(1, 0, 0) );
	x = 0.5f * getWindowWidth() + 120.0f * math<float>::sin(mQuaternion.getAngle());
	y = 0.5f * getWindowHeight() - 120.0f * math<float>::cos(mQuaternion.getAngle());
	gl::drawSolidCircle( Vec2f(x, y), 4.0f );
}

void TestQuatApp::resize( ResizeEvent event )
{
}

void TestQuatApp::mouseMove( MouseEvent event )
{
}

void TestQuatApp::mouseDown( MouseEvent event )
{
}

void TestQuatApp::mouseDrag( MouseEvent event )
{
}

void TestQuatApp::mouseUp( MouseEvent event )
{
}

void TestQuatApp::keyDown( KeyEvent event )
{
	setup();
}

void TestQuatApp::keyUp( KeyEvent event )
{
}

CINDER_APP_BASIC( TestQuatApp, RendererGl )
