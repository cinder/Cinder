#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "ParticleController.h"

using namespace ci;
using std::list;

ParticleController::ParticleController()
{
}

void ParticleController::repulseParticles()
{
	for( list<Particle>::iterator p1 = mParticles.begin(); p1 != mParticles.end(); ++p1 ){
	
		list<Particle>::iterator p2 = p1;
		for( ++p2; p2 != mParticles.end(); ++p2 ) {
			vec2 dir = p1->mLoc - p2->mLoc;
			
			float thresh = ( p1->mRadius + p2->mRadius ) * 5.0f;
			if( dir.x > -thresh && dir.x < thresh && dir.y > -thresh && dir.y < thresh ){
				float distSqrd = dir.lengthSquared() * dir.length();
				
				if( distSqrd > 0.0f ){
					float F = 1.0f/distSqrd;
					dir.normalize();

					// acceleration = force / mass
					p1->mAcc += ( F * dir ) / p1->mMass;
					p2->mAcc -= ( F * dir ) / p2->mMass;
				}
			}
		}
	}
}

void ParticleController::pullToCenter()
{
	for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ){
		p->pullToCenter();
	}
}

void ParticleController::applyPerlin( const Perlin &perlin )
{
	for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ){
		p->applyPerlin( perlin );
	}
}

void ParticleController::update( const Channel32f &channel, const ivec2 &mouseLoc )
{
	for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ){
		if( p->mIsDead ){
			p = mParticles.erase( p );
		} else {
			p->update( channel, mouseLoc );
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
		vec2 loc = mouseLoc + Rand::randVec2f() * Rand::randFloat( 5.0f );
		vec2 velOffset = Rand::randVec2f() * Rand::randFloat( 1.0f, 3.0f );
		vec2 vel = mouseVel * 5.0f + velOffset;
		mParticles.push_back( Particle( loc, vel ) );
	}
	
	std::cout << mParticles.size() << std::endl;
}

void ParticleController::removeParticles( int amt )
{
	for( int i=0; i<amt; i++ )
	{
		mParticles.pop_back();
	}
}

