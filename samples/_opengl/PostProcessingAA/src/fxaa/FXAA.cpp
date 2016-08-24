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

#include "FXAA.h"

using namespace ci;
using namespace std;

FXAA::FXAA()
{
	try {
		auto glsl = gl::GlslProg::create( app::loadAsset( "fxaa.vert" ), app::loadAsset( "fxaa.frag" ) );
		glsl->uniform( "uTexture", 0 );

		mBatch = gl::Batch::create( geom::Rect( Rectf( 0, 0, 1, 1 ) ), glsl );
	}
	catch( const std::exception &e ) {
		CI_LOG_EXCEPTION( "exception caught loading fxaa.vert / frag", e );
	}
}

void FXAA::apply( const gl::FboRef &destination, const gl::FboRef &source )
{
	gl::ScopedFramebuffer fbo( destination );
	gl::ScopedViewport viewport( 0, 0, destination->getWidth(), destination->getHeight() );
	gl::ScopedMatrices matrices;
	gl::setMatricesWindow( destination->getSize(), false );

	// Make sure our source is linearly interpolated.
	GLenum minFilter = source->getFormat().getColorTextureFormat().getMinFilter();
	GLenum magFilter = source->getFormat().getColorTextureFormat().getMagFilter();
	source->getColorTexture()->setMinFilter( GL_LINEAR );
	source->getColorTexture()->setMagFilter( GL_LINEAR );

	// Perform FXAA anti-aliasing.
	gl::clear( ColorA( 0, 0, 0, 0 ) );
	draw( source->getColorTexture(), destination->getBounds() );

	// Restore texture parameters.
	source->getColorTexture()->setMinFilter( minFilter );
	source->getColorTexture()->setMagFilter( magFilter );
}

void FXAA::draw( const gl::TextureRef &source, const Area &bounds )
{
	if( ! mBatch )
		return;

	const float w = (float)bounds.getWidth();
	const float h = (float)bounds.getHeight();

	mBatch->getGlslProg()->uniform( "uExtents", vec4( 1.0f / w, 1.0f / h, w, h ) );

	gl::ScopedTextureBind tex0( source );
	gl::ScopedModelMatrix modelScope;
	gl::scale( w, h, 1.0f );

	mBatch->draw();
}