#pragma once
#include "cinder/Channel.h"
#include "Particle.h"
#include "cinder/Perlin.h"
#include <list>

class ParticleController {
public:
	ParticleController();
	void update( const ci::Perlin &perlin, const ci::Channel32f &channel, const ci::ivec2 &mouseLoc );
	void draw();
	void addParticles( int amt, const ci::ivec2 &mouseLoc, const ci::vec2 &mouseVel );
	void removeParticles( int amt );

	std::list<Particle> mParticles;
};
