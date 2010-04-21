#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/Perlin.h"
#include "cinder/Color.h"
#include "cinder/gl/gl.h"

#include <list>
using std::list;

using namespace ci;
using namespace ci::app;

class Particle {
 public:
	Particle( Vec2f aPosition )
		: mPosition( aPosition ), mLastPosition( aPosition ), mVelocity( Vec2f::zero() ), mZ( 0 )
	{}
 
	Vec2f mPosition, mVelocity, mLastPosition;
	float mZ;
};

class BasicParticleApp : public AppBasic {
 public:	
	void	setup();
	void	mouseDown( MouseEvent event );
	void	keyDown( KeyEvent event );
	
	bool	isOffscreen( const Vec2f &v );
	
	void	update();
	void	draw();	

	float				CONSERVATION_OF_VELOCITY, SPEED;
	static const int	NUM_INITIAL_PARTICLES = 10000;
	static const int	NUM_CLICKED_PARTICLES = 1000;
	
	Perlin			mPerlin;
	list<Particle>	mParticles;
	float			mAnimationCounter;
};

void BasicParticleApp::setup()
{
	mPerlin.setSeed( clock() );

	mAnimationCounter = 0;
	for( int s = 0; s < NUM_INITIAL_PARTICLES; ++s )
		mParticles.push_back( Particle( Vec2f( Rand::randFloat( getWindowWidth() ), Rand::randFloat( getWindowHeight() ) ) ) );

	CONSERVATION_OF_VELOCITY = 0.9f;
	SPEED = 5.0f;

	// Turn on additive blending
	gl::enableAlphaBlending();
}

void BasicParticleApp::mouseDown( MouseEvent event )
{
	// Add a bunch of random particles at the mouse point
	for( int s = 0; s < NUM_CLICKED_PARTICLES; ++s )
		mParticles.push_back( Particle( Vec2f( event.getPos() ) + Rand::randVec2f() * Rand::randFloat( 60 ) ) );
}

void BasicParticleApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' )
		setFullScreen( ! isFullScreen() );
	else if( event.getChar() == 'x' )
		mParticles.clear();
}

// Returns whether a given point is visible onscreen or not
bool BasicParticleApp::isOffscreen( const Vec2f &v )
{
	return ( ( v.x < 0 ) || ( v.x > getWindowWidth() ) || ( v.y < 0 ) || ( v.y > getWindowHeight() ) );
}

void BasicParticleApp::update()
{
	mAnimationCounter += 10.0f; // move ahead in time, which becomes the z-axis of our 3D noise

	// Save off the last position for drawing lines
	for( list<Particle>::iterator partIt = mParticles.begin(); partIt != mParticles.end(); ++partIt )
		partIt->mLastPosition = partIt->mPosition;

	// Add some perlin noise to the velocity
	for( list<Particle>::iterator partIt = mParticles.begin(); partIt != mParticles.end(); ++partIt ) {
		Vec3f deriv = mPerlin.dfBm( Vec3f( partIt->mPosition.x, partIt->mPosition.y, mAnimationCounter ) * 0.001f );
		partIt->mZ = deriv.z;
		Vec2f deriv2( deriv.x, deriv.y );
		deriv2.normalize();
		partIt->mVelocity += deriv2 * SPEED;
	}
		
	// Move the particles according to their velocities
	for( list<Particle>::iterator partIt = mParticles.begin(); partIt != mParticles.end(); ++partIt )
		partIt->mPosition += partIt->mVelocity;

	// Dampen the velocities for the next frame
	for( list<Particle>::iterator partIt = mParticles.begin(); partIt != mParticles.end(); ++partIt )
		partIt->mVelocity *= CONSERVATION_OF_VELOCITY;
		
	// Replace any particles that have gone offscreen with a random onscreen position
	for( list<Particle>::iterator partIt = mParticles.begin(); partIt != mParticles.end(); ++partIt ) {
		if( isOffscreen( partIt->mPosition ) )
			*partIt = Particle( Vec2f( Rand::randFloat( getWindowWidth() ), Rand::randFloat( getWindowHeight() ) ) );
	}
}

void BasicParticleApp::draw()
{
	// clear out the screen
	gl::clear( Color( 0.1f, 0.1f, 0.1f ) );
	
	// draw all the particles as lines from mPosition to mLastPosition
	glBegin( GL_LINES );
	for( list<Particle>::iterator partIt = mParticles.begin(); partIt != mParticles.end(); ++partIt ) {
		// color according to velocity
		glColor3f( 0.5f + partIt->mVelocity.x / ( SPEED * 2 ), 0.5f + partIt->mVelocity.y / ( SPEED * 2 ), 0.5f + partIt->mZ * 0.5f );
		glVertex2f( partIt->mLastPosition );
		glVertex2f( partIt->mPosition );
	}
	glEnd();
}


CINDER_APP_BASIC( BasicParticleApp, RendererGl )