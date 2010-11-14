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
	void pullToCenter( const ci::Vec3f &center );
	void update( bool flatten );
	void draw();
	void addParticles( int amt );
	void addPredators( int amt );
	void removeParticles( int amt );
	ci::Vec3f getPos();
	
	ci::Perlin mPerlin;
	
	std::list<Particle>	mParticles;
	std::list<Predator> mPredators;
	ci::Vec3f mParticleCentroid;
	int mNumParticles;
};