#pragma once

#include "cinder/Vector.h"
#include <vector>

class Billboard
{
public:
	Billboard(cinder::Vec3f _position, float _xRotation, float _yRotation);
	
	void Update();

	int						life;
	bool					isDead;
	float					xRotation;
	float					yRotation;
	float					xStep;
	float					yStep;

	cinder::Vec3f 			position;
	cinder::Vec3f 			velocity;

};

