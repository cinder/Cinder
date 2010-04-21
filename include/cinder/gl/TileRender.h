/*
 Copyright (c) 2010, The Barbarian Group
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

#pragma once

#include "cinder/Cinder.h"
#include "cinder/gl/gl.h"
#include "cinder/Exception.h"
#include "cinder/Surface.h"
#include "cinder/Camera.h"

namespace cinder { namespace gl {

class TileRender {
  public:
	TileRender( int32_t imageWidth, int32_t imageHeight, int32_t tileWidth = 512, int32_t tileHeight = 512 );
	
	bool		nextTile();
	
	int32_t		getImageWidth() const { return mImageWidth; }
	int32_t		getImageHeight() const { return mImageHeight; }
	float		getImageAspectRatio() const { return mImageWidth / (float)mImageHeight; }
	Area		getCurrentTileArea() const { return mCurrentArea; }
	Surface		getSurface() const { return mSurface; }
	
	void		setMatricesWindow( int32_t windowWidth, int32_t windowHeight );
	void		setMatrices( const Camera &camera );

	void		frustum( float left, float right, float bottom, float top, float nearPlane, float farPlane );
	void		ortho( float left, float right, float bottom, float top, float nearPlane, float farPlane );
	
  protected:
	void		updateFrustum();

	int32_t		mImageWidth, mImageHeight;
	int32_t		mTileWidth, mTileHeight;
	int32_t		mNumTilesX, mNumTilesY;

	int32_t		mCurrentTile;
	Area		mCurrentArea;
	Rectf		mCurrentFrustumCoords;
	float		mCurrentFrustumNear, mCurrentFrustumFar;
	bool		mCurrentFrustumPersp;
	
	Area		mSavedViewport;
	Surface		mSurface;
};

} } // namespace cinder::gl