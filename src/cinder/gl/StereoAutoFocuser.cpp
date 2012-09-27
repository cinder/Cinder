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
#include "cinder/gl/StereoAutoFocuser.h"

namespace cinder { namespace gl {

void StereoAutoFocuser::autoFocus( CameraStereo *cam, const Area &area, GLuint buffer )
{
	if( ! cam->isStereoEnabled() )
		return;

	// create or resize buffers
	createBuffers( area );

	// blit (multi-sampled) depth buffer to (non-multi-sampled) auto focus fbo	
	// (they need to be the same size for this to work!)
	{
		SaveFramebufferBinding saveFboBinding;

		Area dstArea = mFboLarge.getBounds();

		glBindFramebufferEXT( GL_READ_FRAMEBUFFER_EXT, buffer );
		glBindFramebufferEXT( GL_DRAW_FRAMEBUFFER_EXT, mFboLarge.getId() );	
		glBlitFramebufferEXT( area.getX1(), area.getY1(), area.getX2(), area.getY2(), 
			dstArea.getX1(), dstArea.getY1(), dstArea.getX2(), dstArea.getY2(), GL_DEPTH_BUFFER_BIT, GL_NEAREST );
	}

	// create a downsampled copy for the auto focus test
	mFboLarge.blitTo( mFboSmall, mFboLarge.getBounds(), mFboSmall.getBounds(),
		GL_NEAREST, GL_DEPTH_BUFFER_BIT );

	// load depth samples into buffer
	glBindFramebufferEXT( GL_READ_FRAMEBUFFER_EXT, mFboSmall.getId() );
	glReadPixels(0, 0, AF_WIDTH, AF_HEIGHT, GL_DEPTH_COMPONENT, GL_FLOAT, &mBuffer.front());
	mFboSmall.unbindFramebuffer();

	// find minimum value 
	std::vector<GLfloat>::const_iterator itr = std::min_element(mBuffer.begin(), mBuffer.end());

	size_t p = itr - mBuffer.begin();
	mNearest.x = 0.5f + (int) ( (p % AF_WIDTH) / (float) AF_WIDTH * mArea.getWidth() );
	mNearest.y = 0.5f + (int) ( (p / AF_WIDTH) / (float) AF_HEIGHT * mArea.getHeight() );
	
	// convert to actual distance from camera
	float nearClip = cam->getNearClip();
	float farClip = cam->getFarClip();
	float depth = 2.0f * (*itr) - 1.0f;
	mNearest.z = 2.0f * nearClip * farClip / ( farClip + nearClip - depth * ( farClip - nearClip ) );
	
	// perform auto focussing
	float z = math<float>::max( nearClip, mNearest.z * mDepth );
	cam->setConvergence( cam->getConvergence() + mSpeed * ( z - cam->getConvergence() ), true );
}

void StereoAutoFocuser::draw()
{
	// visual debugging 
	glPushAttrib( GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_LINE_BIT );
	gl::enableAlphaBlending();
	gl::color( ColorA( 0, 1, 1, 0.1f ) );
	gl::drawSolidRect( mArea );
	gl::color( ColorA(0, 1, 1, 0.8f) );
	glLineWidth( 2.0f );
	gl::drawLine( Vec2f( (float) mArea.getX1() + 0.5f, (float) mArea.getY2() - mNearest.y ), 
		Vec2f( (float) mArea.getX2() + 0.5f, (float) mArea.getY2() - mNearest.y ) );
	gl::drawLine( Vec2f( (float) mArea.getX1() + mNearest.x, (float) mArea.getY1() + 0.5f ), 
		Vec2f( (float) mArea.getX1() + mNearest.x, (float) mArea.getY2() + 0.5f ) );
	glPopAttrib();
}

void StereoAutoFocuser::createBuffers( const Area &area )
{
	mArea = area;

	int width = mArea.getWidth();
	int height = mArea.getHeight();

	if( !mFboLarge || !mFboSmall || mFboLarge.getWidth() != width || mFboLarge.getHeight() != height )
	{
		gl::Fbo::Format fmt;
		fmt.enableColorBuffer(false);
		fmt.enableDepthBuffer(true, false);

		mFboLarge = gl::Fbo( width, height, fmt );
		mFboSmall = gl::Fbo( AF_WIDTH, AF_HEIGHT, fmt );
	}

	int size = AF_WIDTH * AF_HEIGHT;
	if( mBuffer.size() != size )
	{
		mBuffer.resize( size );
		mBuffer.assign( size, 0.0f );
	}
}

} } // namespace cinder::gl