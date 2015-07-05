#pragma once

#include "cinder/Matrix.h"

class Model
{
public:
	Model();

	Model&			modelMatrix( const ci::mat4& m );
	Model&			normalMatrix( const ci::mat3& m );

	const ci::mat4&	getModelMatrix() const;
	const ci::mat3&	getNormalMatrix() const;

	void			setModelMatrix( const ci::mat4& m );
	void			setNormalMatrix( const ci::mat3& m );
protected:
	ci::mat4		mModelMatrix;
	ci::mat3		mNormalMatrix;
};
 