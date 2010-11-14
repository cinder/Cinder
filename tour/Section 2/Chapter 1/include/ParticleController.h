#pragma once
#include "Particle.h"
#include <list>

class ParticleController {
 public:
	ParticleController();
	void pullToCenter( const ci::Vec3f &center );
	void update();
	void draw();
	void addParticles( int amt );
	
	std::list<Particle>	mParticles;
};