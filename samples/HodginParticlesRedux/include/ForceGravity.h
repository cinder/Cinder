
#pragma once

#include "Force.h"
#include "cinder/Vector.h"
#include "Particle.h"
#include "cinder/Perlin.h"
#include "cinder/gl/Texture.h"
#include <list>

class ForceGravity : public Force {
 public:
	ForceGravity( int type, ci::vec3 axis, float magnitude );
	void apply( std::list<Particle> &particles, int counter );
	
	ci::vec3 mAxis;
};
