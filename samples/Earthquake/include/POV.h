#pragma once
#include "cinder/Vector.h"
#include "cinder/Camera.h"
#include "cinder/app/App.h"

class POV {
 public:
	POV() {}
	POV( ci::app::App *aApp, ci::Vec3f aEye, ci::Vec3f aCenter );
	
	void update();
	void adjustAngle( float aAngleDelta, float aPitch );
	void adjustDist( float aDist );
	void setEye( ci::Vec3f v );
	void setCenter( ci::Vec3f v );
	
	ci::CameraPersp mCam;
	ci::Vec3f mEye;
	ci::Vec3f mCenter;
	ci::Vec3f mEyeNormal;
	float mAngle, mAngleDest;
	float mDist, mDistDest;


 private:
	
	ci::app::App *mApp;
	
	float mAspectRatio;
	float mFOV;
	
};
	