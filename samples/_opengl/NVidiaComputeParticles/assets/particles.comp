//----------------------------------------------------------------------------------
// File:        ComputeParticles/assets/shaders/particlesCS.glsl
// Url:         https://github.com/NVIDIAGameWorks/OpenGLSamples/tree/master/samples/es3aep-kepler/ComputeParticles
// SDK Version: v1.2 
// Email:       gameworks@nvidia.com
// Site:        http://developer.nvidia.com/
//
// Copyright (c) 2014, NVIDIA CORPORATION. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------------
#version 430
#extension GL_ARB_compute_shader : enable
#extension GL_ARB_shader_storage_buffer_object : enable

//#UNIFORMS
#define WORK_GROUP_SIZE 128

layout( std140, binding = 0 ) uniform ParticleParams
{
    uniform vec4	attractor;
    uniform float	numParticles;
    uniform float	damping;
    uniform float	noiseFreq;
    uniform float	noiseStrength;
    uniform float	invNoiseSize;
};

uniform sampler3D	noiseTex3D;

layout( std140, binding = 1 ) buffer Pos {
	vec4 pos[];
};

layout( std140, binding = 2 ) buffer Vel {
	vec4 vel[];
};

layout( local_size_x = WORK_GROUP_SIZE,  local_size_y = 1, local_size_z = 1 ) in;

// noise functions
// returns random value in [-1, 1]
vec3 noise3f( vec3 p )
{
	return texture( noiseTex3D, p * invNoiseSize ).xyz;
}

// Fractal sum (Fractal Brownian Motion)
vec3 fBm3f( vec3 p, int octaves, float lacunarity, float gain )
{
	float freq = 1.0;
	float amp = 0.5;
	vec3 sum = vec3( 0.0 );
	for( int i=0; i < octaves; i++ ) {
		sum += noise3f( p * freq ) * amp;
		freq *= lacunarity;
		amp *= gain;
	}
	return sum;
}

vec3 attract( vec3 p, vec3 p2 )
{
	const float softeningSquared = 0.01;
	vec3 v = p2 - p;
	float r2 = dot( v, v );
	r2 += softeningSquared;
	float invDist = 1.0f / sqrt( r2 );
	float invDistCubed = invDist * invDist * invDist;
	return v * invDistCubed;
}

// Compute shader to update particles
void main()
{
	uint i = gl_GlobalInvocationID.x;

	// Thread block size may not be exact multiple of number of particles.
	if( i >= numParticles )
	{
		return;
	}

	// Read particle position and velocity from buffers.
	vec3 p = pos[i].xyz;
	vec3 v = vel[i].xyz;

	v += fBm3f( p * noiseFreq, 4, 2.0, 0.5 ) * noiseStrength;
	v += attract( p, attractor.xyz ) * attractor.w;

	// Integrate
	p += v;
	v *= damping;

	// Write new values
	pos[i] = vec4( p, 1.0 );
	vel[i] = vec4( v, 0.0 );
}
