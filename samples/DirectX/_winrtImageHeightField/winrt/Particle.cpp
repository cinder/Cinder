#include "Particle.h"

Particle::Particle(float _floor)
{
		position = cinder::Vec2f::zero();
		velocity = cinder::Vec2f::zero();
		life = 0;
		isDead = true;
		floor = _floor;
}

void Particle::Reset(cinder::Vec2f _position, cinder::Vec2f _velocity, int _life)
{
	position = _position;
	velocity = _velocity;
	life = _life;
	isDead = false;
}

void Particle::Update(float _gravity, bool _isFloorActive, cinder::Vec2f _noise )
{

	velocity += _noise * .5 * (1.0f - life/200); // * dampen * ( life/200 ); // where dampen is .5 or so, and (life/200) decreases effect over lifetime ( may need to inverse )

	velocity.y += _gravity;

	if ( _isFloorActive ) 
	{
		if ( ( position.y + velocity.y ) > floor ) 
		{
			velocity.x *= 0.75f;
			velocity.y *= 0.75f;
			velocity.y *= -0.5f;
		}
	}

	position.x += velocity.x;
	position.y += velocity.y;

	life--;

	if ( life < 0 ) isDead = true;
}


