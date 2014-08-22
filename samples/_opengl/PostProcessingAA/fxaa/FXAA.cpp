/*
 Copyright (c) 2014, Paul Houx - All rights reserved.
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

#include "cinder/app/AppBasic.h"

#include "FXAA.h"

using namespace ci;
using namespace ci::app;
using namespace std;

void FXAA::setup()
{
	// Load and compile our shader
	try {
		mFXAA = gl::GlslProg::create( loadAsset( "fxaa.vert" ), loadAsset( "fxaa.frag" ) );
	}
	catch( const std::exception& e ) {
		console() << e.what() << std::endl;
	}
}

void FXAA::apply(ci::gl::Fbo& destination, ci::gl::Fbo& source)
{
	if( !mFXAA )
		return;

	// Source and destination should have the same size
	assert(destination.getWidth() == source.getWidth());
	assert(destination.getHeight() == source.getHeight());

	// Apply FXAA
	destination.bindFramebuffer();

	mFXAA->bind();
	mFXAA->uniform("uTexture", 0);
	mFXAA->uniform("uRcpBufferSize", Vec2f::one() / Vec2f( destination.getSize() ));
	{
		gl::clear();
		gl::color( Color::white() );

		gl::draw( source.getTexture(0), destination.getBounds() );
	}
	
	destination.unbindFramebuffer();
}