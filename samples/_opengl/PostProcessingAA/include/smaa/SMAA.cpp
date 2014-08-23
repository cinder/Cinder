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

#include "cinder/Log.h"

#include "SMAA.h"
#include "AreaTex.h"
#include "SearchTex.h"

using namespace ci;
using namespace std;

void SMAA::setup()
{
	// Load and compile our shaders
	try {
		mSMAAFirstPass = Shader::create( "smaa1" );
		mSMAASecondPass = Shader::create( "smaa2" );
		mSMAAThirdPass = Shader::create( "smaa3" );
	}
	catch( const std::exception& e ) {
		CI_LOG_E( e.what() );
	}

	// Create lookup textures
	gl::Texture2d::Format fmt;
	fmt.setMinFilter( GL_LINEAR );
	fmt.setMagFilter( GL_LINEAR );
	fmt.setWrap( GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER );

	// Search Texture (Grayscale, 8 bits unsigned)
	fmt.setInternalFormat( GL_R8 );
	fmt.setSwizzleMask( GL_RED, GL_RED, GL_RED, GL_ONE );
	mSearchTex = gl::Texture2d::create( searchTexBytes, GL_RED, SEARCHTEX_WIDTH, SEARCHTEX_HEIGHT, fmt );

	// Area Texture (Red+Green Channels, 8 bits unsigned)
	fmt.setInternalFormat( GL_RG8 );
	fmt.setSwizzleMask( GL_RED, GL_GREEN, GL_ZERO, GL_ONE );
	mAreaTex = gl::Texture2d::create( areaTexBytes, GL_RG, AREATEX_WIDTH, AREATEX_HEIGHT, fmt );

	// Specify the Fbo format.
	gl::Texture2d::Format tfmt;
	tfmt.setMinFilter( GL_LINEAR );
	tfmt.setMagFilter( GL_LINEAR );

	mFboFormat.setColorTextureFormat( tfmt );
	mFboFormat.enableDepthBuffer( false );
}

void SMAA::draw( ci::gl::Texture2dRef source, const Area& bounds )
{
	if( !mSMAAFirstPass || !mSMAASecondPass || !mSMAAThirdPass )
		return;

	const int w = source->getWidth();
	const int h = source->getHeight();
	createBuffers( w, h );

	// Apply first two passes
	doEdgePass( source );
	doBlendPass();

	// Apply SMAA
	gl::ScopedTextureBind tex1( ( gl::TextureBaseRef ) mFboBlendPass->getColorTexture(), 1 );

	gl::ScopedGlslProg shader( mSMAAThirdPass->program() );
	mSMAAThirdPass->uniform( "uColorTex", 0 );
	mSMAAThirdPass->uniform( "uBlendTex", 1 );
	mSMAAThirdPass->uniform( "SMAA_RT_METRICS", Vec4f( 1.0f / w, 1.0f / h, (float) w, (float) h ) );

	gl::ScopedTextureBind tex0( source );

	gl::color( Color::white() );
	gl::drawSolidRect( bounds );
}

gl::TextureRef SMAA::getEdgePass()
{
	return mFboEdgePass->getColorTexture();
}

gl::TextureRef SMAA::getBlendPass()
{
	return mFboBlendPass->getColorTexture();
}

void SMAA::createBuffers( int width, int height )
{
	// Create or resize frame buffers
	if( !mFboEdgePass || mFboEdgePass->getWidth() != width || mFboEdgePass->getHeight() != height ) {
		mFboFormat.setColorBufferInternalFormat( GL_RG );
		mFboEdgePass = gl::Fbo::create( width, height, mFboFormat );
	}

	if( !mFboBlendPass || mFboBlendPass->getWidth() != width || mFboBlendPass->getHeight() != height ) {
		mFboFormat.setColorBufferInternalFormat( GL_RGBA );
		mFboBlendPass = gl::Fbo::create( width, height, mFboFormat );
	}
}

void SMAA::doEdgePass( ci::gl::Texture2dRef source )
{
	const int w = mFboEdgePass->getWidth();
	const int h = mFboEdgePass->getHeight();

	// Enable frame buffer
	gl::ScopedFramebuffer fbo( mFboEdgePass );
	//gl::pushMatrices();
	//gl::setMatricesWindow( mFboEdgePass->getSize(), false );
	//gl::ScopedViewport viewport( 0, 0, mFboEdgePass->getWidth(), mFboEdgePass->getHeight() );

	gl::ScopedGlslProg shader( mSMAAFirstPass->program() );
	mSMAAFirstPass->uniform( "uColorTex", 0 );
	mSMAAFirstPass->uniform( "SMAA_RT_METRICS", Vec4f( 1.0f / w, 1.0f / h, (float) w, (float) h ) );

	gl::clear();
	gl::color( Color::white() );

	gl::ScopedTextureBind tex0( source );
	gl::drawSolidRect( mFboEdgePass->getBounds() );

	//gl::popMatrices();
}

void SMAA::doBlendPass()
{
	const int w = mFboBlendPass->getWidth();
	const int h = mFboBlendPass->getHeight();

	// Enable frame buffer
	gl::ScopedFramebuffer fbo( mFboBlendPass );
	//gl::pushMatrices();
	//gl::setMatricesWindow( mFboBlendPass->getSize(), false );
	//gl::ScopedViewport viewport( 0, 0, mFboBlendPass->getWidth(), mFboBlendPass->getHeight() );

	gl::ScopedTextureBind tex1( ( gl::TextureBaseRef ) mAreaTex, 1 );
	gl::ScopedTextureBind tex2( ( gl::TextureBaseRef ) mSearchTex, 2 );

	gl::ScopedGlslProg shader( mSMAASecondPass->program() );
	mSMAASecondPass->uniform( "uEdgesTex", 0 );
	mSMAASecondPass->uniform( "uAreaTex", 1 );
	mSMAASecondPass->uniform( "uSearchTex", 2 );
	mSMAASecondPass->uniform( "SMAA_RT_METRICS", Vec4f( 1.0f / w, 1.0f / h, (float) w, (float) h ) );

	gl::clear();
	gl::color( Color::white() );

	gl::ScopedTextureBind tex0( mFboEdgePass->getColorTexture() );
	gl::drawSolidRect( mFboBlendPass->getBounds() );

	//gl::popMatrices();
}