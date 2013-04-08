struct VS_INPUT
{
	float4 pos : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
	float4 color : COLOR;
};

float4 VS(VS_INPUT input) : SV_POSITION
{
	return float4(0, 0, 0, 1);
}
