
#pragma once

#include "Force.h"
#include "cinder/Vector.h"
#include "Particle.h"
#include "cinder/Perlin.h"
#include <list>

class ForceRepulsion : public Force {
 public:
	ForceRepulsion( int type, float aMagnitude );
	void apply( std::list<Particle> &aParticles, int aCounter );
};
