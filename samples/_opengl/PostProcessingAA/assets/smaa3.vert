#version 150

// Pass in the render target metrics as a uniform
uniform vec4 SMAA_RT_METRICS; // (1/w, 1/h, w, h)

#define SMAA_PRESET_ULTRA
#define SMAA_INCLUDE_PS 0
#define SMAA_GLSL_3 1
#include "SMAA.h"

uniform mat4 ciModelViewProjection;

in vec4      ciPosition;
in vec4      ciTexCoord0;

noperspective out vec2     texcoord;
noperspective out vec4     offset;

void main()
{
	texcoord = ciTexCoord0.st;

	// Somehow calling "SMAANeighborhoodBlendingVS(texcoord, offset);" did not work :(
	offset = mad(SMAA_RT_METRICS.xyxy, float4( 1.0, 0.0, 0.0,  1.0), texcoord.xyxy);

	gl_Position = ciModelViewProjection * ciPosition;
}