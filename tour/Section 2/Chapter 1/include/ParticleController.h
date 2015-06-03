#pragma once
#include "Particle.h"
#include <list>

class ParticleController {
 public:
	ParticleController();
	void pullToCenter( const ci::vec3 &center );
	void update();
	void draw();
	void addParticles( int amt );
	
	std::list<Particle>	mParticles;
};