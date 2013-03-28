// The copyright in this software is being made available under the BSD License, included below. 
// This software may be subject to other third party and contributor rights, including patent rights, 
// and no such rights are granted under this license.
//
// Copyright (c) 2013, Microsoft Open Technologies, Inc. 
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright notice, 
//   this list of conditions and the following disclaimer in the documentation 
//   and/or other materials provided with the distribution.
// - Neither the name of Microsoft Open Technologies, Inc. nor the names of its contributors 
//   may be used to endorse or promote products derived from this software 
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, 
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#include "cinder/dx/DxLight.h"

namespace cinder {
namespace dx {

void Light::setAttenuation( float aConstantAttenuation, float aLinearAttenuation, float aQuadraticAttenuation )
{
	mConstantAttenuation = aConstantAttenuation;
	mLinearAttenuation = aLinearAttenuation;
	mQuadraticAttenuation = aQuadraticAttenuation;
}

void Light::setAmbient( const Color &aAmbient )
{
	mAmbient = aAmbient;
}

void Light::setDiffuse( const Color &aDiffuse )
{
	mDiffuse = aDiffuse;
}	

void Light::setSpecular( const Color &aSpecular )
{
	mSpecular = aSpecular;
}

void Light::lookAt( const Vec3f &eye, const Vec3f &target )
{
	setPosition( eye );
	setDirection( ( target - eye ).normalized() );
}

void Light::setDirection( const Vec3f &aDirection )
{
	mDirection = aDirection;
	mShadowCam.lookAt( mPosition, mPosition + mDirection );
}

void Light::setPosition( const Vec3f &aPosition )
{
	mPosition = aPosition;
	mShadowCam.lookAt( mPosition, mPosition + mDirection );
}

void Light::setConstantAttenuation( float aConstantAttenuation )
{
	mConstantAttenuation = aConstantAttenuation;
}

void Light::setLinearAttenuation( float aLinearAttenuation )
{
	mLinearAttenuation = aLinearAttenuation;
}

void Light::setQuadraticAttenuation( float aQuadraticAttenuation )
{
	mQuadraticAttenuation = aQuadraticAttenuation;
}

void Light::setSpotExponent( float aSpotExponent )
{
	mSpotExponent = aSpotExponent;	
}

void Light::setSpotCutoff( float aSpotCutoff )
{
	mSpotCutoff = aSpotCutoff;
}

void Light::enable()
{
	if( mEnabled )
		return;
	
	mEnabled = true;
}

void Light::disable()
{
	mEnabled = false;
}

void Light::update( const Camera &relativeCamera )
{
	if( mType == POINT ) {
		mRelativePos = relativeCamera.getModelViewMatrix().transformPointAffine( mPosition );
	}
	else if( mType == DIRECTIONAL ) {
		mRelativeDir = relativeCamera.getModelViewMatrix().transformVec( mDirection );
	}
	else if( mType == SPOTLIGHT ) {
		mRelativePos = relativeCamera.getModelViewMatrix().transformPointAffine( mPosition );
		mRelativeDir = relativeCamera.getModelViewMatrix().transformVec( mDirection );
	}
	setLight(*this);
}

void Light::setShadowParams( float aShadowFOV, float aShadowNear, float aShadowFar )
{
	mShadowFOV = aShadowFOV;
	mShadowNear = aShadowNear;
	mShadowFar = aShadowFar;
	mShadowCam.setPerspective( mShadowFOV, 1.0f, mShadowNear, mShadowFar );	
}

void Light::setShadowRenderMatrices() const
{
	setMatrices( mShadowCam );
}

Matrix44f Light::getShadowTransformationMatrix( const Camera &camera ) const
{
	//		Matrix44f shadowBias;
	//		shadowBias.scale( Vec3f( 0.5f, 0.5f, 0.5f ) );
	//		shadowBias.translate( Vec3f( 1.0f, 1.0f, 1.0f ) );
	//		glMultMatrixf( shadowBias );

	Matrix44f shadowTransMatrix = mShadowCam.getProjectionMatrix();
	shadowTransMatrix *= mShadowCam.getModelViewMatrix();
	shadowTransMatrix *= camera.getInverseModelViewMatrix();
	return shadowTransMatrix;
}

void Light::setDefaults()
{
	if( mType != DIRECTIONAL )
		setPosition( Vec3f::zero() );
	else
		setDirection( Vec3f( 0, 0, 1.0f ) );
	mAmbient = Color( 1.0f, 1.0f, 1.0f );
	mDiffuse = Color( 1.0f, 1.0f, 1.0f );
	mSpecular = Color( 1.0f, 1.0f, 1.0f );
	mSpotExponent = 1.0f;
	if( mType != SPOTLIGHT )
		mSpotCutoff = 180.0f;
	else
		mSpotCutoff = 1.0f;
	mConstantAttenuation = mLinearAttenuation = mQuadraticAttenuation = 1.0f;
}

} // namespace dx
} // namespace cinder
