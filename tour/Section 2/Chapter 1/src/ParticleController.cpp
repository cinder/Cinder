#include "cinder/app/App.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "ParticleController.h"

using namespace ci;
using std::list;

ParticleController::ParticleController()
{
}

void ParticleController::pullToCenter( const ci::vec3 &center )
{
	for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ) {
		p->pullToCenter( center );
	}
}

void ParticleController::update()
{
	for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ) {
		p->update();
	}
}

void ParticleController::draw()
{
	for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ) {
		p->draw();
	}
}

void ParticleController::addParticles( int amt )
{
	for( int i=0; i<amt; i++ ) {
		vec3 randVec = Rand::randVec3f();
		vec3 pos = randVec * Rand::randFloat( 50.0f );
		vec3 vel = randVec * Rand::randFloat( 5.0f );
		mParticles.push_back( Particle( pos, vel ) );
	}
}


