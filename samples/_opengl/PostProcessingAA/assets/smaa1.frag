#version 150

// Pass in the render target metrics as a uniform
uniform vec4 SMAA_RT_METRICS; // (1/w, 1/h, w, h)

#define SMAA_PRESET_ULTRA
#define SMAA_INCLUDE_VS 0
#define SMAA_GLSL_3 1
#include "SMAA.h"

noperspective in vec2    texcoord;
noperspective in vec4    offset[3];

uniform sampler2D uColorTex;
out vec4 fragColor;

void main()
{
    fragColor = vec4( SMAALumaEdgeDetectionPS(texcoord, offset, uColorTex), 0.0, 0.0);
}