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

#include "cinder/Color.h"
#include "cinder/gl/gl.h"
#include "cinder/Vector.h"
#include "cinder/Camera.h"

namespace cinder {
namespace gl {

class Light {
 public:
	Light( int aType, int aID ) : mType( aType ), mID( aID ), mEnabled( false ) { setDefaults(); }
	
	void setAmbient( const Color &aAmbient );
	void setDiffuse( const Color &aDiffuse );
	void setSpecular( const Color &aSpecular );

	void	lookAt( const Vec3f &eye, const Vec3f &target );
	void	setDirection( const Vec3f &aDirection );
	Vec3f	getPosition() const { return mPosition; }
	void	setPosition( const Vec3f &aPosition );	
	
	void setAttenuation( float aConstantAttenuation, float aLinearAttenuation = 1.0f, float aQuadraticAttenuation = 1.0f );
	void setConstantAttenuation( float aConstantAttenuation );
	void setLinearAttenuation( float aLinearAttenuation );
	void setQuadraticAttenuation( float aQuadraticAttenuation );
	
	void setSpotExponent( float aSpotExponent );
	void setSpotCutoff( float aSpotCutoff );
	
	void enable();
	void disable();
	void update( const Camera &relativeCamera ) const;

	void setShadowParams( float aShadowFOV, float aShadowNear, float aShadowFar );
	void setShadowRenderMatrices() const;
	Matrix44f getShadowTransformationMatrix( const Camera &camera ) const;
	const CameraPersp&	getShadowCamera() const { return mShadowCam; }
	
	enum Type { POINT, DIRECTIONAL, SPOTLIGHT };
	
 protected:
	void	setDefaults();

	int		mType;
	int		mID;
	bool	mEnabled;
	Color	mAmbient, mDiffuse, mSpecular;
	Vec3f	mPosition, mDirection;
	float	mSpotExponent, mSpotCutoff;
	float	mConstantAttenuation, mLinearAttenuation, mQuadraticAttenuation;
	
	CameraPersp		mShadowCam;
	float			mShadowFOV, mShadowNear, mShadowFar;
};

} // namespace gl
} // namespace cinder
