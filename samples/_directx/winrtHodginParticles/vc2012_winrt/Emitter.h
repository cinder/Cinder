#pragma once

#include "Particle.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/dx/DxTexture.h"

class Emitter {
 public:

	Emitter();
	void update( ci::ivec2 mouseLoc );
	void render();
	void addParticles( int _amt );

private:

	void setVelToMouse( ci::ivec2 mouseLoc );
	void findVelocity();
	void setPosition();	

	int nextParticleIndex();
	int mParticleIndex;

	void updateParticles();
	void renderParticles();
	void renderParticleTrails();

	ci::vec3 loc;
	ci::vec3 vel;
	ci::vec3 velToMouse;
	ci::Color myColor;
	std::vector<Particle>	particles;	

};