#pragma once
#include "Particle.h"
#include <list>

class ParticleController {
 public:
	ParticleController();
	void update();
	void draw();
	void addParticles( int amt );
	void removeParticles( int amt );
	
	std::list<Particle>	mParticles;
};