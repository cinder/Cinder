#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "ParticleController.h"

using namespace ci;
using std::list;

ParticleController::ParticleController()
{
}

void ParticleController::update( const Perlin &perlin, const Channel32f &channel, const ivec2 &mouseLoc )
{
	for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ) {
		if( p->mIsDead ){
			p = mParticles.erase( p );
		} else {
			p->update( perlin, channel, mouseLoc );
			++p;
		}
	}
}

void ParticleController::draw()
{
	for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ){
		p->draw();
	}
}

void ParticleController::addParticles( int amt, const ivec2 &mouseLoc, const vec2 &mouseVel )
{
	for( int i=0; i<amt; i++ )
	{
		vec2 loc = mouseLoc + Rand::randVec2f() * 5.0f;
		vec2 velOffset = Rand::randVec2f() * Rand::randFloat( 1.0f, 5.0f );
		vec2 vel = mouseVel * 0.375f + velOffset;
		mParticles.push_back( Particle( loc, vel ) );
	}
}

void ParticleController::removeParticles( int amt )
{
	for( int i=0; i<amt; i++ )
	{
		mParticles.pop_back();
	}
}

