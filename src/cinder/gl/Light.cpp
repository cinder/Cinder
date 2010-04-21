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

#include "cinder/gl/Light.h"

namespace cinder {
namespace gl {

void Light::setAttenuation( float aConstantAttenuation, float aLinearAttenuation, float aQuadraticAttenuation )
{
	mConstantAttenuation = aConstantAttenuation;
	mLinearAttenuation = aLinearAttenuation;
	mQuadraticAttenuation = aQuadraticAttenuation;
}

void Light::setAmbient( const Color &aAmbient )
{
	mAmbient = aAmbient;
	glLightfv( GL_LIGHT0 + mID, GL_AMBIENT, mAmbient );
}

void Light::setDiffuse( const Color &aDiffuse )
{
	mDiffuse = aDiffuse;
	glLightfv( GL_LIGHT0 + mID, GL_DIFFUSE, mDiffuse );	
}	

void Light::setSpecular( const Color &aSpecular )
{
	mSpecular = aSpecular;
	glLightfv( GL_LIGHT0 + mID, GL_SPECULAR, mSpecular );	
}

void Light::lookAt( const Vec3f &eye, const Vec3f &target )
{
	setPosition( eye );
	setDirection( ( target - eye ).normalized() );
}

void Light::setDirection( const Vec3f &aDirection )
{
	mDirection = aDirection;
	if( mType == DIRECTIONAL )
		glLightfv( GL_LIGHT0 + mID, GL_POSITION, Vec4f( mDirection.x, mDirection.y, mDirection.z, 0.0f ) );
	mShadowCam.lookAt( mPosition, mPosition + mDirection );
}

void Light::setPosition( const Vec3f &aPosition )
{
	mPosition = aPosition;
	if( mType != DIRECTIONAL )
		glLightfv( GL_LIGHT0 + mID, GL_POSITION, Vec4f( mPosition.x, mPosition.y, mPosition.z, 1.0f ) );
	mShadowCam.lookAt( mPosition, mPosition + mDirection );
}

void Light::setConstantAttenuation( float aConstantAttenuation )
{
	mConstantAttenuation = aConstantAttenuation;
	glLightf( GL_LIGHT0 + mID, GL_CONSTANT_ATTENUATION, mConstantAttenuation );
}

void Light::setLinearAttenuation( float aLinearAttenuation )
{
	mLinearAttenuation = aLinearAttenuation;
	glLightf( GL_LIGHT0 + mID, GL_LINEAR_ATTENUATION, mLinearAttenuation );	
}

void Light::setQuadraticAttenuation( float aQuadraticAttenuation )
{
	mQuadraticAttenuation = aQuadraticAttenuation;
	glLightf( GL_LIGHT0 + mID, GL_QUADRATIC_ATTENUATION, mQuadraticAttenuation );
}

void Light::setSpotExponent( float aSpotExponent )
{
	mSpotExponent = aSpotExponent;
	glLightf( GL_LIGHT0 + mID, GL_SPOT_EXPONENT, mSpotExponent );	
}

void Light::setSpotCutoff( float aSpotCutoff )
{
	mSpotCutoff = aSpotCutoff;
	glLightf( GL_LIGHT0 + mID, GL_SPOT_CUTOFF, mSpotCutoff );
}

void Light::enable()
{
	if( mEnabled )
		return;
	
	glEnable( GL_LIGHT0 + mID );
	glLightfv( GL_LIGHT0 + mID, GL_AMBIENT, mAmbient );
	glLightfv( GL_LIGHT0 + mID, GL_DIFFUSE, mDiffuse );	
	glLightfv( GL_LIGHT0 + mID, GL_SPECULAR, mSpecular );
	if( mType == DIRECTIONAL )
		glLightfv( GL_LIGHT0 + mID, GL_POSITION, Vec4f( mDirection.x, mDirection.y, mDirection.z, 0.0f ) );
	else
		glLightfv( GL_LIGHT0 + mID, GL_POSITION, Vec4f( mPosition.x, mPosition.y, mPosition.z, 1.0f ) );
	glLightf( GL_LIGHT0 + mID, GL_CONSTANT_ATTENUATION, mConstantAttenuation );
	glLightf( GL_LIGHT0 + mID, GL_LINEAR_ATTENUATION, mLinearAttenuation );
	glLightf( GL_LIGHT0 + mID, GL_QUADRATIC_ATTENUATION, mQuadraticAttenuation );
	glLightfv( GL_LIGHT0 + mID, GL_SPOT_DIRECTION, mDirection );
	glLightf( GL_LIGHT0 + mID, GL_SPOT_CUTOFF, mSpotCutoff );
	glLightf( GL_LIGHT0 + mID, GL_SPOT_EXPONENT, mSpotExponent );
	
	mEnabled = true;
}

void Light::disable()
{
	glDisable( GL_LIGHT0 + mID );
	mEnabled = false;
}

void Light::update( const Camera &relativeCamera ) const
{
	Vec3f relPos;
	
	if( mType == POINT ) {
		Vec3f relPos = relativeCamera.getInverseModelViewMatrix().transformPointAffine( mPosition );
		glLightfv( GL_LIGHT0 + mID, GL_POSITION, Vec4f( relPos.x, relPos.y, relPos.z, 1.0f ) );
	}
	else if( mType == DIRECTIONAL ) {
		Vec3f relDir = relativeCamera.getInverseModelViewMatrix().transformVec( mDirection );
		glLightfv( GL_LIGHT0 + mID, GL_POSITION, Vec4f( relDir.x, relDir.y, relDir.z, 0.0f ) );	
	}
	else if( mType == SPOTLIGHT ) {
		Vec3f relPos = relativeCamera.getInverseModelViewMatrix().transformPointAffine( Vec3f( mPosition.x, mPosition.y, mPosition.z ) );
		glLightfv( GL_LIGHT0 + mID, GL_POSITION, Vec4f( relPos.x, relPos.y, relPos.z, 1.0f ) );
		Vec3f relSpotDir = relativeCamera.getInverseModelViewMatrix().transformPointAffine( Vec3f( mDirection.x, mDirection.y, mDirection.z ) );
		glLightfv( GL_LIGHT0 + mID, GL_SPOT_DIRECTION, relSpotDir );
	}
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

} // namespace gl
} // namespace cinder
