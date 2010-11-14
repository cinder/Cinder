#pragma once
#include "cinder/Channel.h"
#include "Particle.h"
#include <list>

class ParticleController {
 public:
	ParticleController();
	ParticleController( int res );
	void update( const ci::Channel32f &channel );
	void draw();
	void addParticle( int xRes, int yRes, int res );
	void addParticles( int amt );
	void removeParticles( int amt );
	
	std::list<Particle>	mParticles;
	
	int mXRes, mYRes;
};