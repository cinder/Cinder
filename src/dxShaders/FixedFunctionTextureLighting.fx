#define MAX_LIGHTS 8

#define POINT_LIGHT 0
#define DIR_LIGHT   1
#define SPOT_LIGHT  2

cbuffer cbChangeOnResize : register( b0 )
{
    matrix Projection;
	matrix ModelView;
};

struct Light
{
	float4 diffuse;
	float4 specular;
	float4 ambient;
	float constantAtt;
	float linearAtt;
	float quadAtt;
	int type;
	float4 pos;
	float4 direction;
	float spotExponent;
	float spotFalloff;
	float2 padding;
};

cbuffer cbLights : register( b1 )
{
	Light lights[MAX_LIGHTS];
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

float3 PointLight(Light light, float3 color, float3 normal, float3 vertex, float3 vertToEye)
{
	float3 vertToLight = normalize(light.pos.xyz - vertex);
	float3 lightRefl = reflect(-vertToLight, normal);
	float dist = distance(light.pos.xyz, vertex);
	float3 diffuse = light.diffuse.xyz * color * max(0, dot(normal, vertToLight));
	float3 specular = light.specular.xyz * pow(abs(max(0, dot(vertToEye, lightRefl))), light.specular.w);
	float attenuation = min(1.0, 1.0 / (light.constantAtt + light.linearAtt * dist + light.quadAtt * dist * dist));
	return (diffuse + specular) * attenuation + light.ambient.xyz;
}

float3 SpotLight(Light light, float3 color, float3 normal, float3 vertex, float3 vertToEye)
{
	float3 vertToLight = normalize(light.pos.xyz - vertex);
	float3 lightRefl = reflect(-vertToLight, normal);
	float dist = distance(light.pos.xyz, vertex);
	float dirDotVertToLight = dot(normalize(light.direction.xyz), -vertToLight);
	float3 diffuse = light.diffuse.xyz * color * max(0, dot(normal, vertToLight));
	float3 specular = light.specular.xyz * pow(abs(max(0, dot(vertToEye, lightRefl))), light.specular.w);
	float attenuation = min(1.0, 1.0 / (light.constantAtt + light.linearAtt * dist + light.quadAtt * dist * dist));
	float spotAtt;
	if(dirDotVertToLight > cos(light.spotFalloff))
		spotAtt = 0;
	else
		spotAtt = max(1, pow(abs(max(0, dirDotVertToLight)), light.spotExponent));
	return (diffuse + specular) * attenuation * spotAtt + light.ambient.xyz;
}

float3 DirLight(Light light, float3 color, float3 normal, float3 vertex, float3 vertToEye)
{
	float3 vertToLight = normalize(-light.direction.xyz);
	float3 lightRefl = reflect(-vertToLight, normal);
	float3 diffuse = light.diffuse.xyz * color * max(0, dot(normal, vertToLight));
	float3 specular = light.specular.xyz * pow(abs(max(0, dot(vertToEye, lightRefl))), light.specular.w);
	return diffuse + specular + light.ambient.xyz;
}

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output;
	output.pos = mul( ModelView, input.pos );
	output.pos = mul( Projection, output.pos );
	output.normal = mul( ModelView, float4( input.normal, 0 ) ).xyz;
	output.uv = input.uv;
	output.color = input.color;
	output.posView = mul( ModelView, input.pos ).xyz;

	float3 normal = normalize( output.normal );
	float3 vertToEye = normalize( -output.posView );

	float3 finalColor = float3(0, 0, 0);
	for(int i = 0; i < MAX_LIGHTS; ++i)
	{
		if(lights[i].type == POINT_LIGHT)
			finalColor += PointLight(lights[i], input.color.xyz, normal, output.posView, vertToEye);
		else if(lights[i].type == SPOT_LIGHT)
			finalColor += SpotLight(lights[i], input.color.xyz, normal, output.posView, vertToEye);
		else
			finalColor += DirLight(lights[i], input.color.xyz, normal, output.posView, vertToEye);
	}

	output.color = float4(finalColor * input.color.rgb, input.color.a);

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input ) : SV_Target
{
	return input.color * txDiffuse.Sample(samLinear, input.uv);
}