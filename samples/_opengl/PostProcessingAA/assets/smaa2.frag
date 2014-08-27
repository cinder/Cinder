#version 150

// Pass in the render target metrics as a uniform
uniform vec4 SMAA_RT_METRICS; // (1/w, 1/h, w, h)

#define SMAA_PRESET_ULTRA
#define SMAA_INCLUDE_VS 0
#define SMAA_GLSL_3 1
#include "SMAA.h"

// Additional shader inputs
uniform sampler2D uEdgesTex;
uniform sampler2D uAreaTex;
uniform sampler2D uSearchTex;

noperspective in vec2    texcoord;
noperspective in vec2    pixcoord;
noperspective in vec4    offset[3];

out vec4 fragColor;

void main()
{
	ivec4 subsampleIndices = ivec4(0, 0, 0, 0);

	fragColor = SMAABlendingWeightCalculationPS(texcoord, pixcoord, offset, 
	   uEdgesTex, uAreaTex, uSearchTex, subsampleIndices);
}