#include "Emitter.h"

#include "flint/gl/gl.h"

using namespace fli;
using std::list;

// These are defined in HodginParticlesApp.cpp
extern void renderImage( Vec3f _loc, float _diam, Color _col, float _alpha );
extern float floorLevel;
extern uint8_t FLOOR_AXIS;
extern bool ALLOWTRAILS, ALLOWFLOOR;
extern gl::Texture *particleImg, *emitterImg;

Emitter::Emitter()
{
	myColor = Color( 1, 1, 1 );
	loc = Vec3f::zero();
	vel = Vec3f::zero();
}

void Emitter::exist( Offset mouseLoc )
{
	setVelToMouse( mouseLoc );
	findVelocity();
	setPosition();
	iterateListExist();
	render();

	glDisable( GL_TEXTURE_2D );
	if( ALLOWTRAILS )
		iterateListRenderTrails();
}

void Emitter::setVelToMouse( Offset mouseLoc )
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
		if( loc[FLOOR_AXIS] < 0 ) {
			loc[FLOOR_AXIS] = 0;
			vel[FLOOR_AXIS] = 0;
		}
	}
loc = Vec3f( 0, 0, 400 );	
vel = Vec3f::zero();
}

void Emitter::iterateListExist()
{
	glEnable( GL_TEXTURE_2D );
	particleImg->bind();

	for( list<Particle>::iterator it = particles.begin(); it != particles.end(); ) {
		if( ! it->ISDEAD ) {
			it->exist();
			++it;
		}
		else {
			it = particles.erase( it );
		}
	}
}

void Emitter::render()
{
	emitterImg->bind();
	renderImage( loc, 150, myColor, 1.0 );
}

void Emitter::iterateListRenderTrails()
{
	for( list<Particle>::iterator it = particles.begin(); it != particles.end(); ++it ) {
		it->renderTrails();
	}
}

void Emitter::addParticles( int _amt )
{
	for( int i = 0; i < _amt; i++ ) {
		particles.push_back( Particle( loc, vel ) );
	}
}
