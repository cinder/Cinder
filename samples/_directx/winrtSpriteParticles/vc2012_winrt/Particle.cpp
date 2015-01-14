#include "Particle.h"

Particle::Particle(int _textureID)
{
		position = cinder::vec2::zero();
		velocity = cinder::vec2::zero();
		textureID = _textureID%4;
}

void Particle::Reset(cinder::vec2 _position, cinder::vec2 _velocity)
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


