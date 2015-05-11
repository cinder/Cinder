#pragma once
#include "Particle.h"
#include <list>

class ParticleController {
 public:
	ParticleController();
	void applyForceToParticles( float zoneRadiusSqrd );
	void pullToCenter( const ci::vec3 &center );
	void update( bool flatten );
	void draw();
	void addParticles( int amt );
	
	std::list<Particle>	mParticles;
};