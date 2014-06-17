#pragma once

#include "Box2dUtils.h"

#include "cinder/gl/Texture.h"
#include "cinder/Tween.h"

class AudioController;
struct Gear;

struct SceneObject {
	SceneObject( AudioController *audio ) : mAudio( audio )	{}
	virtual ~SceneObject()	{}

	virtual void handleCollision( const Gear *gear, const ci::Vec2f &contactPoint ) = 0;
	virtual void draw() = 0;

	void		setPosition( const ci::Vec2f &pos );
	ci::Vec2f	getPos() const;

	AudioController*	mAudio;
	box2d::BodyRef		mBody;
	std::string			mTag;
};

struct Gear : public SceneObject {
	Gear( AudioController *audio ) : SceneObject( audio )	{}

	void handleCollision( const Gear *gear, const ci::Vec2f &contactPoint )	override	{}
	void draw() override;

	int64_t				mIdentifier;
	ci::gl::TextureRef	mImageTex;
	float				mRadius;
};

struct Island : public SceneObject {
	Island( AudioController *audio ) : SceneObject( audio ), mVibrationLevel( 0 )	{}

	void makeBumpers();

	void handleCollision( const Gear *gear, const ci::Vec2f &contactPoint ) override;
	void draw() override;

	std::vector<ci::Vec2f> mOuterVerts, mInnerVerts;

	std::vector<ci::Path2d>				mBumpers;
	std::vector<ci::Rectf>				mBumperBoundingBoxes;
	std::vector<ci::Anim<float> >		mBumperVibrationLevels;

	ci::Anim<float> mVibrationLevel;
	float			mFreqMidi;
};

struct Wall : public SceneObject {
	Wall( AudioController *audio ) : SceneObject( audio ), mVibrationLevel( 0 ), mFundamentalMidi( 0 ), mWidth( 0 )	{}

	void handleCollision( const Gear *gear, const ci::Vec2f &contactPoint ) override;
	void draw() override;

	ci::Anim<float> mVibrationLevel;
	float			mFundamentalMidi;
	float			mWidth;

	enum class Side	{ LEFT, RIGHT }		mSide;
};
