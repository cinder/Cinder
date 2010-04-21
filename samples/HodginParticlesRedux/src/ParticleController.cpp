#include "ParticleController.h"
#include "Constraint.h"
#include "ForcePerlin.h"
#include "ForceGravity.h"
#include "ForceRepulsion.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;
using std::list;
using std::vector;
using std::cout;


ParticleController::ParticleController()
{
	FORCEGRAVITY		= 0;
	FORCEPERLIN			= 1;
	FORCEREPULSION		= 2;
	
	mEnableQuad			= true;
	mEnableTail			= true;
	mEnablePerlin		= true;
	mEnableGravity		= false;
	mEnableConstraints	= true;
	
	adjustForcesOfType( FORCEPERLIN, mEnablePerlin );
	adjustForcesOfType( FORCEGRAVITY, mEnableGravity );
}

void ParticleController::createConstraints( Vec2f windowDim )
{
	mConstraints.clear();
	//mConstraints.push_back( new Constraint( Vec3f( 1, 0, 0 ), 0.0f, windowDim.x ) );
	mConstraints.push_back( new Constraint( Vec3f( 0, 1, 0 ), -1000.0f, windowDim.y * 0.625f ) );
}




void ParticleController::update( Emitter &emitter, int counter )
{
	// update Forces
	for( vector<Force*>::iterator forceIt = mForces.begin(); forceIt != mForces.end(); ++forceIt ){
		if( (*forceIt)->mEnabled )
			(*forceIt)->apply( mParticles, counter );
	}
	
	// update Particles
	for( list<Particle>::iterator particleIt = mParticles.begin(); particleIt != mParticles.end(); ) {
		if( ! particleIt->mIsDead ) {
			
			if( particleIt->mIsBouncing ){
				if( Rand::randFloat() < 0.025f && !particleIt->mIsDying ){
					mParticles.push_back( Particle( particleIt->mLoc[0], Vec3f::zero() ) );
					mParticles.back().mIsDying = true;
					particleIt->mIsDying = true;
					//particleIt->mVel += Rand::randVec3f() * Rand::randFloat( 2.0f, 3.0f );
				}
			}
			
			particleIt->move();
			particleIt->update();
			particleIt->finish();
			++particleIt;
		}
		else {
			particleIt = mParticles.erase( particleIt );
		}
	}
	
	// apply Constraints;
	for( vector<Constraint*>::iterator constraintIt = mConstraints.begin(); constraintIt != mConstraints.end(); ++constraintIt ){
		if( mEnableConstraints ){
			(*constraintIt)->apply( mParticles );
			(*constraintIt)->apply( emitter );
		}
	}
	
}


void ParticleController::render( float heat )
{
	if( mEnableQuad ){
		renderParticles( heat );
	}

	if( mEnableTail ){
		glDisable( GL_TEXTURE_2D );
		renderTrails();
	}
}


void ParticleController::renderParticles( float heat )
{
	glColor4f( 1, 1, 1, 1 );
	glBegin( GL_QUADS );
	for( list<Particle>::iterator particleIt = mParticles.begin(); particleIt != mParticles.end(); ++particleIt ) {
		if( particleIt->mRadius > 0.1f )
			particleIt->render( heat );
	}
	glEnd();
}


void ParticleController::renderTrails()
{
	for( list<Particle>::iterator particleIt = mParticles.begin(); particleIt != mParticles.end(); ++particleIt ) {
		particleIt->renderQuadStripTrail();
		//particleIt->renderLineStripTrail();
	}
}


void ParticleController::addParticles( int amt, Vec3f loc, Vec3f vel, float heat, float radius )
{
	for( int i = 0; i < amt; i++ ) {
		Vec3f lOffset = Rand::randVec3f();
		Vec3f l = loc + lOffset * radius * 0.25f;
		Vec3f v = -vel + lOffset * Rand::randFloat( 6.0f, 10.5f ) * ( heat + 0.75f ) + Rand::randVec3f() * Rand::randFloat( 1.0f, 2.0f );
		v.y *= 0.65f;
		mParticles.push_back( Particle( l, v ) );
	}
}



void ParticleController::adjustForcesOfType( int type, bool enable )
{
	if( enable ){
		if( type == FORCEPERLIN ){
			mForces.push_back( new ForcePerlin( FORCEPERLIN, 0.5f ) );
		}
		else if( type == FORCEGRAVITY ){
			mForces.push_back( new ForceGravity( FORCEGRAVITY, Vec3f( 0.0f, 0.1f, 0.0f ), 1.0f ) );
		}
		else if( type == FORCEREPULSION ){
			//mForces.push_back( new ForceRepulsion( FORCEREPULSION, 10.0f ) ); // slow and boring
		}
	} else {
		for( vector<Force*>::iterator forceIt = mForces.begin(); forceIt != mForces.end(); ){
			if( (*forceIt)->mType == type ){
				forceIt = mForces.erase( forceIt );
			} else {
				++forceIt;
			}
		}
	}
}

