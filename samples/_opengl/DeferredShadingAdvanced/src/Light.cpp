#include "Light.h"

using namespace ci;
using namespace std;

Light::Light() 
: mColorAmbient( ColorAf::black() ), 
mColorDiffuse( ColorAf::white() ), mColorSpecular( ColorAf::white() ), 
mIntensity( 1.0f ), mPosition( vec3( 0.0f ) ), mPad0( 0 ), mPad1( 0 ),
mRadius( 1.0f ), mVolume( 10.0f )
{
	setPosition( vec3( 0.0f ) );
}

Light& Light::colorAmbient( const ColorAf& c )
{
	mColorAmbient = c;
	return *this;
}

Light& Light::colorDiffuse( const ColorAf& c )
{
	mColorDiffuse = c;
	return *this;
}

Light& Light::colorSpecular( const ColorAf& c )
{
	mColorSpecular = c;
	return *this;
}

Light& Light::position( const vec3& v )
{
	mPosition = v;
	return *this;
}

Light& Light::intensity( float v )
{
	mIntensity = v;
	return *this;
}

Light& Light::radius( float v )
{
	mRadius = v;
	return *this;
}

Light& Light::volume( float v )
{
	mVolume = v;
	return *this;
}

const ColorAf& Light::getColorAmbient() const
{
	return mColorAmbient;
}

const ColorAf& Light::getColorDiffuse() const
{
	return mColorDiffuse;
}

const ColorAf& Light::getColorSpecular() const
{
	return mColorSpecular;
}

float Light::getIntensity() const
{
	return mIntensity;
}

const vec3& Light::getPosition() const
{
	return mPosition;
}

float Light::getRadius() const
{
	return mRadius;
}

float Light::getVolume() const
{
	return mVolume;
}

void Light::setColorAmbient( const ColorAf& c )
{
	mColorAmbient = c;
}

void Light::setColorDiffuse( const ColorAf& c )
{
	mColorDiffuse = c;
}

void Light::setColorSpecular( const ColorAf& c )
{
	mColorSpecular = c;
}

void Light::setPosition( const vec3& v )
{
	mPosition = v;
}

void Light::setIntensity( float v )
{
	mIntensity = v;
}

void Light::setRadius( float v )
{
	mRadius = v;
}

void Light::setVolume( float v )
{
	mVolume = v;
}
