#pragma once

#include "cinder/Vector.h"
#include <vector>

class Particle
{
public:
	Particle(int _textureID);

	void Update(float width, float height);

	void Reset(cinder::vec2 _position, cinder::vec2 _velocity);

	int						textureID;

	cinder::vec2 			position;

	cinder::vec2 			velocity;

};

