#pragma once
#include "cinder/Channel.h"
#include "Particle.h"
#include "cinder/Perlin.h"
#include <list>

class ParticleController {
 public:
	ParticleController();
	void update( const ci::Perlin &perlin, const ci::Channel32f &channel, const ci::Vec2i &mouseLoc );
	void draw();
	void addParticles( int amt, const ci::Vec2i &mouseLoc, const ci::Vec2f &mouseVel );
	void removeParticles( int amt );
	
	std::list<Particle>	mParticles;
};