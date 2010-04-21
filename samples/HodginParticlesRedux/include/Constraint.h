
#pragma once

#include "cinder/Vector.h"
#include "Emitter.h"
#include "Particle.h"
#include "cinder/gl/Texture.h"

#include <list>

class Constraint {
 public:
	Constraint( ci::Vec3f normal, float minValue, float maxValue );
	virtual void apply( Emitter &aEmitter );
	virtual void apply( std::list<Particle> &aParticles );
	ci::Vec3f mNormal;
	float mMinValue;
	float mMaxValue;
 protected:
};
