#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include "flint/app/AppBasic.h"
#include "Box2D.h"

/**
 Work-in-progress getting box2D physics to play nice with flint.
 
 To do soon:
 + Create a decent scaling relationship between the window and the world so it's easy to position objects by pixel-coordinates
 + Create a decent abstraction of the bodies
**/

using namespace fli::app;
using std::vector;


const int	SCREEN_WIDTH = 1024;
const int	WORLD_WIDTH = 200;
const float PHYSICS_SCALING = float(SCREEN_WIDTH)/float(WORLD_WIDTH);
const float BOX_WIDTH = 1.0f;
const float BOX_HEIGHT = 1.0f;
const int	NUM_BODIES = 20;

float worldToScreen( float wc )
{
	return wc * PHYSICS_SCALING;
}
float screenToWorld( float sc )
{
	return sc / PHYSICS_SCALING;
}

class box2DTestApp : public AppBasic {
public:
	void prepareSettings(Settings *settings);
	void setup();
	void update();
	void draw();
	void mouseDown( MouseEvent event );
	void mouseUp( MouseEvent event );
	void scrollWheel(float f);
	void keyDown(KeyEvent event);
	void keyUp( char c );

private:
	b2World *mWorld;
	b2Body* ground;
	b2Body* body;
	float32 timeStep;
	int32 iterations;
	vector<b2Body*> bodies;
	typedef vector<b2Body*>::iterator bodyIterator;
};
void box2DTestApp::prepareSettings( Settings *settings )
	{
		settings->setWindowSize( 1024, 768 );
		settings->setFullScreenSize( 1024, 768 );
		settings->setFullScreen( false );
		settings->setResizable( true );
		settings->setFrameRate( 30.0f );
	}

	void box2DTestApp::setup()
	{
		b2AABB worldAABB;
		worldAABB.lowerBound.Set( -100.0f, -100.0f );
		worldAABB.upperBound.Set( 100.0f, 100.0f );
		
		b2Vec2 gravity( 0.0f, 10.0f );
		bool doSleep = true;
		
		mWorld = new b2World( worldAABB, gravity, doSleep );
		b2BodyDef groundBodyDef;
		groundBodyDef.position.Set( 0.0f, 50.0f );
		ground = mWorld->CreateBody( &groundBodyDef );
		
		b2PolygonDef groundShapeDef;
		groundShapeDef.SetAsBox( 50.0f, 10.0f );
		ground->CreateShape(&groundShapeDef);
		
		b2BodyDef bodyDef;
		b2PolygonDef shapeDef;
		for( int i=0; i < NUM_BODIES; i++ )
		{
			bodyDef.position.Set(5.0f + i,0.0f);
			bodyDef.angle = 16.0f;	//currently not getting updated by the simulation step: weird
			body = mWorld->CreateBody(&bodyDef);
			
			shapeDef.SetAsBox( 1.0f, 1.0f);
			shapeDef.density = 1.0f;
			shapeDef.friction = 0.3f;
			body->CreateShape(&shapeDef);
			body->SetMassFromShapes();
			
			bodies.push_back( body );
		}
		
		timeStep = 1.0f/60.0f;
		iterations = 10;
		
		std::cout << "world to screen: " << worldToScreen(120.0f) << std::endl;
	}

	void box2DTestApp::update()
	{
		mWorld->Step( timeStep, iterations );		
	}

	void box2DTestApp::draw()
	{
		glClearColor( 0.0f, 0.1f, 0.2f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glEnable( GL_MULTISAMPLE_ARB );
		///*
		for( vector<b2Body*>::size_type i=0; i != bodies.size(); ++i )
		{
			glPushMatrix();
			b2Vec2 position = bodies[i]->GetPosition();
			float32 angle = bodies[i]->GetAngle();
			glTranslatef( position.x*PHYSICS_SCALING, position.y*PHYSICS_SCALING, 0 );
			glRotatef( angle, 0.0, 0.0, 1.0 );
				glBegin( GL_QUADS );
				glVertex3f( -BOX_WIDTH*PHYSICS_SCALING, -BOX_HEIGHT*PHYSICS_SCALING, 0.0f );
				glVertex3f(  BOX_WIDTH*PHYSICS_SCALING, -BOX_HEIGHT*PHYSICS_SCALING, 0.0f );
				glVertex3f(  BOX_WIDTH*PHYSICS_SCALING,  BOX_HEIGHT*PHYSICS_SCALING, 0.0f );
				glVertex3f( -BOX_WIDTH*PHYSICS_SCALING,  BOX_HEIGHT*PHYSICS_SCALING, 0.0f );
				glEnd();		
			glPopMatrix();
		}
		// */
	}

	void box2DTestApp::mouseDown( MouseEvent event )
	{		
		if( event.isLeftDown() ) {
			
		}
		else if( event.isRightDown() ) {
			hideCursor();
		}
		else if( event.isMiddleDown() ) {
			showCursor();
		}
	}
	
	void box2DTestApp::mouseUp( MouseEvent event )
	{
		if (event.isLeft()) {
			// std::cout << "left up\n";
		}
		
		if (event.isRight()) {
			std::cout << "right up\n";
		}
		
		if (event.isMiddle()) {
			std::cout << "middle up\n";
		}
		
		if (event.isAltDown()) {
			std::cout << "alt up\n";
		}
		
		if (event.isShiftDown()) {
			std::cout << "shift up\n";
		}
		if (event.isControlDown()) {
			std::cout << "ctrl up\n";
		}
		
		if (event.isMetaDown()) {
			std::cout << "META up\n";
		}
	}
	
	void box2DTestApp::scrollWheel( float f ) {
		std::cout << "scroll wheel delta= " << f << "\n";
	}
	
	void box2DTestApp::keyDown( KeyEvent event ) {
		if( event.getCode() == KeyEvent::KEY_f ) {
			setFullScreen( true );
		}
		else if( event.getCode() == KeyEvent::KEY_ESCAPE ) {
			setFullScreen( false );
		}
	}
	
	void box2DTestApp::keyUp( char c ) {
		std::cout << "key up: " << c << "\n";
	}


FLI_APP_BASIC( box2DTestApp );
