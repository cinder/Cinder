#pragma once

#include "cinder/audio/Buffer.h"
#include "cinder/CinderAssert.h"
#include "cinder/Rand.h"

#define ACCEPTABLE_FLOAT_ERROR 0.000001f 

inline void fillRandom( ci::audio::Buffer *buffer )
{
	for( size_t i = 0; i < buffer->getSize(); i++ )
		(*buffer)[i] = ci::randFloat( -1.0f, 1.0f );
}

inline float maxError( const ci::audio::Buffer &a, const ci::audio::Buffer &b )
{
	CI_ASSERT( a.getSize() == b.getSize() );

	float error = 0.0f;
	for( size_t i = 0; i < a.getSize(); i++ )
		error = std::max( error, std::fabs( a[i] - b[i]) );

	return error;
}