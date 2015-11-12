#version 150

// Pass in the render target metrics as a uniform
uniform vec4 SMAA_RT_METRICS; // (1/w, 1/h, w, h)

#define SMAA_INCLUDE_VS 1
#define SMAA_INCLUDE_PS 0
#include "SMAA.h"

uniform mat4 ciModelViewProjection;

in vec4      ciPosition;
in vec4      ciTexCoord0;

noperspective out vec2     texcoord;
noperspective out vec2     pixcoord;
noperspective out vec4     offset[3];

void main()
{
	texcoord = pixcoord = ciTexCoord0.st;

#if SMAA_PASS == 1
	SMAAEdgeDetectionVS( texcoord, offset );
#elif SMAA_PASS == 2
	SMAABlendingWeightCalculationVS( texcoord, pixcoord, offset );
#elif SMAA_PASS == 3
	SMAANeighborhoodBlendingVS( texcoord, offset[0] );
#endif

	gl_Position = ciModelViewProjection * ciPosition;
}