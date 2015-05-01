/*
The emitter is just an object that follows the cursor and
can spawn new particle objects. It would be easier to just make
the location vector match the cursor position but I have opted
to use a velocity vector because later I will be allowing for 
multiple emitters.
*/

#pragma once
#include "Particle.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/gl/Texture.h"
#include <list>

class Emitter {
 public:
	Emitter();  
	void exist( ci::ivec2 mouseLoc );
	void setVelToMouse( ci::ivec2 mouseLoc );
	void findVelocity();
	void setPosition();
	void iterateListExist();
	void render();
	void iterateListRenderTrails();
	void addParticles( int _amt );

	ci::vec3 loc;
	ci::vec3 vel;
	ci::vec3 velToMouse;
	ci::Color myColor;
	std::list<Particle>	particles;	
};