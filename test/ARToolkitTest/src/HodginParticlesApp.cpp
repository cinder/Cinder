#include "HodginParticlesApp.h"
#include "flint/Perlin.h"
#include "resource.h"

#include "Particle.h"
#include "Emitter.h"

using namespace fli;

// global variables
int			counter = 0;
float		floorLevel = 400.0f;
gl::Texture *particleImg, *emitterImg;
bool		ALLOWFLOOR = true;
bool		ALLOWGRAVITY = true;
bool		ALLOWPERLIN = false;
bool		ALLOWTRAILS = true;
uint8_t		FLOOR_AXIS = 2;
Vec3f		gravity( 0, 0, -1.95f );
const int	FLINT_FACTOR = 10; // how many times more particles than the Java version

void HodginParticlesApp::setup( app::App *app )
{
	particleImg = new gl::Texture( loadImage( app->loadResource( "particle.png", RES_PARTICLE_ID, "PNG" ) ) );
	emitterImg = new gl::Texture( loadImage( app->loadResource( "emitter.png", RES_PARTICLE_ID, "PNG" ) ) );
}

void HodginParticlesApp::update( app::App *app, bool visible )
{
	if( ! visible ) return;
	counter++;
	
	if( true ) {
		if( ALLOWTRAILS && ALLOWFLOOR ) {
			emitter.addParticles( 5 * FLINT_FACTOR );
		}
		else {
			emitter.addParticles( 10 * FLINT_FACTOR );
		}
	}
}

void HodginParticlesApp::draw( app::App *app, const Vec4d &camLoc )
{
	// to accommodate resizable screen, we'll recalculate where the floor should be every frame just in case it's changed
	floorLevel = 2 / 3.0f * app->getWindowHeight();
	
	// Turns on additive blending so we can draw a bunch of glowing images without
	// needing to do any depth testing.
	glDepthMask( GL_FALSE );
	glDisable( GL_DEPTH_TEST );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	
	emitter.exist( Offset( app->getWindowWidth() / 2, app->getWindowHeight() / 2 ) );	
}

// It would be faster to just make QUADS calls directly to the loc
// without dealing with pushing and popping for every particle. The reason
// I am doing it this longer way is due to a billboarding problem which will come
// up later on.
void renderImage( Vec3f _loc, float _diam, Color _col, float _alpha )
{
	glPushMatrix();
	glTranslatef( _loc.x, _loc.y, _loc.z );
	glScalef( _diam, _diam, _diam );
	glColor4f( _col.r, _col.g, _col.b, _alpha );
	glBegin( GL_QUADS );
		glTexCoord2f(0, 0);    glVertex2f(-.5, -.5);
		glTexCoord2f(1, 0);    glVertex2f( .5, -.5);
		glTexCoord2f(1, 1);    glVertex2f( .5,  .5);
		glTexCoord2f(0, 1);    glVertex2f(-.5,  .5);
	glEnd();
	glPopMatrix();
}
