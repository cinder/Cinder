#pragma once

#include "cinder/Vector.h"
#include <vector>

class Particle
{
public:
	Particle(float _floor);

	void Update(float _gravity, bool _isFloorActive, cinder::vec2 _noise );

	void Reset(cinder::vec2 _position, cinder::vec2 _velocity, int _life);

	int						life;
	bool					isDead;
	float					floor;

	cinder::vec2 			position;
	cinder::vec2 			velocity;

};

