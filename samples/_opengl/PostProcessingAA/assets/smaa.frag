#version 150

// Pass in the render target metrics as a uniform
uniform vec4 SMAA_RT_METRICS; // (1/w, 1/h, w, h)

#define SMAA_INCLUDE_VS 0
#define SMAA_INCLUDE_PS 1
#include "SMAA.h"

noperspective in vec2    texcoord;
noperspective in vec2    pixcoord;
noperspective in vec4    offset[3];

// Additional shader inputs
uniform sampler2D uColorTex;   // 1st and 3rd pass
uniform sampler2D uEdgesTex;   // 2nd pass only
uniform sampler2D uAreaTex;    // 2nd pass only
uniform sampler2D uSearchTex;  // 2nd pass only
uniform sampler2D uBlendTex;   // 3rd pass only

out vec4 fragColor;

void main()
{
#if SMAA_PASS == 1
    fragColor = vec4( SMAALumaEdgeDetectionPS( texcoord, offset, uColorTex ), 0.0, 0.0 );
#elif SMAA_PASS == 2
	ivec4 subsampleIndices = ivec4(0, 0, 0, 0);
	fragColor = SMAABlendingWeightCalculationPS( texcoord, pixcoord, offset, 
	   uEdgesTex, uAreaTex, uSearchTex, subsampleIndices );
#elif SMAA_PASS == 3
	fragColor = SMAANeighborhoodBlendingPS( texcoord, offset[0], uColorTex, uBlendTex );
#endif
}