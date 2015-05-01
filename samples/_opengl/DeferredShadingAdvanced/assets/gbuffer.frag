uniform int uMaterialId;

in Vertex
{
	vec3	color;
	vec3 	normal;
	vec2 	uv;
} vertex;

layout (location = 0) out vec3	oAlbedo;
layout (location = 1) out int	oMaterial;
layout (location = 2) out vec2	oNormal;

vec2 encodeNormal( vec3 n )
{
    vec2 enc	= normalize( n.xy ) * ( sqrt( -n.z * 0.5 + 0.5 ) );
    enc			= enc * 0.5 + 0.5;
    return enc;
}

void main( void )
{
	oAlbedo 	= vertex.color;
	oMaterial	= uMaterialId;
	oNormal		= encodeNormal( vertex.normal );
}