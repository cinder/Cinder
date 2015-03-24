#version 330 core

uniform int			uMaterialId;
uniform sampler2D	uSampler;
uniform float		uSamplerMix;

in Vertex
{
	vec3	color;
	vec3 	normal;
	vec2 	uv;
} vertex;

out vec3	oAlbedo;
out int		oMaterial;
out vec2	oNormal;

vec2 encodeNormal( vec3 n )
{
    vec2 enc	= normalize( n.xy ) * ( sqrt( -n.z * 0.5 + 0.5 ) );
    enc			= enc * 0.5 + 0.5;
    return enc;
}

void main( void )
{
	oAlbedo 	= vertex.color - uSamplerMix + texture( uSampler, vertex.uv ).rgb * vertex.color * uSamplerMix;
	oMaterial	= uMaterialId;
	oNormal		= encodeNormal( vertex.normal );
}