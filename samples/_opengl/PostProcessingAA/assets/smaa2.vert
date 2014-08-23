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
out vec2     vPixCoord;
out vec4     vOffset[3];

void main()
{
	float2 uv = ciTexCoord0.st;

	// Somehow calling "SMAABlendingWeightCalculationVS(uv, vPixCoord, vOffset);" did not work :(
	vPixCoord = uv * SMAA_RT_METRICS.zw;

	// We will use these offsets for the searches later on (see @PSEUDO_GATHER4):
	vOffset[0] = mad(SMAA_RT_METRICS.xyxy, float4(-0.25, -0.125,  1.25, -0.125), uv.xyxy);
	vOffset[1] = mad(SMAA_RT_METRICS.xyxy, float4(-0.125, -0.25, -0.125,  1.25), uv.xyxy);

	// And these for the searches, they indicate the ends of the loops:
	vOffset[2] = mad(SMAA_RT_METRICS.xxyy,
					float4(-2.0, 2.0, -2.0, 2.0) * float(SMAA_MAX_SEARCH_STEPS),
					float4(vOffset[0].xz, vOffset[1].yw));

	vTexCoord0 = ciTexCoord0;
	gl_Position = ciModelViewProjection * ciPosition;
}