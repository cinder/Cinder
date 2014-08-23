#version 130
#extension GL_EXT_gpu_shader4 : enable

// Pass in the render target metrics as a uniform
uniform vec4 SMAA_RT_METRICS; // (1/w, 1/h, w, h)

#define SMAA_PRESET_ULTRA
#define SMAA_INCLUDE_PS 0
#define SMAA_GLSL_3 1
#include "SMAA.h"

uniform mat4 ciModelViewProjection;

in vec4      ciPosition;
in vec4      ciTexCoord0;

out vec4     vTexCoord0;
out vec4     vOffset;

void main()
{
	float2 uv = ciTexCoord0.st;

	// Somehow calling "SMAANeighborhoodBlendingVS(uv, vOffset);" did not work :(
	vOffset = mad(SMAA_RT_METRICS.xyxy, float4( 1.0, 0.0, 0.0,  1.0), uv.xyxy);

	vTexCoord0 = ciTexCoord0;
	gl_Position = ciModelViewProjection * ciPosition;
}