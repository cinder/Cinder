#version 330 core

uniform int uMaterialId;

in Vertex
{
	vec3	color;
	vec3 	normal;
	vec2 	uv;
} vertex;

out vec4	oAlbedo;
out ivec4	oMaterial;
out vec4	oNormal;

vec2 encodeNormal( vec3 n )
{
    vec2 enc	= normalize( n.xy ) * ( sqrt( -n.z * 0.5 + 0.5 ) );
    enc			= enc * 0.5 + 0.5;
    return enc;
}

void main( void )
{
	oAlbedo 	= vec4( vertex.color, 1.0 );
	oMaterial	= ivec4( uMaterialId, 0, 0, 1 );
	oNormal		= vec4( encodeNormal( vertex.normal ), 0.0, 1.0 );
}