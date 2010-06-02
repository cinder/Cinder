#include "Branch.h"

#include "cinder/Rand.h"

using std::vector;

int Branch::sWindowWidth, Branch::sWindowHeight;

Branch::Branch( const Vec2f &aOrigin, float flowerHue, int launchDelay )
	: mOrigin( aOrigin )
{
	mIsRoot = true;
	float baseHue = 0.33333f;
	
	float sat = 0.5 + Rand::randFloat( 0.5f );
	float brightness = 1.0;
	float opacity = 0.8 + Rand::randFloat( 0.2f );
	mLaunchDelay = 0;
	ColorA flowerColor( CM_HSV, flowerHue, sat, brightness, opacity );

	for( int i = 0; i < INITIAL_SPAWN; i++ ) {
		createBranch( mOrigin, baseHue, flowerColor, launchDelay );
	}
}

Branch::Branch( const Vec2f &aOrigin, int aLifespan, float aSpeed, int aLaunchDelay, float aAngle, float aAngleDelta, float aChangeProb, float aFlowerProb,
	  float aStartEllipseRadius, float aEndEllipseRadius, ColorA aStartColor, ColorA aEndColor, ColorA aFlowerColor, float aScale )
	: mOrigin( aOrigin ), mPos( aOrigin )
{ 
	mIsRoot = false;
	mLifespan = (int)(aLifespan * aScale);
	mTotalLifespan = (int)(aLifespan * aScale);
	mAngle = aAngle;
	mSpeed = aSpeed;
	mLaunchDelay = aLaunchDelay;
	if( Rand::randBool() )
		mAngleDelta = aAngleDelta;
	else
		mAngleDelta = -aAngleDelta;
	mChangeProb = aChangeProb;
	mFlowerProb = aFlowerProb;
	mStartEllipseRadius = aStartEllipseRadius * aScale;
	mEndEllipseRadius = aEndEllipseRadius * aScale;
	mStartColor = aStartColor;
	mEndColor = aEndColor;
	mFlowerColor = aFlowerColor;
	mScale = aScale;
}

void Branch::update()
{ 
	if ( mLaunchDelay-- > 0 )
	  return;

	if( ( mLifespan > 0 ) && ( ! mIsRoot ) ) {
		mPos += Vec2f( cos( mAngle ), sin( mAngle ) ) * ( mSpeed * 2.0f * mScale );
		if( ! Area( 0, 0, sWindowWidth, sWindowHeight ).contains( mPos ) )
			mLifespan = 0;

		mAngle += mAngleDelta;
		mLifespan--;
		if( Rand::randFloat() < mChangeProb )
			mAngleDelta = -mAngleDelta;

		float ageLerp = 1.0 - mLifespan / (float)mTotalLifespan;
		if( Rand::randFloat() < mFlowerProb * ( 1.0 - ageLerp ) ) {
			if( mBranches.size() < MAX_BRANCHES ) {
				
				Branch newBranch( *this );
				newBranch.mBranches.clear();
				newBranch.mStartColor = lerp( mFlowerColor, ColorA( 0, 0, 0, 1.0f ), 0.5f );
				newBranch.mEndColor = mFlowerColor;
				newBranch.mFlowerProb = -1.0f;
				newBranch.mLifespan = (int)(30 * mScale); 
				newBranch.mTotalLifespan = newBranch.mLifespan;
				newBranch.mLaunchDelay = Rand::randInt( 40, 70 );
				newBranch.mStartEllipseRadius = newBranch.mEndEllipseRadius;
				newBranch.mEndEllipseRadius = 15.0f * mScale;
				newBranch.mAngleDelta *= 1.2f;
				newBranch.mAngle += 0.1f;
				mBranches.push_back( newBranch );
			}
		}
	}
  
	for( vector<Branch>::iterator blossomIt = mBranches.begin(); blossomIt != mBranches.end(); ++blossomIt ) {
		blossomIt->update();
	}
}

void Branch::draw( cairo::Context ctx )
{
	if ( mLaunchDelay > 0 )
	  return;

	if( ( mLifespan > 0 ) && ( ! mIsRoot ) ) {
		float ageLerp = 1.0 - mLifespan / (float)mTotalLifespan;
		float radius = lerp( mStartEllipseRadius, mEndEllipseRadius, ageLerp );
		ColorA drawColor = lerp( mStartColor, mEndColor, ageLerp );
		ctx.setSourceRgba( drawColor.r, drawColor.g, drawColor.b, 0.075f );
		ctx.circle( mPos, ( 5.0f * mScale + radius * 1.3f ) / 2 );
		ctx.fill();
		ctx.setSource( drawColor );
		ctx.circle( mPos, radius / 2 );
		ctx.fill();
	}
	
	for( vector<Branch>::iterator blossomIt = mBranches.begin(); blossomIt != mBranches.end(); ++blossomIt ) {
		blossomIt->draw( ctx );
	}
}

bool Branch::isAlive()
{
	if ( ( ! mIsRoot ) && ( mLifespan > 0 ) ) {
		return true;
	}
	else {
		for( vector<Branch>::iterator blossomIt = mBranches.begin(); blossomIt != mBranches.end(); ++blossomIt ) {
			if( blossomIt->isAlive() )
				return true;
		}
	}
  
	return false;
}

void Branch::createBranch( const Vec2f &origin, float baseHue, ColorA flowerColor, int launchDelay )
{
	float changeProb = 0.05f + Rand::randFloat( 0.05f );
	float flowerProb = 0.05f + Rand::randFloat( 0.21f );
	float startRadius = 5 + Rand::randFloat( 5 );
	float endRadius = Rand::randFloat( 1 );
	int lifespan = 100 + Rand::randInt( 100 );
	float speed = Rand::randFloat( 0.9f, 1.0f );
	float angleDelta = Rand::randFloat( 0.02f, 0.15f );
	float hue = baseHue + Rand::randFloat( 0.1f );
	float sat = 0.5 + Rand::randFloat( 0.5f );
	float brightness = 1.0f;
	float opacity = 0.5 + Rand::randFloat( 0.5 );
	ColorA startColor = ColorA( CM_HSV, hue, sat, brightness, opacity );
	ColorA endColor = ColorA( CM_HSV, hue + 0.1, sat, brightness / 2.0, opacity );
	float scale = Rand::randFloat( 0.5f, 1.0f );

	mBranches.push_back( Branch( origin, lifespan, speed, launchDelay, Rand::randFloat( 6.28f ), angleDelta, 
		changeProb, flowerProb, startRadius, endRadius, startColor, endColor, flowerColor, scale ) );
}


float Branch::randomHue()
{
	if( Rand::randFloat( 0.6666666666f ) > 0.5f ) // in [ 0, 60 ]
		return Rand::randFloat( 0 / 360.0f, 60.0f / 360.0f );
	else
		return Rand::randFloat( 190.0f / 360.0f, 360.0f / 360.0f );
}