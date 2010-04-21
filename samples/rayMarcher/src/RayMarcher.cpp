#include "RayMarcher.h"
#include "cinder/Rand.h"
#include "cinder/Sphere.h"
#include "cinder/AxisAlignedBox.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Light.h"

using namespace ci;
using std::vector;

RayMarcher::RayMarcher( const ci::CameraPersp *aCamera )
	: mCamera( aCamera ), mPerlin( 8 )
{
	mBoundingBox = AxisAlignedBox3f( Vec3f( -1, -1, -1 ) * 10, Vec3f( 1, 1, 1 ) * 10 );
	
	randomScene();
}

void RayMarcher::randomScene()
{
	mSpheres.clear();
	for( int s = 0; s < 50; ++s )
		mSpheres.push_back( Sphere( Rand::randVec3f() * Rand::randFloat( 8 ), Rand::randFloat( 1, 4 ) ) );
}

void RayMarcher::renderSceneGL()
{
	gl::Light light( gl::Light::DIRECTIONAL, 0 );
	light.setDirection( Vec3f( 1, 1, 1 ).normalized() );
	light.update( *mCamera );
	light.enable();

	glColor3f( 1, 1, 1 );
	for( vector<Sphere>::const_iterator sphIt = mSpheres.begin(); sphIt != mSpheres.end(); ++sphIt )
		gl::drawSphere( sphIt->getCenter(), sphIt->getRadius() );

	light.disable();		
}

void RayMarcher::render( ci::Surface8u *surface )
{
	Surface8u::Iter iter = surface->getIter();

	// transform the spheres

	int width = surface->getWidth();
	int height = surface->getHeight();
	float imageAspect = width / (float)height;
	Color8u backgroundColor( 0, 95, 249 );
	while( iter.line() ) {
		while( iter.pixel() ) {
			ColorA color = march( mCamera->generateRay( iter.x() / (float)width, 1.0f - iter.y() / (float)height, imageAspect ) );
			iter.r() = backgroundColor.r * ( 1.0f - color.a ) + color.r * 255;
			iter.g() = backgroundColor.g * ( 1.0f - color.a ) + color.g * 255;
			iter.b() = backgroundColor.b * ( 1.0f - color.a ) + color.b * 255;
		}
		iter.line();
	}	
}

void RayMarcher::renderScanline( int scanline, ci::Surface8u *surface )
{
	int width = surface->getWidth();
	int height = surface->getHeight();
	float imageAspect = width / (float)height;
	Color8u backgroundColor( 0, 95, 249 );
	Surface8u::Iter iter = surface->getIter( Area( 0, scanline, surface->getWidth(), scanline + 1 ) );
	while( iter.pixel() ) {
		ColorA color = march( mCamera->generateRay( iter.x() / (float)width, 1.0f - scanline / (float)height, imageAspect ) );
		iter.r() = backgroundColor.r * ( 1.0f - color.a ) + color.r * 255;
		iter.g() = backgroundColor.g * ( 1.0f - color.a ) + color.g * 255;
		iter.b() = backgroundColor.b * ( 1.0f - color.a ) + color.b * 255;
	}
}

float RayMarcher::sampleDensity( const Vec3f &v )
{
	float d = 0.0f;
	for( vector<Sphere>::const_iterator sphIt = mSpheres.begin(); sphIt != mSpheres.end(); ++sphIt ) {
		float rSquared = sphIt->getRadius() * sphIt->getRadius();
		float dSquared = v.distanceSquared( sphIt->getCenter() );
		if( dSquared < rSquared ) {
			d = std::max( d, 1.0f - math<float>::sqrt( dSquared / rSquared ) );
//			d += 1.0f - math<float>::sqrt( dSquared / rSquared );
		}
	}
	if( d > 0.001f ) {
		d *= d * 4;
		d *= 0.3f + mPerlin.fBm( v * 0.93f ) * 0.7f;
	}
	return d;
}

float RayMarcher::marchSecondary( const Ray &ray )
{
	const float RAY_EPSILON = 0.50f;
	float boxTimes[2];
	if( mBoundingBox.intersect( ray, boxTimes ) != 2 )
		return 0;
		
	Vec3f pointOfDeparture;
	if( boxTimes[0] >= 0 )
		pointOfDeparture = ray.calcPosition( boxTimes[0] );
	else
		pointOfDeparture = ray.calcPosition( boxTimes[1] );
	float span = ray.getOrigin().distance( pointOfDeparture );
	int numSteps = (int)( span / RAY_EPSILON );
	if( numSteps <= 0 ) 
		return 0;

	Vec3f step( ray.getDirection() );
	step *= RAY_EPSILON;

	Vec3f rayPos = ray.getOrigin(); 

	float result = 0;
	for( int i = 0; i < numSteps; ++i ) {
		float D = sampleDensity( rayPos ) * RAY_EPSILON;
		result += D * ( 1.0f - result );
		
		rayPos += step; 
	}

	return result;
}

ColorA RayMarcher::march( const Ray &ray )
{
	const float RAY_EPSILON = 0.25f;
	float boxTimes[2];
	if( mBoundingBox.intersect( ray, boxTimes ) < 2 )
		return ColorA::zero();

	Vec3f pos0, pos1;
	if( boxTimes[0] < boxTimes[1] ) {
		pos0 = ray.calcPosition( boxTimes[0] );
		pos1 = ray.calcPosition( boxTimes[1] );	
	}
	else {
		pos0 = ray.calcPosition( boxTimes[1] );
		pos1 = ray.calcPosition( boxTimes[0] );		
	}

	float span = pos0.distance( pos1 );
	int numSteps = (int)( span / RAY_EPSILON );
	if( numSteps <= 0 ) 
		return ColorA::zero();

	Vec3f step( ray.getDirection() );
	step *= RAY_EPSILON;

	Vec3f rayPos = pos0; 
	Vec3f lightVec = mCamera->getModelViewMatrix().transformVec( Vec3f( 1, 1, 1 ).normalized() );
	float transparency = 1.0f;
	ColorA result = ColorA::zero();
	for( int i = 0; i < numSteps; ++i ) {
		ColorA sample;
		float D = sampleDensity( rayPos ) * RAY_EPSILON;
		if( D >= 0.001f ) {
			sample.a = D;
			float t = sample.a * ( 1.0f - result.a );
			const float ambient = 0.45f;
			const float diffuse = 1.0f - ambient;
			float c = ambient + diffuse * ( 1.0f - marchSecondary( Ray( rayPos, lightVec ) ) );
			result += ColorA( c * t, c * t, c * t, t );

			if( transparency < 0.001f )
				break;
		}
		
		rayPos += step; 
	}

	if( result.r < 0 ) result.r = 0;
	if( result.r > 1 ) result.r = 1;
	if( result.g < 0 ) result.g = 0;
	if( result.g > 1 ) result.g = 1;	
	if( result.b < 0 ) result.b = 0;
	if( result.b > 1 ) result.b = 1;	
	return result;
}
