#include "CarModule.h"
#include "flint/OBJLoader.h"

using namespace fli;

void CarModule::setup( fli::app::App *app )
{
	mAngle = 0;
	mAlive = false;
	mRoadTravelOffset = 0;
	
	mRoadTexture = new gl::Texture( loadImage( app->loadResource( "road.jpg" ) ) );
	
	mBoxShader = new gl::GLSLProg( app->loadResource( "car.vert" ), app->loadResource( "car.frag" ) );
	mRoadShader = new gl::GLSLProg( app->loadResource( "road.vert" ), app->loadResource( "road.frag" ) );
	
	OBJLoader bodyObj( app->loadResource( "carbody.obj" ), false );
	bodyObj.loadAll( &mBodyMesh );

	OBJLoader wheelObj( app->loadResource( "wheel.obj" ) );
	wheelObj.loadAll( &mWheelMesh );

	OBJLoader roadObj( app->loadResource( "road.obj" ) );
	roadObj.loadAll( &mRoadMesh );

	mRoadLength = mRoadMesh.calcBoundingBox().length().x;
	
	float y = 1.884f;
	float left = -4.7f;
	float right = 4.7f;
	float front = 10.0f;
	float back = -7.3f;
	mFrontRight = Vec3f( front, y, right );
	mFrontLeft = Vec3f( front, y, left );
	mBackRight = Vec3f( back, y, right );
	mBackLeft = Vec3f( back, y, left );	
}

void CarModule::update( fli::app::App *app, bool visible )
{
	if( visible ) {
		if( ! mAlive ) {
			mAlive = true;
			mAge = 1.0f / app->getFrameRate();
		}
		else
			mAge += 1.0f / app->getFrameRate();
	}
	else
		mAge = 0;
	
	if( mAge > 1.0f )
		mAngle += 0.46f;
	else
		mAngle += 0.48f * powf( 1.0f - mAge, 1.3 ) + 0.06f;
		
	mRoadTravelOffset += 2;
	if( mRoadTravelOffset >= 120 ) mRoadTravelOffset = 0;
}

void CarModule::drawWheel( const fli::Vec3f &pos, float angle )
{
	// Front right
	gl::pushMatrix();
		gl::translate( pos );
		gl::rotate( Quatf( Vec3f::zAxis(), angle ) );
		gl::draw( mWheelMesh );
	gl::popMatrix();
}

void CarModule::draw( fli::app::App *app, const fli::Vec4d &camLoc )
{
	gl::enableAlphaBlending();
	gl::enableDepthWrite();
	gl::enableDepthRead();
	glColor4f( 1, 1, 1, 1 );

	gl::scale( Vec3f( 1, 1, 1 ) * 15 );

	// draw the road
	mRoadTexture->enableAndBind();
	mRoadShader->bind();
	gl::pushMatrix();
		gl::translate( Vec3f( mRoadTravelOffset * -mRoadLength / 120, 0, 0 ) );
		gl::drawRange( mRoadMesh, mRoadTravelOffset * 60 * 2, ( 120 - mRoadTravelOffset ) * 60 * 2 );
	gl::popMatrix();
	gl::pushMatrix();
		gl::translate( Vec3f( mRoadLength - mRoadTravelOffset * mRoadLength / 120.0f, 0, 0 ) );
		gl::drawRange( mRoadMesh, 0, mRoadTravelOffset * 60 * 2 );
	gl::popMatrix();
	
	// draw the car
	gl::pushMatrix();
		gl::translate( Vec3f( 0, 0, 42.4 ) );
		
		mBoxShader->bind();
		mBoxShader->uniform( "camLoc", Vec3f( camLoc.x, camLoc.y, camLoc.z ) );
		mBoxShader->uniform( "color", Vec3f( 0, 0.35f, 0.96f ) );
		mBoxShader->uniform( "tex0", 0 );
		gl::draw( mBodyMesh );
		
		drawWheel( mFrontRight, mAngle );
		drawWheel( mFrontLeft, mAngle );
		drawWheel( mBackRight, mAngle );
		drawWheel( mBackLeft, mAngle );			
	gl::popMatrix();

	mBoxShader->unbind();
}