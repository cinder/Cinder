#pragma once
#include "cinder/Vector.h"
#include <vector>

class Particle {
 public:
	Particle( ci::Vec3f _loc, ci::Vec3f _vel );

	void update();
	void render();
	void reset( ci::Vec3f _loc, ci::Vec3f _vel );

	void renderTrails();
	bool ISDEAD;
	
	void findPerlin();
	void findVelocity();
	void setPosition();
	void setAge();
	
	int len;            // number of elements in position array
	std::vector<ci::Vec3f> loc;        // array of position vectors
	ci::Vec3f startLoc;     // just used to make sure every loc[] is initialized to the same position
	ci::Vec3f vel;          // velocity vector
	ci::Vec3f perlin;       // perlin noise vector
	float radius;       // particle's size
	float age;          // current age of particle
	int lifeSpan;       // max allowed age of particle
	float agePer;       // range from 1.0 (birth) to 0.0 (death)
	float bounceAge;    // amount to age particle when it bounces off floor
	bool ISBOUNCING; // if particle hits the floor...		
};
