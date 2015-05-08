uniform int uMaterialId;

in Vertex
{
	vec3 color;
	vec3 normal;
} vertex;

layout (location = 0) out vec4	oAlbedo;
layout (location = 1) out ivec4	oMaterial;
layout (location = 2) out vec4	oNormal;

vec2 pack( vec3 v )
{
    vec2 n	= normalize( v.xy ) * ( sqrt( -v.z * 0.5 + 0.5 ) );
    n		= n * 0.5 + 0.5;
    return n;
}

void main( void )
{
	oAlbedo 	= vec4( vertex.color, 1.0 );
	oMaterial	= ivec4( uMaterialId, 0, 0, 255 );
	oNormal		= vec4( pack( normalize( vertex.normal ) ), 0.0, 1.0 );
}
 