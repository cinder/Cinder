#pragma once

#include "cinder/Color.h"

class Material
{
public:
	Material();
	
	Material&			colorAmbient( const ci::ColorAf& c );
	Material&			colorDiffuse( const ci::ColorAf& c );
	Material&			colorEmission( const ci::ColorAf& c );
	Material&			colorSpecular( const ci::ColorAf& c );
	Material&			shininess( float v );

	const ci::ColorAf&	getColorAmbient() const;
	const ci::ColorAf&	getColorDiffuse() const;
	const ci::ColorAf&	getColorEmission() const;
	const ci::ColorAf&	getColorSpecular() const;
	float				getShininess() const;
	
	void				setColorAmbient( const ci::ColorAf& c );
	void				setColorDiffuse( const ci::ColorAf& c );
	void				setColorEmission( const ci::ColorAf& c );
	void				setColorSpecular( const ci::ColorAf& c );
	void				setShininess( float v );
protected:
	ci::ColorAf			mColorAmbient;
	ci::ColorAf			mColorDiffuse;
	ci::ColorAf			mColorEmission;
	ci::ColorAf			mColorSpecular;
	float				mShininess;
	uint32_t			mPad0;
	uint32_t			mPad1;
	uint32_t			mPad2;
};
