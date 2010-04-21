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

#include "cinder/gl/TileRender.h"
#include "cinder/gl/gl.h"
#include "cinder/app/App.h"

#include <algorithm>
#include <sstream>

namespace cinder { namespace gl {

TileRender::TileRender( int32_t imageWidth, int32_t imageHeight, int32_t tileWidth, int32_t tileHeight )
	: mImageWidth( imageWidth ), mImageHeight( imageHeight )
{
	// if we are using the screen, we can't make tiles bigger than the app's window
	mTileWidth = std::min( tileWidth, (int32_t)app::getWindowWidth() );
	mTileHeight = std::min( tileHeight, (int32_t)app::getWindowHeight() ); 

	mNumTilesX = (int32_t)math<float>::ceil( mImageWidth / (float)mTileWidth );
	mNumTilesY = (int32_t)math<float>::ceil( mImageHeight / (float)mTileHeight );
	
	mCurrentTile = -1;
}

bool TileRender::nextTile()
{
	if( mCurrentTile >= mNumTilesX * mNumTilesY ) {
		// suck the pixels out of the final tile
		mSurface.copyFrom( app::copyWindowSurface( Area( 0, app::getWindowHeight() - mCurrentArea.getHeight(), mCurrentArea.getWidth(), app::getWindowHeight() ) ), 
			Area( 0, 0, mCurrentArea.getWidth(), mCurrentArea.getHeight() ), mCurrentArea.getUL() );
		// all done
		gl::setViewport( mSavedViewport );
		mCurrentTile = -1;
		return false;
	}
	
	if( mCurrentTile == -1 ) { // first tile of this frame
		mSavedViewport = gl::getViewport();
		mCurrentTile = 0;
		mSurface = Surface( mImageWidth, mImageHeight, false );
	}
	else {
		// suck the pixels out of the previous tile
		mSurface.copyFrom( app::copyWindowSurface( Area( 0, app::getWindowHeight() - mCurrentArea.getHeight(), mCurrentArea.getWidth(), app::getWindowHeight() ) ), 
			Area( 0, 0, mCurrentArea.getWidth(), mCurrentArea.getHeight() ), mCurrentArea.getUL() );
	}
	
	int tileX = mCurrentTile % mNumTilesX;
	int tileY = mCurrentTile / mNumTilesX;
	int currentTileWidth = ( ( tileX == mNumTilesX - 1 ) && ( mImageWidth != mTileWidth * mNumTilesX ) ) ? ( mImageWidth % mTileWidth ) : mTileWidth;
	int currentTileHeight = ( ( tileY == mNumTilesY - 1 ) && ( mImageHeight != mTileHeight * mNumTilesY ) ) ? ( mImageHeight % mTileHeight ) : mTileHeight;
	mCurrentArea.x1 = tileX * mTileWidth;
	mCurrentArea.x2 = mCurrentArea.x1 + currentTileWidth;
	mCurrentArea.y1 = tileY * mTileHeight;
	mCurrentArea.y2 = mCurrentArea.y1 + currentTileHeight;

	gl::setViewport( Area( 0, 0, mCurrentArea.getWidth(), mCurrentArea.getHeight() ) );
	updateFrustum();

	mCurrentTile++;
	return true;
}

void TileRender::setMatricesWindow( int32_t windowWidth, int32_t windowHeight )
{
	ortho( 0, (float)windowWidth, (float)windowHeight, 0, -1.0f, 1.0f );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}

void TileRender::setMatrices( const cinder::Camera &camera )
{
	float left, top, right, bottom, nearPlane, farPlane;
	camera.getFrustum( &left, &top, &right, &bottom, &nearPlane, &farPlane );

	if( camera.isPersp() ) {
		frustum( left, right, bottom, top, nearPlane, farPlane );
	}
	else {
		ortho( left, right, bottom, top, nearPlane, farPlane );
	}

	glMatrixMode( GL_MODELVIEW );
	glLoadMatrixf( camera.getModelViewMatrix().m );
}

void TileRender::frustum( float left, float right, float bottom, float top, float nearPlane, float farPlane )
{
	mCurrentFrustumCoords = Rectf( Vec2f( left, top ), Vec2f( right, bottom ) );
	mCurrentFrustumNear = nearPlane;
	mCurrentFrustumFar = farPlane;
	mCurrentFrustumPersp = true;
	updateFrustum();
}

void TileRender::ortho( float left, float right, float bottom, float top, float nearPlane, float farPlane )
{
	mCurrentFrustumCoords = Rectf( Vec2f( left, top ), Vec2f( right, bottom ) );
	mCurrentFrustumNear = nearPlane;
	mCurrentFrustumFar = farPlane;
	mCurrentFrustumPersp = false;
	updateFrustum();
}

void TileRender::updateFrustum()
{
	float left = mCurrentFrustumCoords.x1 + mCurrentArea.x1 / (float)mImageWidth * mCurrentFrustumCoords.getWidth();
	float right = left + mCurrentArea.getWidth() / (float)mImageWidth * mCurrentFrustumCoords.getWidth();
	float top = mCurrentFrustumCoords.y1 + mCurrentArea.y1 / (float)mImageHeight * mCurrentFrustumCoords.getHeight();
	float bottom = top + mCurrentArea.getHeight() / (float)mImageHeight * mCurrentFrustumCoords.getHeight();

	int matrixMode;
	glGetIntegerv( GL_MATRIX_MODE, &matrixMode );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	if( mCurrentFrustumPersp ) {
#if defined( CINDER_GLES )
		glFrustumf( left, right, bottom, top, mCurrentFrustumNear, mCurrentFrustumFar );
#else
		glFrustum( left, right, bottom, top, mCurrentFrustumNear, mCurrentFrustumFar );
#endif
	}
	else
#if defined( CINDER_GLES )
		glOrthof( left, right, bottom, top, mCurrentFrustumNear, mCurrentFrustumFar );
#else
		glOrtho( left, right, bottom, top, mCurrentFrustumNear, mCurrentFrustumFar );
#endif
	
	glMatrixMode( matrixMode );
}

} } // namespace cinder::gl