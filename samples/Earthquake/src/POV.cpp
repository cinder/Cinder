#include "POV.h"

#include "cinder/gl/gl.h"
#include "cinder/CinderMath.h"

POV::POV( ci::app::App *aApp, ci::vec3 aEye, ci::vec3 aCenter )
	: mApp( aApp ), mEye( aEye ), mCenter( aCenter )
{
	mAspectRatio = mApp->getWindowAspectRatio();
	mFOV = 60.0f;

	mAngle = 0.0f;
	mAngleDest = 0.0f;
	mDist = 500.0f;
	mDistDest = 500.0f;
}

void POV::update()
{
	mAngle -= ( mAngle - mAngleDest ) * 0.1f;
	mDist -= ( mDist - mDistDest ) * 0.1f;

	mEye.x = sin( mAngle ) * mDist;
	mEye.z = cos( mAngle ) * mDist;

	mEyeNormal = glm::normalize( mEye - mCenter );

	mCam.lookAt( mEye, mCenter );
	mCam.setPerspective( mFOV, mApp->getWindowAspectRatio(), 1, 20000 );

	ci::gl::setMatrices( mCam );
}

void POV::adjustAngle( float angleDelta, float pitch )
{
	mAngleDest += angleDelta;
	mEye.y = pitch;
}

void POV::adjustDist( float distDelta )
{
	mDistDest += distDelta;
}

void POV::setEye( ci::vec3 v )
{
	mEye = v;
}

void POV::setCenter( ci::vec3 v )
{
	mCenter = v;
}