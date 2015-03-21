#include "Light.h"

using namespace ci;
using namespace std;

Light::Light() 
: mColorAmbient( Colorf::black() ), 
mColorDiffuse( Colorf::white() ), mColorSpecular( Colorf::white() ),
mIntensity( 1.0f ), mPosition( vec3( 0.0f ) ), mRadius( 1.0f ),
mVolume( 10.0f )
{
	setPosition( vec3( 0.0f ) );
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

Light& Light::setColorAmbient( const ColorAf& c )
{
	mColorAmbient = c;
	return *this;
}

Light& Light::setColorDiffuse( const ColorAf& c )
{
	mColorDiffuse = c;
	return *this;
}

Light& Light::setColorSpecular( const ColorAf& c )
{
	mColorSpecular = c;
	return *this;
}

Light& Light::setPosition( const vec3& v )
{
	mPosition = v;
	return *this;
}

Light& Light::setIntensity( float v )
{
	mIntensity = v;
	return *this;
}

Light& Light::setRadius( float v )
{
	mRadius = v;
	return *this;
}

Light& Light::setVolume( float v )
{
	mVolume = v;
	return *this;
}
