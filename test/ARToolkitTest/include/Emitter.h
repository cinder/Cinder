/*
The emitter is just an object that follows the cursor and
can spawn new particle objects. It would be easier to just make
the location vector match the cursor position but I have opted
to use a velocity vector because later I will be allowing for 
multiple emitters.
*/

#pragma once
#include "Particle.h"
#include "flint/Vector.h"
#include "flint/Color.h"
#include "flint/Offset.h"
#include "flint/gl/Texture.h"
#include <list>

class Emitter {
 public:
	Emitter();  
	void exist( fli::Offset mouseLoc );
	void setVelToMouse( fli::Offset mouseLoc );
	void findVelocity();
	void setPosition();
	void iterateListExist();
	void render();
	void iterateListRenderTrails();
	void addParticles( int _amt );

	fli::Vec3f loc;
	fli::Vec3f vel;
	fli::Vec3f velToMouse;
	fli::Color myColor;
	std::list<Particle>	particles;	
};