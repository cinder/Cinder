/*
 Copyright (c) 2012, The Cinder Project, All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org
 
 Portions of this code (C) Paul Houx
 All rights reserved.

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

#include "cinder/Camera.h"
#include "cinder/gl/CameraStereoAutoFocuser.h"

namespace cinder { namespace gl {
	
void CameraStereoAutoFocuser::autoFocus( CameraStereo &cam, bool useDepthBuffer )
{
	if( ! cam.isStereoEnabled() )
		return;

	if( !useDepthBuffer )
	{
		// perform naive auto focussing based on distance from center of interest
		float z = math<float>::max( cam.getNearClip(), cam.getEyePoint().length() * mAutoFocusDepth );
		cam.setFocus( cam.getFocalLength() + mAutoFocusSpeed * ( z - cam.getFocalLength() ) );
		
		return;
	}

	// determine sample area
	Area area = getAutoFocusArea();

	// create or resize buffers
	createBuffers( area );

	// blit (multi-sampled) main depth buffer to (non-multi-sampled) auto focus fbo	(they need to be the same size for this to work!)
	mAutoFocusFboLarge.blitFromScreen( area, mAutoFocusFboLarge.getBounds(), 
		GL_NEAREST, GL_DEPTH_BUFFER_BIT );

	// create a downsampled copy for the auto focus test
	mAutoFocusFboLarge.blitTo(mAutoFocusFboSmall, mAutoFocusFboLarge.getBounds(), mAutoFocusFboSmall.getBounds(),
		GL_NEAREST, GL_DEPTH_BUFFER_BIT );

	// load depth samples into buffer
	glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, mAutoFocusFboSmall.getId());
	glReadPixels(0, 0, AF_WIDTH, AF_HEIGHT, GL_DEPTH_COMPONENT, GL_FLOAT, &mAutoFocusBuffer.front());
	mAutoFocusFboSmall.unbindFramebuffer();

	// find minimum value 
	std::vector<GLfloat>::const_iterator itr = std::min_element(mAutoFocusBuffer.begin(), mAutoFocusBuffer.end());

	size_t p = itr - mAutoFocusBuffer.begin();
	mX = 0.5f + (int) ( (p % AF_WIDTH) / (float) AF_WIDTH * area.getWidth() );
	mY = 0.5f + (int) ( (p / AF_WIDTH) / (float) AF_HEIGHT * area.getHeight() );
	
	// convert to actual distance from camera
	float nearClip = cam.getNearClip();
	float farClip = cam.getFarClip();
	float depth = 2.0f * (*itr) - 1.0f;
	depth = 2.0f * nearClip * farClip / ( farClip + nearClip - depth * ( farClip - nearClip ) );
	
	// perform auto focussing
	float z = math<float>::max( nearClip, depth * mAutoFocusDepth );
	cam.setFocus( cam.getFocalLength() + mAutoFocusSpeed * ( z - cam.getFocalLength() ) );
}

inline Area	CameraStereoAutoFocuser::getAutoFocusArea() const
{
	Area area = gl::getViewport();
	area.expand( -area.getWidth() / 4, 0 );

	return area;
}

void CameraStereoAutoFocuser::draw()
{
	// visual debugging 
	Area area = getAutoFocusArea();

	glPushAttrib( GL_CURRENT_BIT | GL_LINE_BIT );
	glLineWidth( 1.0f );
	gl::color( Color(1, 1, 0) );
	gl::drawLine( Vec2f( (float) area.getX1() + 0.5f, (float) area.getY2() - mY ), Vec2f( (float) area.getX2() + 0.5f, (float) area.getY2() - mY ) );
	gl::drawLine( Vec2f( (float) area.getX1() + mX, (float) area.getY1() + 0.5f ), Vec2f( (float) area.getX1() + mX, (float) area.getY2() + 0.5f ) );
	glPopAttrib();
}

void CameraStereoAutoFocuser::createBuffers( const Area &area )
{
	int width = area.getWidth();
	int height = area.getHeight();

	if( !mAutoFocusFboLarge || !mAutoFocusFboSmall || mAutoFocusFboLarge.getWidth() != width || mAutoFocusFboLarge.getHeight() != height )
	{
		gl::Fbo::Format fmt;
		fmt.enableColorBuffer(false);
		fmt.enableDepthBuffer(true, false);

		mAutoFocusFboLarge = gl::Fbo( width, height, fmt );
		mAutoFocusFboSmall = gl::Fbo( AF_WIDTH, AF_HEIGHT, fmt );
	}

	int size = AF_WIDTH * AF_HEIGHT;
	if( mAutoFocusBuffer.size() != size )
	{
		mAutoFocusBuffer.resize( size );
		mAutoFocusBuffer.assign( size, 0.0f );
	}
}

void CameraStereoAutoFocuser::destroyBuffers()
{
	// clear buffer and deallocate memory
	mAutoFocusBuffer.swap( std::vector<GLfloat>() );

	mAutoFocusFboSmall = gl::Fbo();
	mAutoFocusFboLarge = gl::Fbo();
}

} } // namespace gl::cinder
