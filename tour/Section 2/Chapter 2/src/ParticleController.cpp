#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/gl/Batch.h"
#include "ParticleController.h"

using namespace ci;
using std::list;

ParticleController::ParticleController()
{
}

void ParticleController::applyForceToParticles( float zoneRadiusSqrd )
{
	for( list<Particle>::iterator p1 = mParticles.begin(); p1 != mParticles.end(); ++p1 ){
	
		list<Particle>::iterator p2 = p1;
		for( ++p2; p2 != mParticles.end(); ++p2 ) {
			vec3 dir = p1->mPos - p2->mPos;
			float distSqrd = glm::length2( dir );
					
			if( distSqrd <= zoneRadiusSqrd ){	// SEPARATION
				float F = ( zoneRadiusSqrd/distSqrd - 1.0f ) * 0.01f;
				dir = glm::normalize( dir );
				dir *= F;
			
				p1->mAcc += dir;
				p2->mAcc -= dir;
			}
		}
	}
}

void ParticleController::pullToCenter( const ci::vec3 &center )
{
	for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ){
		p->pullToCenter( center );
	}
}

void ParticleController::update( bool flatten )
{
	for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ){
		p->update( flatten );
	}
}

void ParticleController::draw()
{
	gl::ScopedColor color( ColorAf::white() );
	for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ){
		p->draw();
	}
	
	gl::VertBatch batch( GL_LINES );
	for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ){
		p->drawTail( batch );
	}
	batch.draw();
}

void ParticleController::addParticles( int amt )
{
	for( int i=0; i<amt; i++ )
	{
		vec3 pos = Rand::randVec3f() * Rand::randFloat( 50.0f, 250.0f );
		vec3 vel = Rand::randVec3f() * 2.0f;
		mParticles.push_back( Particle( pos, vel ) );
	}
}

