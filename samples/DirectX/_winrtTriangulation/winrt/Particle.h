#pragma once

#include "cinder/Vector.h"
#include <vector>

class Particle
{
public:
	Particle(float _floor);

	void Update(float _gravity, bool _isFloorActive, cinder::Vec2f _noise );

	void Reset(cinder::Vec2f _position, cinder::Vec2f _velocity, int _life);

	int						life;
	bool					isDead;
	float					floor;

	cinder::Vec2f 			position;
	cinder::Vec2f 			velocity;

};

