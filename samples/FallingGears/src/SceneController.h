
#pragma once

#include "SceneObjects.h"

#include "cinder/gl/Texture.h"
#include "cinder/Path2d.h"

#include <Box2D/Box2D.h>
#include "Box2dUtils.h"

#include <memory>

class SceneController : public b2ContactListener {
  public:
	typedef std::vector<std::shared_ptr<Wall> >		WallContainerT;
	typedef std::vector<std::shared_ptr<Gear> >		GearContainerT;
	typedef std::vector<std::shared_ptr<Island> >	IslandContainerT;

	void setup( AudioController *audio );
	void reload();
	void update();
	void addGear( const ci::Vec2f &pos );

	b2World* getWorld() const		{ return mWorld.get(); }

	const GearContainerT&				getGears() const	{ return mGears; }
	const WallContainerT&					getWalls() const		{ return mWalls; }
	const std::vector<IslandContainerT>&	getIslands() const		{ return mIslands; }

	static float getPointsPerMeter();
	static float getMetersPerPoint();

	float getCurrentDecent() const	{ return mCurrentDecent; }

	struct Params {
		Params() : mGearScale( 0.5f ), mDecentSpeed( 0 ), mWallInset( 30 ), mWallWidth( 6 ),
			mBumperDefaultColor( ci::Color::gray( 0.42f ) ), mYOffsetVariance( 60 )
 		{}

		float mGearScale; // points
		float mDecentSpeed; // points per second (inital value set from config.json)
		float mYOffsetVariance;
		float mWallInset;
		float mWallWidth;

		ci::ColorA	mBumperDefaultColor;
	};

	Params& getParams()	{ return mParams; }

  private:

	void setupWalls();
	void setupFallingGears();
	void removeOffscreenGears();
	void scrollIslands( float deltaTime );
	std::shared_ptr<Island> makeIsland();


	void BeginContact( b2Contact* contact ) override;
	void handleIslandCollision( Island *island, SceneObject *object, const ci::Vec2f &contactPoint  );
	void handleWallCollision( Wall *wall, SceneObject *object, const ci::Vec2f &contactPoint  );
	void reConfigIslandGroup( IslandContainerT &islandGroup, float yOffset );

	ci::gl::TextureRef	getRandomGearTex() const;
	size_t calcNextFundamental();

	std::vector<ci::Vec2f> calcNextOuterBumperVerts( float baseHeight, float topHeight, float baseWidth, float topWidth ) const;
	std::vector<ci::Vec2f> calcInnerBumperVerts( float baseHeight, float topHeight, float baseWidth ) const;

	std::unique_ptr<b2World>		mWorld;
	AudioController*				mAudio;
	Params							mParams;

	float							mLastStepTime, mCurrentDecent;
	size_t							mLastFundamental;
	GearContainerT			mGears;
	WallContainerT					mWalls;
	std::vector<IslandContainerT>	mIslands;
	int64_t							mGearIdentifierTab;
};