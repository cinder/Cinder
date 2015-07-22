#include "SceneObjects.h"
#include "AudioController.h"
#include "SceneController.h"

#include "cinder/Timeline.h"
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/Triangulate.h"

const size_t BASS_MAX = 48;

using namespace std;
using namespace ci;

// ----------------------------------------------------------------------------------------------------
// MARK: - SceneObject
// ----------------------------------------------------------------------------------------------------

void SceneObject::setPosition( const ci::vec2 &pos )
{
	mBody->SetTransform( b2Vec2( pos.x, pos.y ), mBody->GetAngle() );
}

vec2 SceneObject::getPos() const
{
	return vec2( mBody->GetPosition().x, mBody->GetPosition().y );
}

// ----------------------------------------------------------------------------------------------------
// MARK: - Gear
// ----------------------------------------------------------------------------------------------------

void Gear::draw()
{
	if( ! mImageTex )
		return;

	float pointsPerMeter = SceneController::getPointsPerMeter();

	Rectf imageDest( - mRadius, - mRadius, mRadius, mRadius );

	vec2 pos = vec2( mBody->GetPosition().x, mBody->GetPosition().y ) * pointsPerMeter;
	float t = mBody->GetAngle();

	gl::ScopedModelMatrix modelScope;
	gl::translate( pos );
	gl::rotate( t );
	gl::draw( mImageTex, imageDest );
}

// ----------------------------------------------------------------------------------------------------
// MARK: - Wall
// ----------------------------------------------------------------------------------------------------

void Wall::handleCollision( const Gear *gear, const vec2 &contactPoint )
{
	const float maxVibration = 6.0f;

	mVibrationLevel.stop();
	mVibrationLevel = ( maxVibration + mVibrationLevel ) / 2;

	app::timeline().apply( &mVibrationLevel, 0.0f, 1.0f );

	float panPos;
	float freqMidi = mFundamentalMidi;

	if( mSide == Wall::Side::RIGHT ) {
		freqMidi += 7;
		panPos = 0.9f; // almost all the way to the right
	}
	else
		panPos = 0.1f; // almost all the way to the left

	while( freqMidi > BASS_MAX )
		freqMidi -= 12;

	//	CI_LOG_V( "f0: " << mFundamentalMidi << ", freqMidi: " << freqMidi << ", bassMaxFreq: " << audio::midiToFreq( BASS_MAX ) << ", resulting bass note: " << audio::midiToFreq( freqMidi ) );

	auto bassSynth = mAudio->getNextBassSynth();
	bassSynth->setPan( panPos );
	bassSynth->trigger( freqMidi, 2.0f );
}

void Wall::draw()
{
	float pointsPerMeter = SceneController::getPointsPerMeter();
	auto pos = pointsPerMeter * mBody->GetPosition();

	Rectf wallRect( pos.x - mWidth, 0, pos.x + mWidth, app::getWindowHeight() );

	Path2d vibrationPath;
	vibrationPath.moveTo( wallRect.getUpperRight() );

	// right vibrating wall
	{
		vec2 controlPoint1( wallRect.x2 + mVibrationLevel, wallRect.y1 + wallRect.getHeight() / 3.0f );
		vec2 controlPoint2( wallRect.x2 + mVibrationLevel, wallRect.y1 + wallRect.getHeight() * 2.0f / 3.0f );
		vibrationPath.curveTo( controlPoint1, controlPoint2, wallRect.getLowerRight() );
	}

	vibrationPath.lineTo( wallRect.getLowerLeft() );

	// left vibrating wall
	{
		vec2 controlPoint1( wallRect.x1 - mVibrationLevel, wallRect.y1 + wallRect.getHeight() * 2.0f / 3.0f );
		vec2 controlPoint2( wallRect.x1 - mVibrationLevel, wallRect.y1 + wallRect.getHeight() / 3.0f );
		vibrationPath.curveTo( controlPoint1, controlPoint2, wallRect.getUpperLeft() );
	}

	vibrationPath.close();

	ColorA vibrationColor = ColorA::gray( 0.2f, 0.6f );
	vibrationColor.g += mVibrationLevel * 0.01f;
	vibrationColor.b += mVibrationLevel * 0.03f;

	gl::color( vibrationColor );
	gl::drawSolid( vibrationPath );

	ColorA wallColor = ColorA::gray( 0.2f, 1.0f );
	wallColor.g += mVibrationLevel * 0.01f;
	wallColor.b += mVibrationLevel * 0.015f;

	gl::color( wallColor );
	gl::drawSolidRect( wallRect );
}

// ----------------------------------------------------------------------------------------------------
// MARK: - Island
// ----------------------------------------------------------------------------------------------------

void Island::handleCollision( const Gear *gear, const vec2 &contactPoint )
{
	mAudio->getNextAltoSynth()->trigger( mFreqMidi, 1.4f );

	float pointsPerMeter = SceneController::getPointsPerMeter();

	for( auto &bumper : mBumpers ) {
		vec2 centerPos = box2d::toCinder( mBody->GetPosition() ) * pointsPerMeter;
		Rectf bbox = bumper.mBoundingBox + centerPos;
		if( bbox.contains( contactPoint ) ) {
			auto &bumperAnim = bumper.mVibrationLevel;
			bumperAnim.stop();
			bumperAnim = 1.0f;
			app::timeline().apply( &bumperAnim, 0.0f, 1.0f );
		}
	}

	mVibrationLevel.stop();
	mVibrationLevel = 0.4f;
	app::timeline().apply( &mVibrationLevel, 0.0f, 1.4f, EaseOutQuart() );
}

void Island::setupGeometry()
{
	CI_ASSERT( mOuterVerts.size() == 6 && mInnerVerts.size() == 5 );

	// create the body of the island
	{
		Path2d path;
		path.moveTo( mOuterVerts[0] );
		for( int i = 1; i < mOuterVerts.size(); ++i )
			path.lineTo( mOuterVerts[i] );

		mBatchBody = gl::Batch::create( Triangulator( path ).calcMesh(), gl::getStockShader( gl::ShaderDef().color() ) );
	}

	mBumpers.clear();

	const float padding = 4;
	const float crookedPaddingPercent = 0.036f;

	// left base
	{
		Path2d bumper;
		bumper.moveTo( mOuterVerts[0] );
		bumper.lineTo( mOuterVerts[1].x, mOuterVerts[1].y + padding );
		bumper.lineTo( mInnerVerts[1].x, mInnerVerts[1].y + padding );
		bumper.lineTo( mInnerVerts[0] );
		bumper.close();

		addBumper( bumper );
	}

	// left top
	{
		vec2 offsetOuter = ( mOuterVerts[2] - mOuterVerts[1] ) * crookedPaddingPercent;
		vec2 offsetInner = ( vec2( mOuterVerts[2].x, mInnerVerts[2].y ) - mInnerVerts[1] ) * crookedPaddingPercent;

		Path2d bumper;
		bumper.moveTo( mOuterVerts[1] + offsetOuter );
		bumper.lineTo( mOuterVerts[2] );
		bumper.lineTo( mOuterVerts[2].x, mInnerVerts[2].y );
		bumper.lineTo( mInnerVerts[1] + offsetInner );
		bumper.close();

		addBumper( bumper );
	}

	// right top
	{
		vec2 offsetOuter = ( mOuterVerts[3] - mOuterVerts[4] ) * crookedPaddingPercent;
		vec2 offsetInner = ( vec2( mOuterVerts[3].x, mInnerVerts[2].y ) - mInnerVerts[3] ) * crookedPaddingPercent;

		Path2d bumper;
		bumper.moveTo( mOuterVerts[3] );
		bumper.lineTo( mOuterVerts[4] + offsetOuter );
		bumper.lineTo( mInnerVerts[3] + offsetInner );
		bumper.lineTo( mOuterVerts[3].x, mInnerVerts[2].y );
		bumper.close();

		addBumper( bumper );
	}

	// right base
	{
		Path2d bumper;
		bumper.moveTo( mOuterVerts[4].x, mOuterVerts[4].y + padding );
		bumper.lineTo( mOuterVerts[5] );
		bumper.lineTo( mInnerVerts[4] );
		bumper.lineTo( mInnerVerts[3].x, mInnerVerts[3].y + padding );
		bumper.close();

		addBumper( bumper );
	}
}

void Island::addBumper( const ci::Path2d& path )
{
	const float boundingBoxExpansion = 1.1f;

	mBumpers.push_back( Bumper() );
	auto &bumper = mBumpers.back();
	bumper.mPath = path;
	bumper.mBatch = gl::Batch::create( Triangulator( path ).calcMesh(), gl::getStockShader( gl::ShaderDef().color() ) );
	bumper.mVibrationLevel = 0;

	// calculate an expanded bounding box for each bumper to do hit detection, ensuring that the entire edge is covered.
	Rectf bbox = bumper.mPath.calcBoundingBox();
	vec2 center = bbox.getCenter();
	bbox -= center;
	bbox *= boundingBoxExpansion;
	bbox += center;

	bumper.mBoundingBox = bbox;
}

void Island::draw()
{
	float pointsPerMeter = SceneController::getPointsPerMeter();

	auto centerPos = box2d::toCinder( mBody->GetPosition() ) * pointsPerMeter;
	gl::ScopedModelMatrix modelScope;
	gl::translate( centerPos );

	Color color = Color::gray( 0.34f );
	color.r *= 1 - mVibrationLevel * 0.5;
	color.g *= 1 - mVibrationLevel * 0.2;
	gl::color( color );

	mBatchBody->draw();

	for( const auto &bumper : mBumpers ) {
		Color color = Color::gray( 0.42f );
		color.r *= 1 + bumper.mVibrationLevel;

		gl::color( color );
		bumper.mBatch->draw();
	}
}
