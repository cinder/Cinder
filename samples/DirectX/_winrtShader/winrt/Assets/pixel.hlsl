// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//----------------------------------------------------------------------

Texture2D SimpleTexture : register( t0 );
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
    return SimpleTexture.Sample( SimpleSampler, input.tex ) * ( 0.8f * saturate( dot( input.norm, -lightDirection ) ) + 0.2f );
}


