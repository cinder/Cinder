cbuffer cbChangeOnResize : register( b0 )
{
    matrix Projection;
	matrix ModelView;
};

Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

struct VS_INPUT
{
	float4 pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
	float4 color : COLOR;
};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD0;
	float4 color : COLOR;
	float3 posView : TEXCOORD1;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output;
	output.pos = mul( ModelView, input.pos );
	output.pos = mul( Projection, output.pos );
	output.uv = input.uv;
	output.color = input.color;

	//so the compiler doesn't whine about uninitialized fields
	output.normal = float3(0, 0, 0);
	output.posView = float3(0, 0, 0);

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input ) : SV_Target
{
	return txDiffuse.Sample( samLinear, input.uv ) * input.color;
}
