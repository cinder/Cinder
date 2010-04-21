
#pragma once
#include "Particle.h"
#include "Force.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/gl/Texture.h"
#include <list>
#include <vector>

class Emitter {
 public:
	Emitter(); 
	void update( ci::Vec3f mouseLoc, bool enableConstraints, bool mouseIsDown );
	void render( bool mouseIsDown, float radius, float alpha );
	void renderFloorLighting( float floorLevel );
	
	ci::Vec3f mLoc;
	ci::Vec3f mVel;
	float mRadius;
	float mRadiusDest;
	ci::gl::Texture mTexture;
	float mHeat;
	float mSpinSpeed;
	bool mIsTouchingConstraint;
};