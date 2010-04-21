#pragma once

#include "Quake.h"
#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"
#include <list>
#include <string>

class Earth {
 public:
	Earth();
	Earth( ci::gl::Texture aTexDiffuse, ci::gl::Texture aTexNormal, ci::gl::Texture aTexMask );
	
	void setQuakeLocTip();
	void update();
	void repelLocTips();
	void draw();
	void drawQuakes();
	void drawQuakeLabelsOnBillboard( const ci::Vec3f &aRight, const ci::Vec3f &aUp );
	void drawQuakeLabelsOnSphere( const ci::Vec3f aEyeNormal, const float aEyeDist );
	void drawQuakeVectors();
	void addQuake( float aLat, float aLong, float aMag, std::string aTitle );
	void setMinMagToRender( float amt );
	ci::Vec3f mLoc;
	float mRadius;
	ci::gl::Texture mTexDiffuse;
	ci::gl::Texture mTexNormal;
	ci::gl::Texture mTexMask;
	std::list<Quake> mQuakes;
	float mMinMagToRender;
};