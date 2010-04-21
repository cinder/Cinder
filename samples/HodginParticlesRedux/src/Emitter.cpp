#include "Emitter.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;

Emitter::Emitter()
{
	mLoc		= Vec3f::zero();
	mVel		= Vec3f::zero();
	mRadius		= 50.0f;
	mRadiusDest = 50.0f;
	mHeat		= 0.0f;
	mSpinSpeed	= 0.0f;
}


void Emitter::update( Vec3f mouseLoc, bool enableConstraints, bool mouseIsDown )
{
	Vec3f mouseVel( mouseLoc.x - mLoc.x, mouseLoc.y - mLoc.y, 0 );
	mVel -= ( mVel - mouseVel ) * 0.1f;
	
	mLoc += mVel;
	mVel *= 0.9f;
	
	if( mIsTouchingConstraint && mouseIsDown )
		mVel += Rand::randVec3f() * 3.0f * mHeat;
			
	if( mouseIsDown  ) {
		mSpinSpeed -= ( mSpinSpeed - 1.0f ) * 0.04f;
		mHeat -= ( mHeat - mSpinSpeed ) * 0.1f;
		mIsTouchingConstraint = false;
	} else {
		mSpinSpeed -= ( mSpinSpeed - 0.0f ) * 0.05f;
		mHeat -= ( mHeat - mSpinSpeed ) * 0.01f;
	}
}


void Emitter::render( bool mouseIsDown, float radius, float alpha )
{
	mRadius = radius - mSpinSpeed * 30.0f + 70.0f * ( 1.0f - mSpinSpeed );
	glColor4f( 1, 1, 1, alpha );
	gl::drawSphere( mLoc, mRadius, 32 );
}


void Emitter::renderFloorLighting( float floorLevel )
{
	mRadius -= 15.0f;
	float heightPer = 1.0f - ( floorLevel - ( mLoc.y + mRadius  ) ) / floorLevel;
	glBegin( GL_QUADS );
	
	float radius = 2000.0f * ( 1.3f - heightPer ) + 250.0f;
	
	glColor4f( 1.0f, 0.0f, 0.0f, heightPer * ( mHeat - 0.25f ) - Rand::randFloat( 0.25f ) );
	glTexCoord2f( 0, 0 );	glVertex3f( mLoc.x - radius, floorLevel, mLoc.z - radius );
	glTexCoord2f( 1, 0 );	glVertex3f( mLoc.x + radius, floorLevel, mLoc.z - radius );
	glTexCoord2f( 1, 1 );	glVertex3f( mLoc.x + radius, floorLevel, mLoc.z + radius );
	glTexCoord2f( 0, 1 );	glVertex3f( mLoc.x - radius, floorLevel, mLoc.z + radius );
	
	radius = 400.0f * heightPer * mSpinSpeed + 50.0f;
	
	glColor4f( 1.0f, 0.65f, 0.3f, pow( heightPer, 5.0f ) * ( mHeat - 0.25f ) );
	glTexCoord2f( 0, 0 );	glVertex3f( mLoc.x - radius, floorLevel, mLoc.z - radius );
	glTexCoord2f( 1, 0 );	glVertex3f( mLoc.x + radius, floorLevel, mLoc.z - radius );
	glTexCoord2f( 1, 1 );	glVertex3f( mLoc.x + radius, floorLevel, mLoc.z + radius );
	glTexCoord2f( 0, 1 );	glVertex3f( mLoc.x - radius, floorLevel, mLoc.z + radius );
	
	glEnd();
}
