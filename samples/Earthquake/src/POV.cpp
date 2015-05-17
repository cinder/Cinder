#include "POV.h"
#include "cinder/gl/gl.h"
#include "cinder/CinderMath.h"

POV::POV( ci::app::App *aApp, ci::vec3 aEye, ci::vec3 aCenter )
{
	mApp = aApp;

	mAspectRatio = mApp->getWindowAspectRatio();
	mFOV = 60.0f;

	mEye = aEye;
	mCenter = aCenter;

	mAngle = 0.0f;
	mAngleDest = 0.0f;
	mDist = 500.0f;
	mDistDest = 500.0f;
}

void POV::update()
{
	//mAngleDest += 0.01f;
	//mEye.y = sin( mAngleDest * 0.1f + 1.5707963f ) * 250.0f;

	mAngle -= ( mAngle - mAngleDest ) * 0.1f;
	mDist -= ( mDist - mDistDest ) * 0.1f;

	mEye.x = sin( mAngle ) * mDist;
	mEye.z = cos( mAngle ) * mDist;

	mEyeNormal = mEye - mCenter;
	glm::normalize( mEyeNormal );

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