#include "Particle.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"

using namespace ci;

Particle::Particle()
{
}

Particle::Particle( vec3 pos, vec3 vel )
{
	mPos			= pos;
	mVel			= vel;
	mAcc			= vec3::zero();

	mRadius			= 2.0f;
	mDecay			= 0.99f;
}

void Particle::pullToCenter( const vec3 &center )
{
	vec3 dirToCenter	= mPos - center;
	float distToCenter	= dirToCenter.length();
	float maxDistance	= 300.0f;
	
	if( distToCenter > maxDistance ){
		dirToCenter.normalize();
		float pullStrength = 0.0001f;
		mVel -= dirToCenter * ( ( distToCenter - maxDistance ) * pullStrength );
	}
}

void Particle::update()
{	
	mVel += mAcc;
	mPos += mVel;
	mVel *= mDecay;
	mAcc = vec3::zero();
}

void Particle::draw()
{
	gl::drawSphere( mPos, mRadius, 16 );
}



