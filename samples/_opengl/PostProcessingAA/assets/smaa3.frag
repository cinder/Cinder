#version 150

// Pass in the render target metrics as a uniform
uniform vec4 SMAA_RT_METRICS; // (1/w, 1/h, w, h)

#define SMAA_PRESET_ULTRA
#define SMAA_INCLUDE_VS 0
#define SMAA_GLSL_3 1
#include "SMAA.h"

uniform sampler2D uColorTex;
uniform sampler2D uBlendTex;

noperspective in vec2    texcoord;
noperspective in vec4    offset;

out vec4   fragColor;

void main()
{	
    fragColor = SMAANeighborhoodBlendingPS(texcoord, offset, uColorTex, uBlendTex);
}