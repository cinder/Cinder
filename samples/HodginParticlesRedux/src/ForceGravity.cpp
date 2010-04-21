#include "ForceGravity.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;
using std::list;

ForceGravity::ForceGravity( int type, Vec3f axis, float magnitude )
	: Force( type, magnitude )
{
	mAxis = axis;
}


void ForceGravity::apply( std::list<Particle> &particles, int counter )
{
	for( list<Particle>::iterator particleIt = particles.begin(); particleIt != particles.end(); ++particleIt )
	{
		particleIt->mAcc += mAxis * particleIt->mMass;
	}
}


