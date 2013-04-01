struct rgbaVS_INPUT
{
	float4 pos : POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

struct rgbVS_INPUT
{
	float4 pos : POSITION;
	float3 normal : NORMAL;
	float3 color : COLOR;
	float2 uv : TEXCOORD;
};

float4 rgbaVS(rgbaVS_INPUT input) : SV_POSITION
{
	return float4(0, 0, 0, 1);
}

float4 rgbVS(rgbVS_INPUT input) : SV_POSITION
{
	return float4(0, 0, 0, 1);
}