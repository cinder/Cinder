#include "Emitter.h"
#include "cinder/dx/dx.h"

using namespace ci;
using std::list;

extern void renderImage( Vec3f _loc, float _diam, Color _col, float _alpha );
extern float floorLevel;
extern bool ALLOWTRAILS, ALLOWFLOOR;
extern dx::TextureRef particleImg, emitterImg;

#define TOTAL_PARTICLES 1000

Emitter::Emitter() :
	mParticleIndex(0),
	myColor(Color(1,1,1)),
	loc(Vec3f::zero()),
	vel(Vec3f::zero())
{
	for( int i = 0; i < TOTAL_PARTICLES; i++ ) {
		particles.push_back( Particle( loc, vel ) );
	}

}

void Emitter::updateParticles()
{
	for ( Particle &particle : particles ) 
	{
		if ( !particle.ISDEAD ) 
		{
			int foo = 0;
			particle.update();
		}
	}
}

void Emitter::renderParticles()
{

	particleImg->bind();
	dx::begin( GL_QUADS );
	for ( Particle particle : particles ) 
	{
		if ( !particle.ISDEAD ) 
		{
			int foo = 0;
			particle.render();
		}
	}

	dx::end();
	particleImg->unbind();

}

void Emitter::renderParticleTrails()
{
	dx::begin( GL_QUADS );
	for ( Particle &particle : particles ) 
	{
		if ( !particle.ISDEAD ) 
		{
			particle.renderTrails();
		}
	}
	dx::end();
}

void Emitter::update( Vec2i mouseLoc )
{
	setVelToMouse( mouseLoc );
	findVelocity();
	setPosition();
	updateParticles();
}


void Emitter::render() 
{
	// RENDER EMITTER
	emitterImg->bind();
	dx::begin( GL_QUADS );
	renderImage( loc, 150, myColor, 1.0 );
	dx::end();
	emitterImg->unbind();

	// RENDER PARTICLES
	renderParticles();

	// CHECK RENDER TRAILS
	if( ALLOWTRAILS )
		renderParticleTrails();

}

void Emitter::setVelToMouse( Vec2i mouseLoc )
{
	velToMouse.set( mouseLoc.x - loc.x, mouseLoc.y - loc.y, 0 );
}

void Emitter::findVelocity()
{
	vel += ( velToMouse - vel ) * 0.25f;
}

void Emitter::setPosition()
{
	loc += vel;

	if( ALLOWFLOOR ) {
		if( loc.y > floorLevel ) {
			loc.y = floorLevel;
			vel.y = 0;
		}
	}
}

void Emitter::addParticles( int _amt )
{
	for( int i = 0; i < _amt; i++ ) {
		particles[mParticleIndex].reset( loc, vel);
		nextParticleIndex();
	}
}

int Emitter::nextParticleIndex() {
	mParticleIndex++;
	if ( mParticleIndex >= TOTAL_PARTICLES ) 
		mParticleIndex = 0; 
	return mParticleIndex;
}