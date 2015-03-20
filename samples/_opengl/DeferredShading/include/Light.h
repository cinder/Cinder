#pragma once

#include "cinder/Camera.h"
#include "cinder/Color.h"
#include "cinder/Vector.h"

class Light 
{
public:
	Light();
	
	const ci::ColorAf&	getColorAmbient() const;
	const ci::ColorAf&	getColorDiffuse() const;
	const ci::ColorAf&	getColorSpecular() const;
	float				getIntensity() const;
	const ci::vec3&		getPosition() const;
	float				getRadius() const;
	float				getVolume() const;

	Light&				setColorAmbient( const ci::ColorAf& c );
	Light&				setColorDiffuse( const ci::ColorAf& c );
	Light&				setColorSpecular( const ci::ColorAf& c );
	Light&				setIntensity( float v );
	Light&				setRadius( float v );
	Light&				setVolume( float v );
	Light&				setPosition( const ci::vec3& v );
protected:
	ci::ColorAf			mColorAmbient;
	ci::ColorAf			mColorDiffuse;
	ci::ColorAf			mColorSpecular;
	float				mIntensity;
	float				mRadius;
	float				mVolume;
	ci::vec3			mPosition;
};
