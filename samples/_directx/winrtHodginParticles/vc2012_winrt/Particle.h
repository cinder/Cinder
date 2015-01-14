#pragma once
#include "cinder/Vector.h"
#include <vector>

class Particle {
 public:
	Particle( ci::vec3 _loc, ci::vec3 _vel );

	void update();
	void render();
	void reset( ci::vec3 _loc, ci::vec3 _vel );

	void renderTrails();
	bool ISDEAD;
	
	void findPerlin();
	void findVelocity();
	void setPosition();
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
	bool ISBOUNCING; // if particle hits the floor...		
};
