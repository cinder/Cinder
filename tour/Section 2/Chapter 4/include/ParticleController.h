#pragma once
#include "Particle.h"
#include <list>

class ParticleController {
 public:
	ParticleController();
	void applyForce( float zoneRadius, float lowerThresh, float higherThresh, float attractStrength, float repelStrength, float orientStrength );
	void pullToCenter( const ci::vec3 &center );
	void update( bool flatten );
	void draw();
	void addParticles( int amt );
	
	std::list<Particle>	mParticles;
	ci::vec3 mParticleCentroid;
	int mNumParticles;
};