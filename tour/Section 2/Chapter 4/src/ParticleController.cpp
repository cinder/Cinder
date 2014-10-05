#include "cinder/app/AppBasic.h"
#include "cinder/gl/Batch.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "ParticleController.h"

using namespace ci;
using std::list;

ParticleController::ParticleController()
{
}

void ParticleController::applyForce( float zoneRadius, float lowerThresh, float higherThresh, float attractStrength, float repelStrength, float alignStrength  )
{
	float twoPI = M_PI * 2.0f;
	mParticleCentroid = vec3(0);
	mNumParticles = mParticles.size();
	
	for( list<Particle>::iterator p1 = mParticles.begin(); p1 != mParticles.end(); ++p1 ){
	
		list<Particle>::iterator p2 = p1;
		for( ++p2; p2 != mParticles.end(); ++p2 ) {
			vec3 dir = p1->mPos - p2->mPos;
			float distSqrd = glm::length2( dir );
			float zoneRadiusSqrd = zoneRadius * zoneRadius;
			
			if( distSqrd < zoneRadiusSqrd ){			// Neighbor is in the zone
				float percent = distSqrd/zoneRadiusSqrd;
	
				if( percent < lowerThresh ){			// Separation
					float F = ( lowerThresh/percent - 1.0f ) * repelStrength;
					dir = glm::normalize( dir ) * F;
			
					p1->mAcc += dir;
					p2->mAcc -= dir;
				} else if( percent < higherThresh ){	// Alignment
					float threshDelta		= higherThresh - lowerThresh;
					float adjustedPercent	= ( percent - lowerThresh )/threshDelta;
					float F					= ( 1.0f - ( cos( adjustedPercent * twoPI ) * -0.5f + 0.5f ) ) * alignStrength;
					
					p1->mAcc += p2->mVelNormal * F;
					p2->mAcc += p1->mVelNormal * F;
					
				} else {								// Cohesion
					float threshDelta		= 1.0f - higherThresh;
					float adjustedPercent	= ( percent - higherThresh )/threshDelta;
					float F					= ( 1.0f - ( cos( adjustedPercent * twoPI ) * -0.5f + 0.5f ) ) * attractStrength;
										
					dir = glm::normalize( dir );
					dir *= F;
			
					p1->mAcc -= dir;
					p2->mAcc += dir;
				}
			}
		}
		
		mParticleCentroid += p1->mPos;
	}
	mParticleCentroid /= (float)mNumParticles;
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
	{
		gl::ScopedColor color( ColorAf::white() );
		for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ){
			p->draw();
		}
	}
	
	{
		gl::ScopedColor color( ColorAf::white() );
		gl::VertBatch batch( GL_LINES );
		for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ){
			p->drawTail( batch );
		}
		batch.draw();
	}
}

void ParticleController::addParticles( int amt )
{
	for( int i=0; i<amt; i++ )
	{
		vec3 randVec = Rand::randVec3f();
		vec3 pos = randVec * Rand::randFloat( 100.0f, 600.0f );
		vec3 vel = -randVec;

		mParticles.push_back( Particle( pos, vel ) );
	}
}


