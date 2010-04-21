
#pragma once

#include "Force.h"
#include "cinder/Vector.h"
#include "Particle.h"
#include "cinder/Perlin.h"
#include <list>

class ForcePerlin : public Force {
 public:
	ForcePerlin( int type, float magnitude );
	void apply( std::list<Particle> &particles, int counter );
	
	ci::Perlin mPerlin;
};
