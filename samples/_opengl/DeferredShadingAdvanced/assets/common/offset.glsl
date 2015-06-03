uniform vec2 uOffset;
uniform vec2 uWindowSize;

vec2 calcTexCoordFromFrag( vec2 fragCoord )
{
	return ( fragCoord + uOffset ) / uWindowSize;
}

vec2 calcTexCoordFromUv( vec2 uv )
{
	vec2 offset	= uOffset / uWindowSize;
	uv			*= vec2( 1.0 ) - offset * 2.0;
	uv			+= offset;
	return uv;
}
 