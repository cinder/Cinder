uniform vec2		uProjectionParams;
uniform mat4		uProjMatrixInverse;
uniform sampler2D	uSamplerDepth;

vec3 unpackNormal( in vec2 uv )
{
	vec2 fenc   = uv * 4.0 - 2.0;
	float f     = dot( fenc, fenc );
	float g     = sqrt( 1.0 - f / 4.0 );

	return vec3( fenc * g, 1.0 - f / 2.0 );
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
 