#pragma once

#include "Quake.h"

#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"

class Earth {
public:
	Earth();

	void update();
	void draw();

	void drawQuakes();
	void drawQuakeLabelsOnBillboard( const ci::vec3 &aRight, const ci::vec3 &aUp );
	void drawQuakeLabelsOnSphere( const ci::vec3 &aEyeNormal, const float aEyeDist );

	void setQuakeLocTips();
	void repelLocTips();

	void addQuake( float aLat, float aLong, float aMag, const std::string &aTitle );

	void setMinMagToRender( float amt ) { mMinMagToRender = ci::math<float>::clamp( mMinMagToRender + amt, 2.0f, 8.0f ); mNumQuakes = 0; }

public:
	static const int MAX_NUMBER_OF_QUAKES = 1024;

	float mMinMagToRender;
	float mRadius;

	ci::vec3 mLoc;
	ci::vec3 mLightDir;

	ci::gl::Texture2dRef mTexDiffuse;
	ci::gl::Texture2dRef mTexNormal;
	ci::gl::Texture2dRef mTexMask;

	std::vector<Quake>   mQuakes;
	size_t               mNumQuakes;

	ci::gl::BatchRef     mEarth;
	ci::gl::BatchRef     mQuake;

	ci::gl::VboRef       mInstanceDataVbo;
};