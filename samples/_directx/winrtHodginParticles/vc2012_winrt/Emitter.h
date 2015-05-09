#pragma once

#include "Particle.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/dx/DxTexture.h"

class Emitter {
 public:

	Emitter();
	void update( ci::Vec2i mouseLoc );
	void render();
	void addParticles( int _amt );

private:

	void setVelToMouse( ci::Vec2i mouseLoc );
	void findVelocity();
	void setPosition();	

	int nextParticleIndex();
	int mParticleIndex;

	void updateParticles();
	void renderParticles();
	void renderParticleTrails();

	ci::Vec3f loc;
	ci::Vec3f vel;
	ci::Vec3f velToMouse;
	ci::Color myColor;
	std::vector<Particle>	particles;	

};