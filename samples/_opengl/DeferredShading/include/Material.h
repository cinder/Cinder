#pragma once

#include "cinder/Color.h"

class Material
{
public:
	Material();
	
	Material&			setColorAmbient( const ci::ColorAf& c );
	Material&			setColorDiffuse( const ci::ColorAf& c );
	Material&			setColorEmission( const ci::ColorAf& c );
	Material&			setColorSpecular( const ci::ColorAf& c );
	Material&			setShininess( float v );
	Material&			setTwoSided( float v );

	const ci::ColorAf&	getColorAmbient() const;
	const ci::ColorAf&	getColorDiffuse() const;
	const ci::ColorAf&	getColorEmission() const;
	const ci::ColorAf&	getColorSpecular() const;
	float				getShininess() const;
	float				getTwoSided() const;
protected:
	ci::ColorAf			mColorAmbient;
	ci::ColorAf			mColorDiffuse;
	ci::ColorAf			mColorEmission;
	ci::ColorAf			mColorSpecular;
	float				mShininess;
	float				mTwoSided;
	uint32_t			mPad0;
	uint32_t			mPad1;
};
