#pragma once

#include "Box2dUtils.h"

#include "cinder/gl/Texture.h"
#include "cinder/gl/Batch.h"
#include "cinder/Tween.h"
#include "cinder/Path2d.h"

class AudioController;
struct Gear;

struct SceneObject {
	SceneObject( AudioController *audio ) : mAudio( audio )	{}
	virtual ~SceneObject()	{}

	virtual void handleCollision( const Gear *gear, const ci::vec2 &contactPoint ) = 0;
	virtual void draw() = 0;

	void		setPosition( const ci::vec2 &pos );
	ci::vec2	getPos() const;

	AudioController*	mAudio;
	box2d::BodyRef		mBody;
	std::string			mTag;
};

struct Gear : public SceneObject {
	Gear( AudioController *audio ) : SceneObject( audio )	{}

	void handleCollision( const Gear *gear, const ci::vec2 &contactPoint )	override	{}
	void draw() override;

	int64_t				mIdentifier;
	ci::gl::TextureRef	mImageTex;
	float				mRadius;
};

struct Island : public SceneObject {
	Island( AudioController *audio ) : SceneObject( audio ), mVibrationLevel( 0 )	{}

	void setupGeometry();
	void addBumper( const ci::Path2d& path );

	void handleCollision( const Gear *gear, const ci::vec2 &contactPoint ) override;
	void draw() override;

	std::vector<ci::vec2> mOuterVerts, mInnerVerts;

	struct Bumper {
		ci::gl::BatchRef	mBatch;
		ci::Path2d			mPath;
		ci::Rectf			mBoundingBox;
		ci::Anim<float>		mVibrationLevel;
	};

	std::vector<Bumper>	mBumpers;
	ci::Anim<float>		mVibrationLevel;
	float				mFreqMidi;
	ci::gl::BatchRef	mBatchBody;
};

struct Wall : public SceneObject {
	Wall( AudioController *audio ) : SceneObject( audio ), mVibrationLevel( 0 ), mFundamentalMidi( 0 ), mWidth( 0 )	{}

	void handleCollision( const Gear *gear, const ci::vec2 &contactPoint ) override;
	void draw() override;

	ci::Anim<float> mVibrationLevel;
	float			mFundamentalMidi;
	float			mWidth;

	enum class Side	{ LEFT, RIGHT }		mSide;
};
