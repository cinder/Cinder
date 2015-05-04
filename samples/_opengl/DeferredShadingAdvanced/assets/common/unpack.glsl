uniform vec2		uProjectionParams;
uniform mat4		uProjMatrixInverse;
uniform sampler2D	uSamplerDepth;

vec3 unpackNormal( in vec2 uv )
{
	vec4 n				= vec4( uv.xy, 0.0, 0.0 ) * vec4( 2.0, 2.0, 0.0, 0.0 ) + vec4( -1.0, -1.0, 1.0, -1.0 );
	float l				= dot( n.xyz, -n.xyw );
	n.z					= l;
	n.xy				*= sqrt( l );
	return n.xyz * 2.0 + vec3( 0.0, 0.0, -1.0 );
}
 
vec4 unpackPosition( in vec2 uv )
{
	float depth			= texture( uSamplerDepth, uv ).x;
	float linearDepth 	= uProjectionParams.y / ( depth - uProjectionParams.x );
	vec4 posProj		= vec4( ( uv.x - 0.5 ) * 2.0, ( uv.y - 0.5 ) * 2.0, 0.0, 1.0 );
	vec4 viewRay		= uProjMatrixInverse * posProj;
	return vec4( viewRay.xyz * linearDepth, 1.0 );
}

vec4 unpackPosition( in vec2 uv, inout float depth )
{
	depth				= texture( uSamplerDepth, uv ).x;
	float linearDepth 	= uProjectionParams.y / ( depth - uProjectionParams.x );
	vec4 posProj		= vec4( ( uv.x - 0.5 ) * 2.0, ( uv.y - 0.5 ) * 2.0, 0.0, 1.0 );
	vec4 viewRay		= uProjMatrixInverse * posProj;
	return vec4( viewRay.xyz * linearDepth, 1.0 );
}
 