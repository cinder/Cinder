#include "Material.h"

using namespace ci;

Material::Material()
: mColorAmbient( ColorAf::gray( 0.2f ) ), mColorDiffuse( ColorAf::gray( 0.8f ) ), 
mColorEmission( ColorAf::black() ), mColorSpecular( ColorAf::black() ), 
mShininess( 0.0f ), mPad0( 0 ), mPad1( 0 ), mPad2( 0 )
{
}

Material& Material::colorAmbient( const ci::ColorAf& c )
{
	mColorAmbient = c;
	return *this;
}

Material& Material::colorDiffuse( const ci::ColorAf& c )
{
	mColorDiffuse = c;
	return *this;
}

Material& Material::colorEmission( const ci::ColorAf& c )
{
	mColorEmission = c;
	return *this;
}

Material& Material::colorSpecular( const ci::ColorAf& c )
{
	mColorSpecular = c;
	return *this;
}

Material& Material::shininess( float v )
{
	mShininess = v;
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

void Material::setColorAmbient( const ci::ColorAf& c )
{
	mColorAmbient = c;
}

void Material::setColorDiffuse( const ci::ColorAf& c )
{
	mColorDiffuse = c;
}

void Material::setColorEmission( const ci::ColorAf& c )
{
	mColorEmission = c;
}

void Material::setColorSpecular( const ci::ColorAf& c )
{
	mColorSpecular = c;
}

void Material::setShininess( float v )
{
	mShininess = v;
}
