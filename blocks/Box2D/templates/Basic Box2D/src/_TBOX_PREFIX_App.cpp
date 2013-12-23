#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#include <Box2D/Box2D.h>

using namespace ci;
using namespace ci::app;
using namespace std;

const float BOX_SIZE = 10;

class _TBOX_PREFIX_App : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
	
	void addBox( const Vec2f &pos );
	
	b2World				*mWorld;
	vector<b2Body*>		mBoxes;
};

void _TBOX_PREFIX_App::setup()
{
	b2Vec2 gravity( 0.0f, 10.0f );
	mWorld = new b2World( gravity );

	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set( 0.0f, getWindowHeight() );
	b2Body* groundBody = mWorld->CreateBody(&groundBodyDef);

	// Define the ground box shape.
	b2PolygonShape groundBox;

	// The extents are the half-widths of the box.
	groundBox.SetAsBox( getWindowWidth(), 10.0f );

	// Add the ground fixture to the ground body.
	groundBody->CreateFixture(&groundBox, 0.0f);
	
}

void _TBOX_PREFIX_App::addBox( const Vec2f &pos )
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set( pos.x, pos.y );

	b2Body *body = mWorld->CreateBody( &bodyDef );

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox( BOX_SIZE, BOX_SIZE );

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;
	fixtureDef.restitution = 0.5f; // bounce

	body->CreateFixture( &fixtureDef );
	mBoxes.push_back( body );
}

void _TBOX_PREFIX_App::mouseDown( MouseEvent event )
{
	addBox( event.getPos() );
}

void _TBOX_PREFIX_App::update()
{
	for( int i = 0; i < 10; ++i )
		mWorld->Step( 1 / 30.0f, 10, 10 );
}

void _TBOX_PREFIX_App::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
	
	gl::color( Color( 1, 0.5f, 0.25f ) );
	for( vector<b2Body*>::const_iterator boxIt = mBoxes.begin(); boxIt != mBoxes.end(); ++boxIt ) {
		Vec2f pos( (*boxIt)->GetPosition().x, (*boxIt)->GetPosition().y );
		float t = toDegrees( (*boxIt)->GetAngle() );

		glPushMatrix();
		gl::translate( pos );
		gl::rotate( t );

		Rectf rect( -BOX_SIZE, -BOX_SIZE, BOX_SIZE, BOX_SIZE );
		gl::drawSolidRect( rect );

		glPopMatrix();	
	}
}

CINDER_APP_NATIVE( _TBOX_PREFIX_App, RendererGl )
