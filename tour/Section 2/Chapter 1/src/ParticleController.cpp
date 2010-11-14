#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "ParticleController.h"

using namespace ci;
using std::list;

ParticleController::ParticleController()
{
}

void ParticleController::pullToCenter( const ci::Vec3f &center )
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
		Vec3f randVec = Rand::randVec3f();
		Vec3f pos = randVec * Rand::randFloat( 50.0f );
		Vec3f vel = randVec * Rand::randFloat( 5.0f );
		mParticles.push_back( Particle( pos, vel ) );
	}
}


