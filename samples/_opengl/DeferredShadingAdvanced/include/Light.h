#pragma once

#include "cinder/Camera.h"
#include "cinder/Color.h"
#include "cinder/Vector.h"

class Light 
{
public:
	Light();
	
	Light&				colorAmbient( const ci::ColorAf& c );
	Light&				colorDiffuse( const ci::ColorAf& c );
	Light&				colorSpecular( const ci::ColorAf& c );
	Light&				intensity( float v );
	Light&				radius( float v );
	Light&				volume( float v );
	Light&				position( const ci::vec3& v );

	const ci::ColorAf&	getColorAmbient() const;
	const ci::ColorAf&	getColorDiffuse() const;
	const ci::ColorAf&	getColorSpecular() const;
	float				getIntensity() const;
	const ci::vec3&		getPosition() const;
	float				getRadius() const;
	float				getVolume() const;

	void				setColorAmbient( const ci::ColorAf& c );
	void				setColorDiffuse( const ci::ColorAf& c );
	void				setColorSpecular( const ci::ColorAf& c );
	void				setIntensity( float v );
	void				setRadius( float v );
	void				setVolume( float v );
	void				setPosition( const ci::vec3& v );
protected:
	ci::ColorAf			mColorAmbient;
	ci::ColorAf			mColorDiffuse;
	ci::ColorAf			mColorSpecular;
	ci::vec3			mPosition;
	float				mIntensity;
	float				mRadius;
	float				mVolume;
	uint32_t			mPad0;
	uint32_t			mPad1;
};
