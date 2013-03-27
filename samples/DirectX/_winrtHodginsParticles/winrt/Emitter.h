#pragma once

#include "Particle.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"

//#include "cinder/gl/Texture.h"
#include "cinder/dx/DxTexture.h"

#include <list>

class Emitter {
 public:
	Emitter();  
	void exist( ci::Vec2i mouseLoc );
	void setVelToMouse( ci::Vec2i mouseLoc );
	void findVelocity();
	void setPosition();
	void iterateListExist();
	void render();
	void iterateListRenderTrails();
	void addParticles( int _amt );

	ci::Vec3f loc;
	ci::Vec3f vel;
	ci::Vec3f velToMouse;
	ci::Color myColor;
	std::list<Particle>	particles;	
};