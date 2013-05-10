#include "Particle.h"

Particle::Particle(int _textureID)
{
		position = cinder::Vec2f::zero();
		velocity = cinder::Vec2f::zero();
		textureID = _textureID%4;
}

void Particle::Reset(cinder::Vec2f _position, cinder::Vec2f _velocity)
{
	position = _position;
	velocity = _velocity;
}

void Particle::Update(float width, float height )
{

	// update velocity
	position.x += velocity.x;
	position.y += velocity.y;

	// wrap around screen if needed
	if ( position.x < 0 ) position.x += width;
	if ( position.x > width ) position.x -= width;
	if( position.y < 0 ) position.y += height;
	if ( position.y > height ) position.y -= height;

}


