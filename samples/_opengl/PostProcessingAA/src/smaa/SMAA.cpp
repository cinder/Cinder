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

#include "cinder/app/App.h"
#include "cinder/Log.h"

#include "SMAA.h"
#include "AreaTex.h"
#include "SearchTex.h"

using namespace ci;
using namespace std;

SMAA::SMAA()
{
	// Load and compile our shaders
	try {
		mGlslFirstPass = gl::GlslProg::create( app::loadAsset( "smaa1.vert" ), app::loadAsset( "smaa1.frag" ) );
		mGlslSecondPass = gl::GlslProg::create( app::loadAsset( "smaa2.vert" ), app::loadAsset( "smaa2.frag" ) );
		mGlslThirdPass = gl::GlslProg::create( app::loadAsset( "smaa3.vert" ), app::loadAsset( "smaa3.frag" ) );
	}
	catch( const std::exception& e ) {
		CI_LOG_EXCEPTION( "exception caught loading smaa shaders", e );
	}

	// Create lookup textures
	gl::Texture2d::Format fmt;
	fmt.setMinFilter( GL_LINEAR );
	fmt.setMagFilter( GL_LINEAR );
	fmt.setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );

	// Search Texture (Grayscale, 8 bits unsigned).
	fmt.setInternalFormat( GL_R8 );
	fmt.setSwizzleMask( GL_RED, GL_RED, GL_RED, GL_ONE );
	fmt.loadTopDown( true );
	mSearchTex = gl::Texture2d::create( searchTexBytes, GL_RED, SEARCHTEX_WIDTH, SEARCHTEX_HEIGHT, fmt );

	// Area Texture (Red+Green Channels, 8 bits unsigned).
	fmt.setInternalFormat( GL_RG8 );
	fmt.setSwizzleMask( GL_RED, GL_GREEN, GL_ZERO, GL_ONE );
	fmt.loadTopDown( true );
	mAreaTex = gl::Texture2d::create( areaTexBytes, GL_RG, AREATEX_WIDTH, AREATEX_HEIGHT, fmt );

	// Specify the Fbo format.
	gl::Texture2d::Format tfmt;
	tfmt.setMinFilter( GL_LINEAR );
	tfmt.setMagFilter( GL_LINEAR );
	tfmt.setInternalFormat( GL_RGBA8 );

	mFboFormat.setColorTextureFormat( tfmt );
	mFboFormat.enableDepthBuffer( false );
}

void SMAA::apply( const gl::FboRef &destination, const gl::FboRef &source )
{
	gl::ScopedFramebuffer fbo( destination );
	gl::ScopedViewport viewport( 0, 0, destination->getWidth(), destination->getHeight() );
	gl::ScopedMatrices matrices;
	gl::setMatricesWindow( destination->getSize() );

	// Make sure our source is linearly interpolated.
	GLenum minFilter = source->getFormat().getColorTextureFormat().getMinFilter();
	GLenum magFilter = source->getFormat().getColorTextureFormat().getMagFilter();
	bool filterChanged = ( minFilter != GL_LINEAR || magFilter != GL_LINEAR );
	if( filterChanged ) {
		source->getColorTexture()->setMinFilter( GL_LINEAR );
		source->getColorTexture()->setMagFilter( GL_LINEAR );
	}

	// Perform SMAA anti-aliasing.
	gl::clear( ColorA( 0, 0, 0, 0 ) );
	draw( source->getColorTexture(), destination->getBounds() );

	// Restore texture parameters.
	if( filterChanged ) {
		source->getColorTexture()->setMinFilter( minFilter );
		source->getColorTexture()->setMagFilter( magFilter );
	}
}

void SMAA::draw( const gl::Texture2dRef &source, const Area &bounds )
{
	if( ! mGlslFirstPass || ! mGlslSecondPass || ! mGlslThirdPass )
		return;

	// Create or resize buffers.
	const int width = source->getWidth();
	const int height = source->getHeight();
	createBuffers( width, height );

	// Apply first two passes.
	doEdgePass( source );
	doBlendPass();

	// Apply SMAA.
	gl::ScopedTextureBind tex0( source );
	gl::ScopedTextureBind tex1( mFboBlendPass->getColorTexture(), 1 );
	gl::ScopedGlslProg glslScope( mGlslThirdPass );
	mGlslThirdPass->uniform( "SMAA_RT_METRICS", mMetrics );
	mGlslThirdPass->uniform( "uColorTex", 0 );
	mGlslThirdPass->uniform( "uBlendTex", 1 );

	gl::color( Color::white() );
	gl::drawSolidRect( bounds );

	//	gl::draw( mAreaTex );
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
		mFboEdgePass = gl::Fbo::create( width, height, mFboFormat );
	}

	if( !mFboBlendPass || mFboBlendPass->getWidth() != width || mFboBlendPass->getHeight() != height ) {
		mFboBlendPass = gl::Fbo::create( width, height, mFboFormat );
	}

	mMetrics = vec4( 1.0f / width, 1.0f / height, (float) width, (float) height );
}

void SMAA::doEdgePass( const gl::Texture2dRef &source )
{
	// Enable frame buffer, bind textures and shader.
	gl::ScopedFramebuffer fbo( mFboEdgePass );
	gl::clear( ColorA( 0, 0, 0, 0 ) );

	gl::ScopedTextureBind tex0( source );
	gl::ScopedGlslProg glslScope( mGlslFirstPass );
	mGlslFirstPass->uniform( "SMAA_RT_METRICS", mMetrics );
	mGlslFirstPass->uniform( "uColorTex", 0 );

	// Execute shader by drawing a 'full screen' rectangle.
	gl::color( Color::white() );
	gl::drawSolidRect( mFboEdgePass->getBounds() );
}

void SMAA::doBlendPass()
{
	// Enable frame buffer, bind textures and shader.
	gl::ScopedFramebuffer fbo( mFboBlendPass );
	gl::clear( ColorA( 0, 0, 0, 0 ) );

	gl::ScopedTextureBind tex0( mFboEdgePass->getColorTexture() );
	gl::ScopedTextureBind tex1( mAreaTex, 1 );
	gl::ScopedTextureBind tex2( mSearchTex, 2 );
	gl::ScopedGlslProg glslScope( mGlslSecondPass );
	mGlslSecondPass->uniform( "SMAA_RT_METRICS", mMetrics );
	mGlslSecondPass->uniform( "uEdgesTex", 0 );
	mGlslSecondPass->uniform( "uAreaTex", 1 );
	mGlslSecondPass->uniform( "uSearchTex", 2 );

	// Execute shader by drawing a 'full screen' rectangle.
	gl::color( Color::white() );
	gl::drawSolidRect( mFboBlendPass->getBounds() );
}