#version 150

#ifdef GL_ARB_gpu_shader5
#extension GL_ARB_gpu_shader5 : enable
#else
#extension GL_EXT_gpu_shader4 : enable
#define FXAA_FAST_PIXEL_OFFSET 0
#endif

#define FXAA_PC 1
#define FXAA_GLSL_130 1
#define FXAA_QUALITY__PRESET 39
#define FXAA_GREEN_AS_LUMA 0

#include "FXAA3_11.h"

uniform sampler2D	uTexture;
uniform vec4		uExtents;

out     vec4        oColor;

void main( void )
{
	FxaaFloat2 fxaaQualityRcpFrame;
	fxaaQualityRcpFrame.x = uExtents.x;
	fxaaQualityRcpFrame.y = uExtents.y;

	FxaaFloat2 uv = gl_FragCoord.xy * uExtents.xy;

	FxaaFloat4 ConsolePosPos = FxaaFloat4(0.0,0.0,0.0,0.0);
	FxaaFloat4 ConsoleRcpFrameOpt = FxaaFloat4(0.0,0.0,0.0,0.0);
	FxaaFloat4 ConsoleRcpFrameOpt2 = FxaaFloat4(0.0,0.0,0.0,0.0);
	FxaaFloat4 Console360RcpFrameOpt2 = FxaaFloat4(0.0,0.0,0.0,0.0);

	// Only used on FXAA Quality.
	// Choose the amount of sub-pixel aliasing removal.
	// This can effect sharpness.
	//   1.00 - upper limit (softer)
	//   0.75 - default amount of filtering
	//   0.50 - lower limit (sharper, less sub-pixel aliasing removal)
	//   0.25 - almost off
	//   0.00 - completely off
	FxaaFloat QualitySubpix = 0.75;

	// The minimum amount of local contrast required to apply algorithm.
	//   0.333 - too little (faster)
	//   0.250 - low quality
	//   0.166 - default
	//   0.125 - high quality
	//   0.033 - very high quality (slower)
	FxaaFloat QualityEdgeThreshold = 0.033;

	// You dont need to touch theses variables it have no visible effect
	FxaaFloat QualityEdgeThresholdMin = 0.0;
	FxaaFloat ConsoleEdgeSharpness = 8.0;
	FxaaFloat ConsoleEdgeThreshold = 0.125;
	FxaaFloat ConsoleEdgeThresholdMin = 0.05;
	FxaaFloat4 Console360ConstDir = FxaaFloat4(1.0, -1.0, 0.25, -0.25);

	oColor = FxaaPixelShader(uv, ConsolePosPos, uTexture, uTexture, uTexture, fxaaQualityRcpFrame,
		ConsoleRcpFrameOpt, ConsoleRcpFrameOpt2, Console360RcpFrameOpt2, 
		QualitySubpix, QualityEdgeThreshold, QualityEdgeThresholdMin, 
		ConsoleEdgeSharpness, ConsoleEdgeThreshold, ConsoleEdgeThresholdMin, Console360ConstDir);
}
