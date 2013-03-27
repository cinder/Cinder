#include "Particle.h"
#include "cinder/Rand.h"
//#include "cinder/gl/gl.h"
#include "cinder/dx/dx.h"
#include "cinder/Color.h"
#include "cinder/Perlin.h"

using namespace ci;

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

	startLoc = _loc + Rand::randVec3f() * Rand::randFloat( 5.0f );

	for( int i = 0; i < len; i++ ) {
		loc.push_back( startLoc );
	}

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
	for( int i = len - 1; i > 0; i-- ) {
		loc[i] = loc[i-1];
	}
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
	//glBegin( GL_QUAD_STRIP );
	dx::begin(GL_QUADS); // GL_QUAD_STRIP );

	for( int i = 0; i < len - 2; i++ ) {
		float per     = 1.0f - i / (float)(len-1);
		Vec3f perp0 = loc[i] - loc[i+1];
		Vec3f perp1 = perp0.cross( Vec3f::yAxis() );
		Vec3f perp2 = perp0.cross( perp1 );
			  perp1 = perp0.cross( perp2 ).normalized();
			  
		Vec3f off = perp1 * ( radius * agePer * per * 0.1f );

		//glColor4f( per, per * 0.25f, 1.0f - per, per * 0.5f );
		//gl::vertex( loc[i] - off );
		//gl::vertex( loc[i] + off );

		dx::color( per, per * 0.25f, 1.0f - per, per * 0.5f );
		dx::vertex( loc[i] - off );
		dx::vertex( loc[i] + off );
	}

	//glEnd();
	dx::end();
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
		agePer = 1.0f - age / (float)lifeSpan;
	}
}