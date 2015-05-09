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

cbuffer cbFixedFunctionParameters : register( b2 )
{
	float4 ffNormal;
	float4 ffUV;
	float4 ffColor;
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

float4 VS(VS_INPUT input) : SV_POSITION
{
	return float4(0, 0, 0, 1);
}

PS_INPUT Common(float4 pos, float3 normal, float2 uv, float4 color)
{
	PS_INPUT output;
	output.pos = mul(ModelView, pos);
	output.posView = output.pos.xyz;
	output.pos = mul(Projection, output.pos);
	output.normal = mul( ModelView, float4( normal, 0 ) ).xyz;
	output.uv = uv;
	output.color = color;
	return output;
}

PS_INPUT PositionVS(VS_INPUT input)
{
	return Common(input.pos, ffNormal.xyz, ffUV.xy, ffColor);
}

PS_INPUT PositionColorVS(VS_INPUT input)
{
	return Common(input.pos, ffNormal.xyz, ffUV.xy, input.color);
}

PS_INPUT PositionNormalVS(VS_INPUT input)
{
	return Common(input.pos, input.normal, ffUV.xy, ffColor);
}

PS_INPUT PositionTextureVS(VS_INPUT input)
{
	return Common(input.pos, ffNormal.xyz, input.uv, ffColor);
}

PS_INPUT PositionNormalColorVS(VS_INPUT input)
{
	return Common(input.pos, input.normal, ffUV.xy, input.color);
}

PS_INPUT PositionNormalTextureVS(VS_INPUT input)
{
	return Common(input.pos, input.normal, input.uv, ffColor);
}

PS_INPUT PositionColorTextureVS(VS_INPUT input)
{
	return Common(input.pos, ffNormal.xyz, input.uv, input.color);
}

PS_INPUT PositionNormalColorTextureVS(VS_INPUT input)
{
	return Common(input.pos, input.normal, input.uv, input.color);
}

float3 PointLight(Light light, float3 color, float3 normal, float3 vertex, float3 vertToEye)
{
	float3 vertToLight = normalize(light.pos.xyz - vertex);
	float3 lightRefl = reflect(-vertToLight, normal);
	float dist = distance(light.pos.xyz, vertex);
	float3 diffuse = light.diffuse.xyz * color * max(0, dot(normal, vertToLight));
	float3 specular = light.specular.xyz * pow(abs(max(0, dot(vertToEye, lightRefl))), light.specular.w * 128.0);
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
	if(dirDotVertToLight < cos(light.spotFalloff))
		spotAtt = 0;
	else
		spotAtt = pow(abs(max(0, dirDotVertToLight)), light.spotExponent);
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

PS_INPUT CommonLight(float4 pos, float3 normal, float2 uv, float4 color)
{
	PS_INPUT output = Common(pos, normal, uv, color);
	normal = normalize( output.normal );
	float3 vertToEye = normalize( -output.posView );

	float3 finalColor = float3(0, 0, 0);
	for(int i = 0; i < MAX_LIGHTS; ++i)
	{
		if(lights[i].type == POINT_LIGHT)
			finalColor += PointLight(lights[i], color.xyz, normal, output.posView, vertToEye);
		else if(lights[i].type == SPOT_LIGHT)
			finalColor += SpotLight(lights[i], color.xyz, normal, output.posView, vertToEye);
		else
			finalColor += DirLight(lights[i], color.xyz, normal, output.posView, vertToEye);
	}

	output.color = float4(finalColor, color.a);
	return output;
}

PS_INPUT PositionLightVS(VS_INPUT input)
{
	return CommonLight(input.pos, ffNormal.xyz, ffUV.xy, ffColor);
}

PS_INPUT PositionColorLightVS(VS_INPUT input)
{
	return CommonLight(input.pos, ffNormal.xyz, ffUV.xy, input.color);
}

PS_INPUT PositionNormalLightVS(VS_INPUT input)
{
	return CommonLight(input.pos, input.normal, ffUV.xy, ffColor);
}

PS_INPUT PositionTextureLightVS(VS_INPUT input)
{
	return CommonLight(input.pos, ffNormal.xyz, input.uv, ffColor);
}

PS_INPUT PositionNormalColorLightVS(VS_INPUT input)
{
	return CommonLight(input.pos, input.normal, ffUV.xy, input.color);
}

PS_INPUT PositionNormalTextureLightVS(VS_INPUT input)
{
	return CommonLight(input.pos, input.normal, input.uv, ffColor);
}

PS_INPUT PositionColorTextureLightVS(VS_INPUT input)
{
	return CommonLight(input.pos, ffNormal.xyz, input.uv, input.color);
}

PS_INPUT PositionNormalColorTextureLightVS(VS_INPUT input)
{
	return CommonLight(input.pos, input.normal, input.uv, input.color);
}
