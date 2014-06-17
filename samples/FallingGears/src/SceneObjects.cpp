#include "SceneObjects.h"
#include "AudioController.h"
#include "SceneController.h"

#include "cinder/Timeline.h"
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"

const size_t BASS_MAX = 48;

using namespace std;
using namespace ci;

// ----------------------------------------------------------------------------------------------------
// MARK: - SceneObject
// ----------------------------------------------------------------------------------------------------

void SceneObject::setPosition( const ci::Vec2f &pos )
{
	mBody->SetTransform( b2Vec2( pos.x, pos.y ), mBody->GetAngle() );
}

Vec2f SceneObject::getPos() const
{
	return Vec2f( mBody->GetPosition().x, mBody->GetPosition().y );
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

	Vec2f pos = Vec2f( mBody->GetPosition().x, mBody->GetPosition().y ) * pointsPerMeter;
	float t = toDegrees( mBody->GetAngle() );

	gl::pushModelView();
		gl::translate( pos );
		gl::rotate( t );
		gl::draw( mImageTex, imageDest );
	gl::popModelView();
}

// ----------------------------------------------------------------------------------------------------
// MARK: - Wall
// ----------------------------------------------------------------------------------------------------

void Wall::handleCollision( const Gear *gear, const Vec2f &contactPoint )
{
	const float maxVibration = 6.0f;

	mVibrationLevel.stop();
	mVibrationLevel = ( maxVibration + mVibrationLevel ) / 2;

	app::timeline().apply( &mVibrationLevel, 0.0f, 1.0f );

	float panPos;
	float freqMidi = mFundamentalMidi;

	if( mSide == Wall::Side::RIGHT ) {
		freqMidi += 7;
		panPos = 0.1f;
	}
	else
		panPos = 0.9f;

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
		Vec2f controlPoint1( wallRect.x2 + mVibrationLevel, wallRect.y1 + wallRect.getHeight() / 3.0f );
		Vec2f controlPoint2( wallRect.x2 + mVibrationLevel, wallRect.y1 + wallRect.getHeight() * 2.0f / 3.0f );
		vibrationPath.curveTo( controlPoint1, controlPoint2, wallRect.getLowerRight() );
	}

	vibrationPath.lineTo( wallRect.getLowerLeft() );

	// left vibrating wall
	{
		Vec2f controlPoint1( wallRect.x1 - mVibrationLevel, wallRect.y1 + wallRect.getHeight() * 2.0f / 3.0f );
		Vec2f controlPoint2( wallRect.x1 - mVibrationLevel, wallRect.y1 + wallRect.getHeight() / 3.0f );
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

void Island::handleCollision( const Gear *gear, const Vec2f &contactPoint )
{
	mAudio->getNextAltoSynth()->trigger( mFreqMidi, 1.4f );

	float pointsPerMeter = SceneController::getPointsPerMeter();

	for( size_t i = 0; i < mBumpers.size(); i++ ) {
		Vec2f centerPos = box2d::toCinder( mBody->GetPosition() ) * pointsPerMeter;
		Rectf bbox = mBumperBoundingBoxes[i] + centerPos;
		if( bbox.contains( contactPoint ) ) {
			auto &bumperAnim = mBumperVibrationLevels[i];
			bumperAnim.stop();
			bumperAnim = 1.0f;
			app::timeline().apply( &bumperAnim, 0.0f, 1.0f );
		}
	}

	mVibrationLevel.stop();
	mVibrationLevel = 0.4f;
	app::timeline().apply( &mVibrationLevel, 0.0f, 1.4f, EaseOutQuart() );
}

void Island::makeBumpers()
{
	CI_ASSERT( mOuterVerts.size() == 6 && mInnerVerts.size() == 5 );

	mBumpers.clear();
	mBumperBoundingBoxes.clear();
	mBumperVibrationLevels.clear();

	const float padding = 4;
	const float crookedPaddingPercent = 0.036f;
	const float boundingBoxExpansion = 1.1f;

	// left base
	{
		Path2d bumper;
		bumper.moveTo( mOuterVerts[0] );
		bumper.lineTo( mOuterVerts[1].x, mOuterVerts[1].y + padding );
		bumper.lineTo( mInnerVerts[1].x, mInnerVerts[1].y + padding );
		bumper.lineTo( mInnerVerts[0] );
		bumper.close();
		mBumpers.push_back( bumper );
	}

	// left top
	{
		Vec2f offsetOuter = ( mOuterVerts[2] - mOuterVerts[1] ) * crookedPaddingPercent;
		Vec2f offsetInner = ( Vec2f( mOuterVerts[2].x, mInnerVerts[2].y ) - mInnerVerts[1] ) * crookedPaddingPercent;

		Path2d bumper;
		bumper.moveTo( mOuterVerts[1] + offsetOuter );
		bumper.lineTo( mOuterVerts[2] );
		bumper.lineTo( mOuterVerts[2].x, mInnerVerts[2].y );
		bumper.lineTo( mInnerVerts[1] + offsetInner );
		bumper.close();
		mBumpers.push_back( bumper );
	}

	// right top
	{
		Vec2f offsetOuter = ( mOuterVerts[3] - mOuterVerts[4] ) * crookedPaddingPercent;
		Vec2f offsetInner = ( Vec2f( mOuterVerts[3].x, mInnerVerts[2].y ) - mInnerVerts[3] ) * crookedPaddingPercent;

		Path2d bumper;
		bumper.moveTo( mOuterVerts[3] );
		bumper.lineTo( mOuterVerts[4] + offsetOuter );
		bumper.lineTo( mInnerVerts[3] + offsetInner );
		bumper.lineTo( mOuterVerts[3].x, mInnerVerts[2].y );
		bumper.close();
		mBumpers.push_back( bumper );
	}

	// right base
	{
		Path2d bumper;
		bumper.moveTo( mOuterVerts[4].x, mOuterVerts[4].y + padding );
		bumper.lineTo( mOuterVerts[5] );
		bumper.lineTo( mInnerVerts[4] );
		bumper.lineTo( mInnerVerts[3].x, mInnerVerts[3].y + padding );
		bumper.close();
		mBumpers.push_back( bumper );
	}

	for( size_t i = 0; i < mBumpers.size(); i++ ) {
		mBumperVibrationLevels.push_back( 0 );

		// calculate an expanded bounding box for each bumper to do hit detection, ensuring that the entire edge is covered.
		Rectf bbox = mBumpers[i].calcBoundingBox();

		Vec2f center = bbox.getCenter();
		bbox -= center;
		bbox *= boundingBoxExpansion;
		bbox += center;

		mBumperBoundingBoxes.push_back( bbox );
	}
}

void Island::draw()
{
	float pointsPerMeter = SceneController::getPointsPerMeter();

	auto centerPos = box2d::toCinder( mBody->GetPosition() ) * pointsPerMeter;
	gl::pushModelView();
	gl::translate( centerPos );

	Path2d path;
	path.moveTo( mOuterVerts[0] );
	for( int i = 1; i < mOuterVerts.size(); ++i )
		path.lineTo( mOuterVerts[i] );

	Color color = Color::gray( 0.34f );
	color.r *= 1 - mVibrationLevel * 0.5;
	color.g *= 1 - mVibrationLevel * 0.2;
	gl::color( color );

	gl::drawSolid( path );

	for( size_t i = 0; i < mBumpers.size(); i++ ) {

		Color color = Color::gray( 0.42f );
		color.r *= 1 + mBumperVibrationLevels[i];

		gl::color( color );
		gl::drawSolid( mBumpers[i] );
	}

	gl::popModelView();
}
