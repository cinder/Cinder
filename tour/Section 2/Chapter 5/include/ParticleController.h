#pragma once
#include "Particle.h"
#include "Predator.h"
#include "cinder/Perlin.h"
#include <list>

class ParticleController {
 public:
	ParticleController();
	void applyForceToParticles( float zoneRadius, float lowerThresh, float higherThresh, float attractStrength, float repelStrength, float orientStrength );
	void applyForceToPredators( float zoneRadius, float lowerThresh, float higherThresh );
	void pullToCenter( const ci::vec3 &center );
	void update( bool flatten );
	void draw();
	void addParticles( int amt );
	void addPredators( int amt );
	void removeParticles( int amt );
	ci::vec3 getPos();
	
	ci::Perlin mPerlin;
	
	std::list<Particle>	mParticles;
	std::list<Predator> mPredators;
	ci::vec3 mParticleCentroid;
	int mNumParticles;
};