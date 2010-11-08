#include "Particle.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"

using namespace ci;

Particle::Particle()
{
}

Particle::Particle( Vec2f loc )
{
	mLoc			= loc;
	mDir			= Rand::randVec2f();
	mDirToCursor	= Vec2f::zero();
	mVel			= 0.0f;
	mRadius			= 0.0f;
	mScale			= 3.0f;
}	

void Particle::update( const Channel32f &channel, const Vec2i &mouseLoc )
{
	mDirToCursor		= mouseLoc - mLoc;

	float distToCursor	= mDirToCursor.length();
	float time			= app::getElapsedSeconds();
	float dist			= distToCursor * 0.025f;
	float sinOffset		= sin( dist - time ) + 1.0f;
	
	mDirToCursor.normalize();
	mDirToCursor		*= sinOffset * 100.0f;
	
	Vec2f newLoc		= mLoc + mDirToCursor;
	newLoc.x			= constrain( newLoc.x, 0.0f, channel.getWidth() - 1.0f );
	newLoc.y			= constrain( newLoc.y, 0.0f, channel.getHeight() - 1.0f );
	
	mRadius				= channel.getValue( newLoc ) * mScale;
}

void Particle::draw()
{
	//gl::color( Color( 1.0f, 1.0f, 1.0f ) );
	//gl::drawVector( Vec3f( mLoc, 0.0f ), Vec3f( mLoc + mDirToCursor * 15.0f, 0.0f ), 6.0f, 3.0f );
	gl::drawSolidCircle( mLoc + mDirToCursor * 0.2f, mRadius );
}



