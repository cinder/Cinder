#pragma once

#include "cinder/Vector.h"
#include <vector>

class Particle
{
public:
	Particle(int _textureID);

	void Update(float width, float height);

	void Reset(cinder::Vec2f _position, cinder::Vec2f _velocity);

	int						textureID;

	cinder::Vec2f 			position;

	cinder::Vec2f 			velocity;

};

