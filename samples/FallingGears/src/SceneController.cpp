
#include "SceneController.h"
#include "Config.h"

#include "cinder/Log.h"
#include "cinder/Rand.h"
#include "cinder/CinderAssert.h"
#include "cinder/app/App.h"

#include <array>

#include "cinder/audio/Utilities.h"

const float METERS_TO_POINTS = 100;
const float POINTS_TO_METERS = 0.01f;

const float FUNNEL_MAX_HEIGHT = 150; // in points (screen resolution)

const size_t FUNDAMENTAL_MIN = 50;
const size_t FUNDAMENTAL_MAX = 55;

using namespace std;
using namespace ci;

namespace {

float metersToPoints( float var )			{ return var * METERS_TO_POINTS; }
vec2 metersToPoints( const vec2 &var )	{ return var * METERS_TO_POINTS; }
float pointsToMeters( float var )			{ return var * POINTS_TO_METERS; }
vec2 pointsToMeters( const vec2 &var )	{ return var * POINTS_TO_METERS; }
b2Vec2 pointsToMeters( const b2Vec2 &var )	{ return POINTS_TO_METERS * var; }

} // anonymous namespace

//static
float SceneController::getPointsPerMeter()
{
	return METERS_TO_POINTS;
}

//static
float SceneController::getMetersPerPoint()
{
	return POINTS_TO_METERS;
}

void SceneController::setup( AudioController *audio )
{
	mAudio = audio;

	mGearIdentifierTab = 0;
	mLastStepTime = 0;
	mCurrentDecent = 0;
	mLastFundamental = 0;

	b2Vec2 gravity( 0, 10 );

	mWorld.reset( new b2World( gravity ) );
	mWorld->SetContactListener( this );

	reload();
}

void SceneController::reload()
{
	mGearIdentifierTab = 0;
	mGears.clear();

	setupWalls();
	setupFallingGears();

	mParams.mDecentSpeed = Config::instance()->getDescentSpeed();
}

void SceneController::update()
{
	float currentTime = app::getElapsedSeconds();
	float deltaTime = currentTime - mLastStepTime;

	removeOffscreenGears();
	scrollIslands( deltaTime );

	mWorld->Step( deltaTime, 8, 3 );

	mLastStepTime = currentTime;
}

void SceneController::setupWalls()
{
	vec2 center = pointsToMeters( vec2( app::getWindowCenter() ) );
	float inset = pointsToMeters( mParams.mWallInset );

	vec2 size( center.x - inset, center.y - inset );

	b2BodyDef bodyDef;
	bodyDef.position.Set( inset, center.y );
	bodyDef.type = b2_staticBody;


	b2EdgeShape leftWallShape;
	leftWallShape.Set( b2Vec2( 0, - size.y ), b2Vec2( 0, size.y ) );

	b2FixtureDef leftWallFixture;
	leftWallFixture.shape = &leftWallShape;

	mWalls.push_back( make_shared<Wall>( mAudio ) );
	auto &leftWall = mWalls.back();

	leftWall->mSide = Wall::Side::LEFT;
	leftWall->mWidth = mParams.mWallWidth;
	leftWall->mTag = "left wall";
	leftWall->mBody = box2d::makeBodyShared( mWorld.get(), bodyDef );
	leftWall->mBody->CreateFixture( &leftWallFixture );
	leftWall->mBody->SetUserData( leftWall.get() );


	bodyDef.position.Set( pointsToMeters( app::getWindowWidth() ) - inset, center.y );

	b2EdgeShape rightWallShape;
	rightWallShape.Set( b2Vec2( 0, - size.y ), b2Vec2( 0, size.y ) );

	b2FixtureDef rightWallFixture;
	rightWallFixture.shape = &rightWallShape;

	mWalls.push_back( make_shared<Wall>( mAudio ) );
	auto &rightWall = mWalls.back();

	rightWall->mSide = Wall::Side::RIGHT;
	rightWall->mWidth = mParams.mWallWidth;
	rightWall->mTag = "right wall";
	rightWall->mBody = box2d::makeBodyShared( mWorld.get(), bodyDef );
	rightWall->mBody->CreateFixture( &rightWallFixture );
	rightWall->mBody->SetUserData( rightWall.get() );
}

void SceneController::setupFallingGears()
{
	mIslands.resize( 2 );

	float yOffset = 0;
	for( auto &islandGroup : mIslands ) {
		islandGroup.clear();
		for( size_t i = 0; i < 4; i++ ) {
			islandGroup.push_back( makeIsland() );
		}

		reConfigIslandGroup( islandGroup, yOffset );
		yOffset -= pointsToMeters( app::getWindowHeight() * 0.8f );
	}
}

vector<ci::vec2> SceneController::calcNextOuterBumperVerts( float baseHeight, float topHeight, float baseWidth, float topWidth ) const
{
	vector<vec2> result;

	result.emplace_back( - baseWidth / 2, 0 );
	result.emplace_back( - baseWidth / 2, - baseHeight );
	result.emplace_back( - topWidth / 2, - baseHeight - topHeight );
	result.emplace_back( topWidth / 2, - baseHeight - topHeight );
	result.emplace_back( baseWidth / 2, - baseHeight );
	result.emplace_back( baseWidth / 2, 0 );

	return result;
}

vector<vec2> SceneController::calcInnerBumperVerts( float baseHeight, float topHeight, float baseWidth ) const
{
	const float innerPercent = 0.7f;

	float innerBaseHeight = baseHeight * innerPercent;
	float innerTopHeight = topHeight * innerPercent;
	float innerBaseWidth = baseWidth * innerPercent;

	const float bottomOffset = - ( baseHeight - innerBaseHeight ) / 2 ;

	vector<vec2> result;

	result.emplace_back( - innerBaseWidth / 2, bottomOffset );
	result.emplace_back( - innerBaseWidth / 2, bottomOffset - innerBaseHeight );
	result.emplace_back( 0, bottomOffset - innerBaseHeight - innerTopHeight );
	result.emplace_back( innerBaseWidth / 2, bottomOffset - innerBaseHeight );
	result.emplace_back( innerBaseWidth / 2, bottomOffset );

	return result;
}

shared_ptr<Island> SceneController::makeIsland()
{
	const float baseHeight = randFloat( 70, 110 );
	const float topHeight = randFloat( 70, 90 );
	const float baseWidth = 140;
	const float topWidth = 8;

	auto island = make_shared<Island>( mAudio );
	island->mOuterVerts = calcNextOuterBumperVerts( baseHeight, topHeight, baseWidth, topWidth );
	island->mInnerVerts = calcInnerBumperVerts( baseHeight, topHeight, baseWidth );
	island->setupGeometry();

	vector<b2Vec2> vertsBox2d;

	for( size_t i = 0; i < island->mOuterVerts.size(); i++ )
		vertsBox2d.emplace_back( pointsToMeters( island->mOuterVerts[i].x ), pointsToMeters( island->mOuterVerts[i].y ) );

	b2PolygonShape polygon;
	polygon.Set( vertsBox2d.data(), vertsBox2d.size() );

	b2BodyDef islandBodyDef;
	islandBodyDef.type = b2_kinematicBody;

	island->mBody = box2d::makeBodyShared( mWorld.get(), islandBodyDef );
	island->mBody->CreateFixture( &polygon, 0.0f );
	island->mBody->SetUserData( island.get() );

	return island;
}

void SceneController::addGear( const vec2 &pos )
{
	float wallDeadZoneWidth = mParams.mWallInset + mParams.mWallWidth / 2;
	if( pos.x < wallDeadZoneWidth || pos.x > ( app::getWindowWidth() - wallDeadZoneWidth ) )
	   return;

	vec2 posScaled = pointsToMeters( pos );

	auto imageTex = getRandomGearTex();
	const float radius = mParams.mGearScale * imageTex->getWidth() / 2.0f;

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.allowSleep = false;
	bodyDef.position.Set( posScaled.x, posScaled.y );

	const float velMax = 5;
	bodyDef.linearVelocity = b2Vec2( randFloat( -velMax, velMax ), randFloat( -velMax, 0 ) );

	b2CircleShape shape;
	shape.m_radius = pointsToMeters( radius );

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 1.0f;
	fixtureDef.restitution = 0.4f;

	mGears.push_back( make_shared<Gear>( mAudio ) );
	auto &gear = mGears.back();
	gear->mTag = string( "gear" );
	gear->mIdentifier = mGearIdentifierTab++;
	gear->mImageTex = imageTex;
	gear->mRadius = radius;

	gear->mBody = box2d::makeBodyShared( mWorld.get(), bodyDef );
	gear->mBody->CreateFixture( &fixtureDef );
	gear->mBody->SetUserData( gear.get() );
}

ci::gl::TextureRef SceneController::getRandomGearTex() const
{
	const auto &gears = Config::instance()->getGearData();
	int32_t index = randInt( 0, gears.size() );

	return gears.at( index ).mImageTex;
}

void SceneController::removeOffscreenGears()
{
	float windowHeightMeters = pointsToMeters( app::getWindowHeight() );

	for( auto &gear : mGears ) {
		if( gear->mBody->GetPosition().y > windowHeightMeters )
			gear->mBody.reset();
	}

	mGears.erase( remove_if( mGears.begin(), mGears.end(),
								  []( const shared_ptr<Gear> &gear ) { return ! gear->mBody; } ),
					   mGears.end() );
}

void SceneController::scrollIslands( float deltaTime )
{
	float pointsPerSecond = mParams.mDecentSpeed;
	mCurrentDecent -= pointsPerSecond * deltaTime;

	float deltaDecentMeters = pointsToMeters( pointsPerSecond ) * deltaTime;
	float islandHeightMeters = pointsToMeters( FUNNEL_MAX_HEIGHT );

	for( auto &islandGroup : mIslands ) {
		bool needsReConfig = false;

		for( auto &island : islandGroup ) {
			b2Vec2 pos = island->mBody->GetPosition();
			pos.y -= deltaDecentMeters;

			if( pos.y < - islandHeightMeters ) {
				needsReConfig = true;
				break;
			}

			island->mBody->SetTransform( pos, 0 );
		}

		if( needsReConfig )
			reConfigIslandGroup( islandGroup, 0 );
	}
}

void SceneController::reConfigIslandGroup( IslandContainerT &islandGroup, float yOffset  )
{
	CI_ASSERT( islandGroup.size() == 4 );

	const size_t pitchMax = FUNDAMENTAL_MAX + 11;

	size_t f0 = calcNextFundamental();
	CI_LOG_V( "f0: " << f0 );

	bool isMinor = randBool();

	islandGroup[0]->mFreqMidi = f0;
	islandGroup[1]->mFreqMidi = f0 + ( isMinor ? 3 : 4 );;
	islandGroup[2]->mFreqMidi = f0 + 7;
	islandGroup[3]->mFreqMidi = f0 + ( isMinor ? 10 : 11 );

	float islandY = yOffset + pointsToMeters( app::getWindowHeight() + FUNNEL_MAX_HEIGHT + 200 );

	for( auto &island : islandGroup ) {
		float x = lmap<float>( island->mFreqMidi, FUNDAMENTAL_MIN, pitchMax, 0, app::getWindowWidth() );
		float perYOffset = pointsToMeters( randFloat( - mParams.mYOffsetVariance, mParams.mYOffsetVariance ) );
		island->setPosition( vec2( pointsToMeters( x ), islandY + perYOffset ) );
	}
}

// generates fundamental pitches that are contionusly decending in fifths, wraps if below FUNDAMENTAL_MIN
// note: we're going over the max here, but that's ok, it is only used to layout how far over right side of islands should sit.
size_t SceneController::calcNextFundamental()
{
	if( mLastFundamental == 0 )
		mLastFundamental = FUNDAMENTAL_MIN;

	size_t fundamental = mLastFundamental - 5;
	if( fundamental < FUNDAMENTAL_MIN )
		fundamental += 12;

	// update walls with current fundamental
	for( auto &wall : mWalls )
		wall->mFundamentalMidi = fundamental;

	mLastFundamental = fundamental;
	return fundamental;
}

void SceneController::BeginContact( b2Contact* contact )
{
	void *userDataA = contact->GetFixtureA()->GetBody()->GetUserData();
	void *userDataB = contact->GetFixtureB()->GetBody()->GetUserData();

	CI_ASSERT( userDataA && userDataB );

	b2WorldManifold worldManifold;
	contact->GetWorldManifold( &worldManifold );

	vec2 contactPoint = metersToPoints( box2d::toCinder( worldManifold.points[0] ) );

	SceneObject *objectA = static_cast<SceneObject *>( userDataA );
	SceneObject *objectB = static_cast<SceneObject *>( userDataB );

	if( typeid( *objectA ) == typeid( Island ) )
		handleIslandCollision( dynamic_cast<Island *>( objectA ), objectB, contactPoint );
	else if( typeid( *objectA ) == typeid( Wall ) )
		handleWallCollision( dynamic_cast<Wall *>( objectA ), objectB, contactPoint );
	else if( typeid( *objectB ) == typeid( Island ) )
		handleIslandCollision( dynamic_cast<Island *>( objectB ), objectA, contactPoint );
	else if( typeid( *objectB ) == typeid( Wall ) )
		handleWallCollision( dynamic_cast<Wall *>( objectB ), objectA, contactPoint );
}

void SceneController::handleIslandCollision( Island *island, SceneObject *object, const vec2 &contactPoint )
{
	Gear *gear = dynamic_cast<Gear *>( object );

	CI_ASSERT( island );
	CI_ASSERT( gear );

	island->handleCollision( gear, contactPoint );
}

void SceneController::handleWallCollision( Wall *wall, SceneObject *object, const vec2 &contactPoint  )
{
	Gear *gear = dynamic_cast<Gear *>( object );

	CI_ASSERT( wall );
	CI_ASSERT( gear );

	wall->handleCollision( gear, contactPoint );
}
