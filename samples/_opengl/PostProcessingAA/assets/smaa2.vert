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
noperspective out vec2     pixcoord;
noperspective out vec4     offset[3];

void main()
{
	texcoord = ciTexCoord0.st;

	// Somehow calling "SMAABlendingWeightCalculationVS(texcoord, pixcoord, offset);" did not work :(
	pixcoord = texcoord * SMAA_RT_METRICS.zw;

	// We will use these offsets for the searches later on (see @PSEUDO_GATHER4):
	offset[0] = mad(SMAA_RT_METRICS.xyxy, float4(-0.25, -0.125,  1.25, -0.125), texcoord.xyxy);
	offset[1] = mad(SMAA_RT_METRICS.xyxy, float4(-0.125, -0.25, -0.125,  1.25), texcoord.xyxy);

	// And these for the searches, they indicate the ends of the loops:
	offset[2] = mad(SMAA_RT_METRICS.xxyy,
					float4(-2.0, 2.0, -2.0, 2.0) * float(SMAA_MAX_SEARCH_STEPS),
					float4(offset[0].xz, offset[1].yw));

	gl_Position = ciModelViewProjection * ciPosition;
}