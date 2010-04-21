#include "Particle.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/Color.h"
#include "cinder/Perlin.h"

using namespace ci;

// These live in HodginParticleApp.cpp
extern void renderImage( Vec3f _loc, float _diam, Color _col, float _alpha );
extern float floorLevel;
extern bool ALLOWFLOOR, ALLOWGRAVITY, ALLOWPERLIN;
extern Vec3f gravity;
extern int counter;
Perlin sPerlin( 2 );

Particle::Particle( Vec3f _loc, Vec3f _vel )
{
	radius      = Rand::randFloat( 10, 40 );
	len         = (int)radius;

	// This confusing-looking line does three things at once.
	// First, you make a random vector.
	// Next, you multiply that vector by a random number from 0.0 to 5.0.
	// Finally, you add this new vector to the original sent vector.
	// _loc.add( );
	// This is just a way to make sure all the particles made this frame
	// don't all start on the exact same pixel. This staggering will be useful
	// when we incorporate magnetic repulsion in a later tutorial.
	startLoc = _loc + Rand::randVec3f() * Rand::randFloat( 5.0f );

	for( int i = 0; i < len; i++ ) {
		loc.push_back( startLoc );
	}

	// This next confusing-looking line does four things.
	// 1) Make a random vector.
	// 2) Multiply that vector by a random number from 0.0 to 10.0.
	// 3) Scale down the original sent velocity just to calm things down a bit.
	// 4) Add this new vector to the scaled down original sent vector.
	// This randomizes the original sent velocity so the particles
	// dont all move at the same speed in the same direction.
	vel = _vel * 0.5f + Rand::randVec3f() * Rand::randFloat( 10.0f );

	perlin = Vec3f::zero();

	age         = 0;
	agePer		= 1.0f;
	bounceAge   = 2;
	lifeSpan    = (int)( radius );
	ISDEAD		= false;
	ISBOUNCING	= false;
}

void Particle::exist()
{
	if( ALLOWPERLIN )
		findPerlin();
	  
	findVelocity();
	setPosition();
	render();
	setAge();
}

void Particle::findPerlin()
{
	Vec3f noise = sPerlin.dfBm( loc[0] * 0.01f + Vec3f( 0, 0, counter / 100.0f ) );
	perlin = noise.normalized() * 0.5f;
}

void Particle::findVelocity()
{
	if( ALLOWGRAVITY )
		vel += gravity;
	  
	if( ALLOWPERLIN )
		vel += perlin;

	if( ALLOWFLOOR ) {
		if( loc[0].y + vel.y > floorLevel ){
			ISBOUNCING = true;
		}
		else {
			ISBOUNCING = false;
		}
	}

	if( ISBOUNCING ) {
		vel *= 0.75f;
		vel.y *= -0.5f;
	}
}

void Particle::setPosition()
{
	// Every frame, the current location will be passed on to
	// the next element in the location array. Think 'cursor trail effect'.
	for( int i = len - 1; i > 0; i-- ) {
		loc[i] = loc[i-1];
	}

	// Set the initial location.
	// loc[0] represents the current position of the particle.
	loc[0] += vel;
}

void Particle::render()
{
	// As the particle ages, it will gain blue but will lose red and green.
	Color c = Color( agePer, agePer * 0.75f, 1.0f - agePer );
	renderImage( loc[0], radius * agePer, c, 1.0f );
}

void Particle::renderTrails()
{
	glBegin( GL_QUAD_STRIP );

	for( int i = 0; i < len - 2; i++ ) {
		float per     = 1.0f - i / (float)(len-1);
		// Okay, here is some vector craziness that I probably cant explain very well.
		// This is one of those things that I was taught and though I can picture in my mind
		// what the following 4 lines of code does, I doubt I can explain it.  In short,
		// I am using the cross product (wikipedia it) of the vector between adjacent
		// location array elements (perp0), and finding two vectors that are at right angles to 
		// it (perp1 and perp2). I then use perp1 to allow me to draw a ribbon with controllable
		// widths.
		// 
		// It's much more useful when dealing with a 3D space and a rotating camera. Think of it
		// like this. These trails are meant to function like motion blurs rather than dragged ribbons.
		// A dragged ribbon can be observed at different angles which would make its width fluctuate.
		// You can view it side-on and it would be incredibly thin but you can also view it top-down
		// and you would see its full width. I don't want this effect for the trails so I need to
		// make sure I am always looking at them top-down. So no matter where the camera is, I will
		// always see the ribbons with their width oriented to the camera. The one change I made for
		// this particular piece of source which has no camera object is I have replaced the eyeNormal
		// (which would be the vector pointing from ribbon towards camera) with a generic ci::Vec3f(0, 1, 0).
		// Why? Well cause it works and thats enough for me. WHEE!
		Vec3f perp0 = loc[i] - loc[i+1];
		Vec3f perp1 = perp0.cross( Vec3f::yAxis() );
		Vec3f perp2 = perp0.cross( perp1 );
			  perp1 = perp0.cross( perp2 ).normalized();
			  
		Vec3f off = perp1 * ( radius * agePer * per * 0.1f );

		glColor4f( per, per * 0.25f, 1.0f - per, per * 0.5f );
		glVertex3fv( loc[i] - off );
		glVertex3fv( loc[i] + off );
	}

	glEnd();
}

void Particle::setAge()
{
	if( ALLOWFLOOR ){
		if( ISBOUNCING ){
			age += bounceAge;
			bounceAge += 1.0f;
		}
		else {
			age += 0.25f;
		}
	}
	else {
		age += 1.0f;
	}

	if( age > lifeSpan ) {
		ISDEAD = true;
	}
	else {
		// When spawned, the agePer is 1.0.
		// When death occurs, the agePer is 0.0.
		agePer = 1.0f - age / (float)lifeSpan;
	}
}