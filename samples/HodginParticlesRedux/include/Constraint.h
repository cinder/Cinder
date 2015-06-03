
#pragma once

#include "cinder/Vector.h"
#include "Emitter.h"
#include "Particle.h"
#include "cinder/gl/Texture.h"

#include <list>

class Constraint {
 public:
	Constraint( ci::vec3 normal, float minValue, float maxValue );
	virtual void apply( Emitter &aEmitter );
	virtual void apply( std::list<Particle> &aParticles );
	ci::vec3 mNormal;
	float mMinValue;
	float mMaxValue;
 protected:
};
