// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//----------------------------------------------------------------------

cbuffer cbColor : register(b1)
{
	float4 userColor;
};

//when you call bind() on a texture from the application it takes a texture unit which
//corresponds to the register value, so if you do bind(1) then register should have t1
Texture2D SimpleTexture : register( t0 );

//the SamplerState will be passed in based on the sampling parameters specified in the
//application, and the register's index must be the same as the texture it's applied to
SamplerState SimpleSampler : register( s0 );

struct sPSInput
{
    float4 pos : SV_POSITION;
    float3 norm : NORMAL;
    float2 tex : TEXCOORD0;
};

float4 SimplePixelShader( sPSInput input ) : SV_TARGET
{
    float3 lightDirection = normalize( float3(1,-3,2) );
    return SimpleTexture.Sample( SimpleSampler, input.tex ) * userColor;// * ( 0.8f * saturate( dot( input.norm, -lightDirection ) ) + 0.2f );
}


