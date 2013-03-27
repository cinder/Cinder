//  ---------------------------------------------------------------------------
//
//  @file       TwDirect3D11.hlsl
//  @author     Philippe Decaudin - http://www.antisphere.com
//	@brief		AntTweakBar shaders and techniques for Direct3D11 support
//  @license    This file is part of the AntTweakBar library.
//              For conditions of distribution and use, see License.txt
//
//  ---------------------------------------------------------------------------
 
float4 g_Offset : register(c0) = 0;
float4 g_CstColor : register(c1) = 1;

// Shaders for lines and rectangles

struct LineRectPSInput 
{ 
    float4 Pos : SV_POSITION; 
    float4 Color : COLOR0; 
};

LineRectPSInput LineRectVS(float4 pos : POSITION, float4 color : COLOR) 
{
    LineRectPSInput ps; 
    ps.Pos = pos + g_Offset;
    ps.Color = color; 
    return ps; 
}

LineRectPSInput LineRectCstColorVS(float4 pos : POSITION, float4 color : COLOR) 
{
    LineRectPSInput ps; 
    ps.Pos = pos + g_Offset;
    ps.Color = g_CstColor; 
    return ps; 
}

float4 LineRectPS(LineRectPSInput input) : SV_TARGET
{ 
    return input.Color; 
}

// Shaders for text

Texture2D g_Font : register(t0);

SamplerState g_FontSampler : register(s0)
{ 
    Filter = MIN_MAG_MIP_POINT; 
    AddressU = BORDER; 
    AddressV = BORDER; 
    BorderColor = float4(0, 0, 0, 0); 
};

struct TextPSInput 
{ 
    float4 Pos : SV_POSITION; 
    float4 Color : COLOR0; 
    float2 Tex : TEXCOORD0; 
};

TextPSInput TextVS(float4 pos : POSITION, float4 color : COLOR, float2 tex : TEXCOORD0)
{
    TextPSInput ps; 
    ps.Pos = pos + g_Offset;
    ps.Color = color; 
    ps.Tex = tex; 
    return ps; 
}

TextPSInput TextCstColorVS(float4 pos : POSITION, float4 color : COLOR, float2 tex : TEXCOORD0)
{
    TextPSInput ps; 
    ps.Pos = pos + g_Offset;
    ps.Color = g_CstColor; 
    ps.Tex = tex; 
    return ps; 
}
   
float4 TextPS(TextPSInput input) : SV_TARGET
{ 
    return g_Font.Sample(g_FontSampler, input.Tex) * input.Color; 
}
