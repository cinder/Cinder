#version 130
#extension GL_EXT_gpu_shader4 : enable

// Pass in the render target metrics as a uniform
uniform vec4 SMAA_RT_METRICS; // (1/w, 1/h, w, h)

#define SMAA_PRESET_ULTRA
#define SMAA_INCLUDE_VS 0
#define SMAA_GLSL_3 1
#include "SMAA.h"

in vec4    vTexCoord0;
in vec4    vOffset[3];

uniform sampler2D uColorTex;

void main()
{
    gl_FragColor.rg = SMAALumaEdgeDetectionPS(vTexCoord0.st, vOffset, uColorTex);
}