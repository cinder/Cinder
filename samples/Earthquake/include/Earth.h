#pragma once

#include "Quake.h"
#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"
#include <list>
#include <string>

class Earth {
 public:
	Earth();
	Earth( const ci::gl::Texture2dRef &aTexDiffuse, const ci::gl::Texture2dRef &aTexNormal, const ci::gl::Texture2dRef &aTexMask );
	
	void setQuakeLocTip();
	void update();
	void repelLocTips();
	void draw();
	void drawQuakes();
	void drawQuakeLabelsOnBillboard( const ci::vec3 &aRight, const ci::vec3 &aUp );
	void drawQuakeLabelsOnSphere( const ci::vec3 aEyeNormal, const float aEyeDist );
	void drawQuakeVectors();
	void addQuake( float aLat, float aLong, float aMag, std::string aTitle );
	void setMinMagToRender( float amt );
	ci::vec3 mLoc;
	float mRadius;
	ci::gl::Texture2dRef mTexDiffuse;
	ci::gl::Texture2dRef mTexNormal;
	ci::gl::Texture2dRef mTexMask;
	std::list<Quake> mQuakes;
	float mMinMagToRender;
};