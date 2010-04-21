#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include "cinder/app/AppBasic.h"
#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"
#include "cinder/CinderMath.h"
#include "cinder/Camera.h"
#include "cinder/ImageIo.h"

#include "particles.h"
#include "util.h"
#include "Resources.h"

using namespace ci::app;
using namespace ci;

class WaterSimApp : public AppBasic {
 public:	
	void prepareSettings( Settings *settings );
	void setup();
	void resetParticles();
	void toggleInfo();

	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void mouseMove( MouseEvent event );
	void mouseUp( MouseEvent event );
	void keyDown( KeyEvent event );
	void update();
	void drawLogo();
	void drawArrow();
	void drawInfo();
	void draw();
	
 public:
	Grid		*grid;
	Particles	*particles;
	Vec2f		mGravityVector;
	int			mCounter;
	bool		mMousePressed;
	bool		mTrackingMouse;
	bool		mRenderInfo;
	gl::Texture	mLogo, mArrow, mInfo;
};


static float fluidphi( Grid &grid, float x, float y, int dropType, const Vec2f &center )
{
	// These are all different shaping functions
	//return y-0.5*grid.ly;
	//return min(sqrt(sqr(x-0.5*grid.lx)+sqr(y-0.625*grid.ly))-0.02*grid.ly, y-0.6*grid.ly);
	//return min(sqrt(sqr(x-0.3333*grid.lx)+sqr(y-0.71*grid.ly))-0.3*grid.ly, y-0.2*grid.ly);
	//return max(y-0.8*grid.ly, -sqrt(sqr(x-0.5*grid.lx)+sqr(y-0.2*grid.ly))+0.1*grid.lx);
	//return sqrt(sqr(x-0.5*grid.lx)+sqr(y-0.75*grid.ly))-0.15*grid.lx;
	//return 0.75*grid.lx-x;
	//return max(x-0.75*grid.lx, 0.25*grid.lx-x, y-0.75*grid.ly, 0.25*grid.ly-y);	
	float amt;
	if( dropType == 0 ) {
		amt = std::min(y-0.05*grid.ly, sqrt(sqr(x-center.x*grid.lx)+sqr(y-(1.0f-center.y)*grid.ly))-0.085*grid.lx);
	}
	else {
		amt = std::min(y-0.05*grid.ly, sqrt(sqr(x-center.x*grid.lx)+sqr(y-(1.0f-center.y)*grid.ly))-0.05*grid.lx);
	}
		
	return amt;
}

void project( Grid &grid, float &x, float &y, float current, float target, Vec2f center )
{
   float dpdx=(fluidphi(grid, x+1e-4, y, 1, center )-fluidphi(grid, x-1e-4, y, 1, center ))/2e-4;
   float dpdy=(fluidphi(grid, x, y+1e-4, 1, center )-fluidphi(grid, x, y-1e-4, 1, center ))/2e-4;
   float scale=(target-current)/sqrt(dpdx*dpdx+dpdy*dpdy);
   x += scale * dpdx;
   y += scale * dpdy;
}

void init_water_drop( Grid &grid, Particles &particles, int na, int nb, int dropType, Vec2f center = Vec2f(0.5f,0.05f) )
{
   int i, j, a, b;
   float x, y, phi;


   for( i = 1; i < grid.marker.nx - 1; ++i ) {
		for( j = 1; j< grid.marker.ny - 1; ++j ) {
			for( a = 0; a < na; ++a ) {
				for( b = 0; b < nb; ++b ) {
					x = (i+(a+0.1+0.8*rand()/(double)RAND_MAX)/na)*grid.h;
					y = (j+(b+0.1+0.8*rand()/(double)RAND_MAX)/nb)*grid.h;

					phi = fluidphi( grid, x, y, dropType, center );

					if( phi >- 0.25 * grid.h / na )
						continue;
					else if( phi >- 1.5 * grid.h / na ) {

						project(grid, x, y, phi, -0.75*grid.h/na, center);
						phi=fluidphi(grid, x, y, dropType, center );
						project(grid, x, y, phi, -0.75*grid.h/na, center);
						phi=fluidphi(grid, x, y, dropType, center );

					}
					particles.add_particle( Vec2f(x,y), Vec2f(0,0) );
				}
			}
		}
	}
}

void advance_one_step( Grid &grid, Particles &particles, double dt )
{
	for( int i=0; i < 5; ++i )
		particles.move_particles_in_grid( 0.2 * dt );
	particles.transfer_to_grid();
	grid.save_velocities();
	grid.add_gravity(dt);
	grid.compute_distance_to_fluid();
	grid.extend_velocity();
	grid.apply_boundary_conditions();
	grid.make_incompressible();
	grid.extend_velocity();
	grid.get_velocity_update();
	particles.update_from_grid();
}

void advance_one_frame(Grid &grid, Particles &particles, double frametime)
{
	double t = 0;
	double dt;
	bool finished = false;
	while( ! finished ) {
		dt = 2 * grid.CFL();
		if( t + dt >= frametime ) {
			dt = frametime - t;
			finished = true;
		}
		else if( t + 1.5 * dt >= frametime )
			dt = 0.5 * ( frametime - t );
		advance_one_step( grid, particles, dt );
		t += dt;
	}
}

void WaterSimApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 640, 480 );
	settings->setFullScreen( true );
	settings->setResizable( true );
	settings->setFrameRate( 60.0f );
}

void WaterSimApp::setup()
{
	mGravityVector = Vec2f( 0, 9.8f );
	grid = new Grid( mGravityVector, 30, 30, 1.0 );
	particles = new Particles( *grid );

	resetParticles();
	mCounter = 0;
	//mTrackingMouse = false;
	mRenderInfo = true;
	mLogo = gl::Texture( loadImage( loadResource( RES_LOGO ) ) );
	mArrow = gl::Texture( loadImage( loadResource( RES_ARROW ) ) );
	mInfo = gl::Texture( loadImage( loadResource( RES_INFO ) ) );
}

void WaterSimApp::resetParticles()
{
	particles->clear_particles();
}

void WaterSimApp::toggleInfo()
{
	mRenderInfo = !mRenderInfo;
}

void WaterSimApp::mouseDown( MouseEvent event )
{
	mMousePressed = true;
}

void WaterSimApp::mouseDrag( MouseEvent event )
{
	Vec2f delta = Vec2f( getWindowWidth(), getWindowHeight() ) / 2.0f - event.getPos();
	if( delta.lengthSquared() > 5 ) {
		mGravityVector = delta.normalized() * 9.8f;
		mGravityVector.y = -mGravityVector.y;
	}
}

void WaterSimApp::mouseMove( MouseEvent event )
{
	Vec2f delta = Vec2f( getWindowWidth(), getWindowHeight() ) / 2.0f - event.getPos();
	if( delta.lengthSquared() > 5 ) {
		mGravityVector = delta.normalized() * 9.8f;
		mGravityVector.y = -mGravityVector.y;
	}
}

void WaterSimApp::mouseUp( MouseEvent event )
{
	mMousePressed = false;
}
	
void WaterSimApp::keyDown( KeyEvent event )
{
	if( event.getCode() == KeyEvent::KEY_f ) {
		setFullScreen( ! isFullScreen() );
	}
	else if( event.getCode() == KeyEvent::KEY_ESCAPE ) {
		setFullScreen( false );
	}
	else if( event.getChar() == 'c' ) {
		resetParticles();
	}
	else if( event.getChar() == '/' || event.getChar() == '?' ) {
		toggleInfo();
	}
}

void WaterSimApp::update()
{
	grid->set_gravity( mGravityVector );
	advance_one_frame( *grid, *particles, 1./60 );
	mCounter ++;
	
	if( mCounter < 150 || mMousePressed )
		init_water_drop( *grid, *particles, 1, 1, 0 );
}

void WaterSimApp::drawLogo()
{
	const int LOGO_OFFSET_X = 35, LOGO_OFFSET_Y = 20;
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	gl::setMatricesWindow( getWindowSize() );
	glEnable( GL_TEXTURE_2D );

	mLogo.bind();
	if( mRenderInfo )
		glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	else
		glColor4f( 1.0f, 1.0f, 1.0f, 0.1f );

	glBegin( GL_QUADS );
		glTexCoord2f( 0, 0 ); glVertex2f( getWindowWidth() - mLogo.getWidth() - LOGO_OFFSET_X, getWindowHeight() - mLogo.getHeight() - LOGO_OFFSET_Y );
		glTexCoord2f( 1.0, 0.0f ); glVertex2f( getWindowWidth() - LOGO_OFFSET_X, getWindowHeight() - mLogo.getHeight() - LOGO_OFFSET_Y );
		glTexCoord2f( 1.0f, 1.0f ); glVertex2f( getWindowWidth() - LOGO_OFFSET_X, getWindowHeight() - LOGO_OFFSET_Y );
		glTexCoord2f( 0.0f, 1.0f );	glVertex2f( getWindowWidth() - mLogo.getWidth() - LOGO_OFFSET_X, getWindowHeight() - LOGO_OFFSET_Y );
	glEnd();
	glDisable( GL_TEXTURE_2D );
}

void WaterSimApp::drawArrow()
{
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	gl::setMatricesWindow( getWindowSize() );
	glEnable( GL_TEXTURE_2D );
	mArrow.bind();
	glColor4f( 0.5f, 0.0f, 0.0f, 1.0f );
	Vec2f center( getWindowWidth() / 2.0f, getWindowHeight() / 2.0f );
	float halfWidth = mArrow.getWidth() / 2.0f, halfHeight = mArrow.getHeight() / 2.0f;
	glTranslatef( center.x, center.y, 0 );
	glRotatef( toDegrees( math<float>::atan2( -mGravityVector.y, mGravityVector.x ) ) + 90.0f, 0, 0, 1 );
	glBegin( GL_QUADS );
		glTexCoord2f( 0, 0 ); glVertex2f( -halfWidth, -halfHeight );
		glTexCoord2f( 1.0, 0.0f ); glVertex2f( halfWidth, -halfHeight );
		glTexCoord2f( 1.0f, 1.0f ); glVertex2f( halfWidth, halfHeight );
		glTexCoord2f( 0.0f, 1.0f );	glVertex2f( -halfWidth, halfHeight );
	glEnd();
	glDisable( GL_TEXTURE_2D );
}

void WaterSimApp::drawInfo()
{
	const int LOGO_OFFSET_X = 15, LOGO_OFFSET_Y = 20;
	
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	gl::setMatricesWindow( getWindowSize() );
	glEnable( GL_TEXTURE_2D );
	mInfo.bind();
	
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	
	Vec2f center( getWindowWidth() - mInfo.getWidth() / 2.0f - LOGO_OFFSET_X, mInfo.getHeight() / 2.0f + LOGO_OFFSET_Y );
	float halfWidth = mInfo.getWidth() / 2.0f, halfHeight = mInfo.getHeight() / 2.0f;
	glTranslatef( center.x, center.y, 0 );
	glBegin( GL_QUADS );
		glTexCoord2f( 0, 0 ); glVertex2f( -halfWidth, -halfHeight );
		glTexCoord2f( 1.0, 0.0f ); glVertex2f( halfWidth, -halfHeight );
		glTexCoord2f( 1.0f, 1.0f ); glVertex2f( halfWidth, halfHeight );
		glTexCoord2f( 0.0f, 1.0f );	glVertex2f( -halfWidth, halfHeight );
	glEnd();
	glDisable( GL_TEXTURE_2D );
}

void WaterSimApp::draw()
{
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable( GL_MULTISAMPLE_ARB );
	
	ci::CameraOrtho cam( 0.05, 0.95, 0.05, 0.95, -1.0, 1.0 );
	ci::gl::setMatrices( cam );
	
	glColor3f( 1.0f, 1.0f, 1.0f );
	glBegin( GL_POINTS );
	for( unsigned int i = 0; i < particles->x.size(); ++i )
		glVertex2fv( particles->x[i] );
	glEnd();
	
	drawLogo();
	
	if( mRenderInfo ){
		drawArrow();
		drawInfo();
	}	
}

CINDER_APP_BASIC( WaterSimApp, RendererGl )
