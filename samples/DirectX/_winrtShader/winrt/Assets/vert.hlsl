// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//----------------------------------------------------------------------

//for constant buffers register b0 will always be reserved for the projection and model-view matrices
cbuffer cbMatrices : register( b0 )
{
	matrix Projection;
	matrix ModelView;
};

//all other constant buffers are user-defined
//cbuffer cbColor : register(b1)
//{
//	float4 userColor;
//};

//all vertex shader input must be laid out like this
struct sVSInput
{
	float4 pos : POSITION;
	float3 norm : NORMAL;
	float2 tex : TEXCOORD;
	float4 color : COLOR;
};

//this can be laid however
struct sPSInput
{
    float4 pos : SV_POSITION;
    float3 norm : NORMAL;
    float2 tex : TEXCOORD0;
};

sPSInput SimpleVertexShader( sVSInput input )
{
    sPSInput output;
    float4 temp = input.pos;
    temp = mul( ModelView, temp );
    temp = mul( Projection, temp );
    output.pos = temp;
    output.tex = input.tex;
    output.norm = mul( ModelView, float4( input.norm, 0.0f ) ).xyz;
    return output;
}

