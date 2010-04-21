#pragma once

#include "cinder/Ray.h"
#include "cinder/Camera.h"
#include "cinder/Surface.h"
#include "cinder/Perlin.h"
#include "cinder/Sphere.h"
#include "cinder/AxisAlignedBox.h"
#include "cinder/Quaternion.h"

class RayMarcher {
 public:
	RayMarcher( const ci::CameraPersp *aCamera );

	void		randomScene();
 
	void		render( ci::Surface8u *surface );
	void		renderScanline( int scanline, ci::Surface8u *surface );
	void		renderSceneGL();

 private:
	ci::ColorA		march( const ci::Ray &ray );
	float			marchSecondary( const ci::Ray &ray );
	float			sampleDensity( const ci::Vec3f &v );
	
	const ci::Camera			*mCamera;
	ci::Perlin					mPerlin;
	std::vector<ci::Sphere>		mSpheres;
	std::vector<ci::Sphere>		mTransformedSpheres;
	ci::AxisAlignedBox3f		mBoundingBox;
};
