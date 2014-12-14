/*
General Structure notes.
My classes tend to have a similar naming scheme and flow. I start with the 'exist' method.
Exist is what an object needs to do every frame. Usually 'existing' consists of four main things.
1) Find the velocity. This involves determining what influences there are on the velocity.
2) Apply the velocity to the location.
3) Render the object.
4) Age the object.

I also use the metaphor of aging and death. When first made, a particle's age will be zero.
Every frame, the age will increment. If the age reaches the lifeSpan (which is a random number
that I set in the constructor), then the bool ISDEAD is set to true and the arraylist iterator
removes the dead element from the list.
*/

#pragma once

#include "cinder/Vector.h"
#include <vector>

class Particle {
 public:
	Particle( ci::vec3 _loc, ci::vec3 _vel );
	void exist();
	void findPerlin();
	void findVelocity();
	void setPosition();
	void render();
	void renderTrails();
	void setAge();
	
	
	int len;            // number of elements in position array
	std::vector<ci::vec3> loc;        // array of position vectors
	ci::vec3 startLoc;     // just used to make sure every loc[] is initialized to the same position
	ci::vec3 vel;          // velocity vector
	ci::vec3 perlin;       // perlin noise vector
	float radius;       // particle's size
	float age;          // current age of particle
	int lifeSpan;       // max allowed age of particle
	float agePer;       // range from 1.0 (birth) to 0.0 (death)
	float bounceAge;    // amount to age particle when it bounces off floor
	bool ISDEAD;     // if age == lifeSpan, make particle die
	bool ISBOUNCING; // if particle hits the floor...		
};
