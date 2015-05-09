cbuffer cbChangeOnResize : register( b0 )
{
    matrix Projection;
	matrix ModelView;
};

cbuffer cbColor : register(b1)
{
	float4 someColor;
};

Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

struct VS_INPUT
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output;
	output.pos = mul( ModelView, float4( input.pos, 1 ) );
	output.pos = mul( Projection, output.pos );

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input ) : SV_Target
{
	return someColor;
}
