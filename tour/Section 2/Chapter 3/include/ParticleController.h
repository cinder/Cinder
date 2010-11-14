#pragma once
#include "Particle.h"
#include <list>

class ParticleController {
 public:
	ParticleController();
	void applyForceToParticles( float zoneRadiusSqrd, float thresh );
	void pullToCenter( const ci::Vec3f &center );
	void update( bool flatten );
	void draw();
	void addParticles( int amt );
	void removeParticles( int amt );
	
	std::list<Particle>	mParticles;
};