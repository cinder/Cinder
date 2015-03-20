#include "Material.h"

using namespace ci;

Material::Material()
: mColorAmbient( ColorAf::gray( 0.2f ) ), mColorDiffuse( ColorAf::gray( 0.8f ) ), 
mColorEmission( ColorAf::black() ), mColorSpecular( ColorAf::black() ), 
mShininess( 0.0f ), mTwoSided( 0.8f ), mPad0( 0 ), mPad1( 0 )
{
}

Material& Material::setColorAmbient( const ci::ColorAf& c )
{
	mColorAmbient = c;
	return *this;
}

Material& Material::setColorDiffuse( const ci::ColorAf& c )
{
	mColorDiffuse = c;
	return *this;
}

Material& Material::setColorEmission( const ci::ColorAf& c )
{
	mColorEmission = c;
	return *this;
}

Material& Material::setColorSpecular( const ci::ColorAf& c )
{
	mColorSpecular = c;
	return *this;
}

Material& Material::setShininess( float v )
{
	mShininess = v;
	return *this;
}

Material& Material::setTwoSided( float v )
{
	mTwoSided = v;
	return *this;
}

const ci::ColorAf& Material::getColorAmbient() const
{
	return mColorAmbient;
}

const ci::ColorAf& Material::getColorDiffuse() const
{
	return mColorDiffuse;
}

const ci::ColorAf& Material::getColorEmission() const
{
	return mColorEmission;
}

const ci::ColorAf& Material::getColorSpecular() const
{
	return mColorSpecular;
}

float Material::getShininess() const
{
	return mShininess;
}

float Material::getTwoSided() const
{
	return mTwoSided;
}
