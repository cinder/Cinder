cbuffer cbChangeOnResize : register( b0 )
{
    matrix Projection;
	matrix ModelView;
};

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
	output.color = input.color;

	//so the compiler quits whining about uninitialized members
	output.normal = float3(0, 0, 0);
	output.uv = float2(0, 0);
	output.posView = float3(0, 0, 0);

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input ) : SV_Target
{
	return input.color;
}
