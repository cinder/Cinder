#pragma once

#include "cinder/audio2/Buffer.h"
#include "cinder/audio2/CinderAssert.h"
#include "cinder/Rand.h"

#define ACCEPTABLE_FLOAT_ERROR 0.000001f 

void fillRandom( ci::audio2::Buffer *buffer )
{
	for( size_t i = 0; i < buffer->getSize(); i++ )
		(*buffer)[i] = ci::randFloat( -1.0f, 1.0f );
}

float maxError( const ci::audio2::Buffer &a, const ci::audio2::Buffer &b )
{
	CI_ASSERT( a.getSize() == b.getSize() );

	float error = 0.0f;
	for( size_t i = 0; i < a.getSize(); i++ )
		error = std::max( error, std::fabs( a[i] - b[i]) );

	return error;
}