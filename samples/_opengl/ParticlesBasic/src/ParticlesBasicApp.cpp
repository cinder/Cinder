#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/Perlin.h"
#include "cinder/Color.h"

#include <vector>

using namespace ci;
using namespace ci::app;
using namespace std;

class Particle {
public:
	Particle( const vec2 &position )
		: mPosition( position ), mLastPosition( position ), mVelocity( vec2( 0 ) ), mZ( 0 ) {}

	void reset( const vec2 &position )
	{
		mPosition = mLastPosition = position;
		mVelocity = vec2( 0 );
		mZ = 0;
	}

	vec2 mPosition, mVelocity, mLastPosition;
	float mZ;
};

class BasicParticleApp : public App {
public:
	void	setup();
	void	update();
	void	draw();

	void	keyDown( KeyEvent event );

	bool	isOffscreen( const vec2 &v );

public:
	static const int	NUM_PARTICLES = 10000;

	float				mConservationOfVelocity;
	float				mSpeed;
	float				mAnimationCounter;

	Perlin				mPerlin;
	vector<Particle>	mParticles;
};

void BasicParticleApp::setup()
{
	// Randomize the Perlin noise function.
	mPerlin.setSeed( clock() );

	// Create particles.
	mParticles.reserve( NUM_PARTICLES );
	for( int s = 0; s < NUM_PARTICLES; ++s )
		mParticles.push_back( Particle( vec2( Rand::randFloat( getWindowWidth() ), Rand::randFloat( getWindowHeight() ) ) ) );

	mConservationOfVelocity = 0.9f;
	mSpeed = 5.0f;
	mAnimationCounter = 0.0f;
}

void BasicParticleApp::update()
{
	// Move ahead in time, which becomes the z-axis of our 3D noise.
	mAnimationCounter += 10.0f;

	for( auto &particle : mParticles ) {
		// Save off the last position for drawing lines.
		particle.mLastPosition = particle.mPosition;

		// Add some perlin noise to the velocity.
		vec3 deriv = mPerlin.dfBm( vec3( particle.mPosition.x, particle.mPosition.y, mAnimationCounter ) * 0.001f );
		particle.mZ = deriv.z;
		vec2 deriv2 = normalize( vec2( deriv.x, deriv.y ) );
		particle.mVelocity += deriv2 * mSpeed;

		// Move the particles according to their velocities.
		particle.mPosition += particle.mVelocity;

		// Dampen the velocities for the next frame.
		particle.mVelocity *= mConservationOfVelocity;

		// Replace any particles that have gone offscreen with a random onscreen position.
		if( isOffscreen( particle.mPosition ) )
			particle.reset( vec2( Rand::randFloat( getWindowWidth() ), Rand::randFloat( getWindowHeight() ) ) );
	}
}

void BasicParticleApp::draw()
{
	// Clear the window.
	gl::clear( Color::gray( 0.1f ) );

	// Enable additive blending.
	gl::ScopedBlendAdditive blend;

	// Draw all the particles as lines from mPosition to mLastPosition.
	// We use the convenience methods begin(), color(), vertex() and end() for simplicity,
	// see the ParticleSphere* samples for a faster method.
	gl::begin( GL_LINES );
	for( auto &particle : mParticles ) {
		// Color according to velocity.
		gl::color( 0.5f + particle.mVelocity.x / ( mSpeed * 2 ), 0.5f + particle.mVelocity.y / ( mSpeed * 2 ), 0.5f + particle.mZ * 0.5f );
		gl::vertex( particle.mLastPosition );
		gl::vertex( particle.mPosition );
	}
	gl::end();
}

void BasicParticleApp::keyDown( KeyEvent event )
{
	switch( event.getCode() ) {
		case KeyEvent::KEY_ESCAPE:
			if( isFullScreen() )
				setFullScreen( false );
			else
				quit();
			break;
		case KeyEvent::KEY_f:
			setFullScreen( !isFullScreen() );
			break;
		case KeyEvent::KEY_v:
			gl::enableVerticalSync( !gl::isVerticalSyncEnabled() );
			break;
	}
}

// Returns \c true if a given point is visible onscreen.
bool BasicParticleApp::isOffscreen( const vec2 &v )
{
	return ( ( v.x < 0 ) || ( v.x > getWindowWidth() ) || ( v.y < 0 ) || ( v.y > getWindowHeight() ) );
}

CINDER_APP( BasicParticleApp, RendererGl )