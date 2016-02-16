#version 330 
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(std140, binding = 0) uniform ciBlock0T {
	uniform mat4		ciModelViewProjection;
	uniform mat4		ciModelView;
	uniform mat4		ciViewMatrix;
	uniform mat4		ciViewMatrixInverse;
	uniform mat3		ciNormalMatrix;
	uniform mat4		ciModelMatrix;
	uniform mat4		ciModelMatrixInverse;

	uniform mat4		uShadowMvp;
	uniform vec4		uLightPos;
	uniform float		uTime;
	uniform float		uFogNearDist;
	uniform float		uFogFarDist;
	uniform float		uFogPower;

	uniform int			uFboRes;
} ciBlock0;

layout(binding = 1) uniform sampler2DRect	uPrevPosTex;
layout(binding = 2) uniform sampler2DRect	uCurrPosTex;
layout(binding = 3) uniform sampler2DRect 	uCurrVelTex;

layout(location = 0) in vec4	ciPosition;
layout(location = 1) in vec2	ciTexCoord0;
layout(location = 2) in vec3	ciNormal;
//layout(location = 3) in vec3	vInstancePosition; // per-layout(location = 0) instance position variable

layout(location =  0) out vec2	PosCoords;
layout(location =  1) out vec4	Position;
layout(location =  2) out vec4	WorldPos;
layout(location =  3) out vec4	ViewPos;
layout(location =  4) out vec2	TexCoord;
layout(location =  5) out vec3	ViewNormal;
layout(location =  6) out vec3	Normal;
layout(location =  7) out vec3	LightPos;  
layout(location =  8) out vec3	LightDir;
layout(location =  9) out float	FogPer;
layout(location = 10) out vec4	ShadowCoord;
layout(location = 11) out float	vCrowd;

float getSinVal( float z, float randPer )
{
	float t			= ( ciBlock0.uTime * 0.75 + randPer * 26.0 );
	float s			= sin( z + t ) * z;
	return s;
}

void main()
{
	Position			= ciBlock0.ciModelView * ciPosition;
	TexCoord			= ciTexCoord0;
	Normal				= normalize( ciNormal );
	vec4 VertPos		= ciPosition;
	
	// texture data
	vec2 samplePos		= vec2( ( gl_InstanceIndex % ciBlock0.uFboRes ), ( gl_InstanceIndex / ciBlock0.uFboRes ) );
	vec4 currPos		= texture( uCurrPosTex, samplePos );
	vec4 prevPos		= texture( uPrevPosTex, samplePos );
	vec4 currVel		= texture( uCurrVelTex, samplePos );
	float leader		= currPos.a;
	vCrowd				= ( 1.0 - pow( min( currVel.a * 0.015, 1.0 ), 0.5 ) ) + 0.25;
	
	float currZ			= VertPos.z;
	float prevZ			= currZ + 1.0;
//	float speed			= length( currVel.xyz ) * 0.075;
	float currVal		= getSinVal( currZ * 0.2, leader );	// currZ * speed
	float prevVal		= getSinVal( prevZ * 0.2, leader );	// prevZ * speed
	
	vec3 currCenter		= vec3( currVal, 0.0, currZ );
	vec3 prevCenter		= vec3( prevVal, 0.0, prevZ );
	vec3 pathDir		= normalize( currCenter - prevCenter );
	vec3 pathUp			= vec3( 0.0, 1.0, 0.0 );
	vec3 pathNormal		= normalize( cross( pathDir, pathUp ) );
	
	mat3 basis			= mat3( pathNormal, pathUp, pathDir );
	vec3 at				= vec3( basis[0][0], basis[1][0], basis[2][0] );
	vec3 bt				= vec3( basis[0][1], basis[1][1], basis[2][1] );
	vec3 ct				= vec3( basis[0][2], basis[1][2], basis[2][2] );
	mat3 trans			= mat3( at, bt, -ct );
	
	vec3 spoke			= vec3( VertPos.x, VertPos.y, 0.0 );
	VertPos.xyz			= currCenter + basis * spoke;
	
	// calculate matrix
	vec3 dir			= normalize( currVel.xyz ); //normalize( currPos.xyz - prevPos.xyz );	
	vec3 right			= normalize( cross( vec3( 0.0, 1.0, 0.0 ), dir ) );
	vec3 up				= normalize( cross( dir, right ) );
	mat3 m				= mat3( right, up, dir );
	
	// set final vertex
	vec3 vertexPos		= m * VertPos.xyz;
	VertPos				= vec4( vertexPos + currPos.xyz, ciPosition.w );
	
	PosCoords			= VertPos.xz * 0.025;
	
	ViewPos				= ciBlock0.ciModelMatrix * VertPos;
	WorldPos			= ciBlock0.ciModelView * VertPos;
	Normal				= ciBlock0.ciNormalMatrix * normalize( m * ( trans * Normal ) );
	ViewNormal			= vec3( ciBlock0.ciViewMatrixInverse * vec4( Normal, 0.0 ) );
	
	// slide fish forward to help with collision avoidance appearance
	//	VertPos.xyz			+= dir * 1.2;
	
	LightPos			= vec3( ciBlock0.ciViewMatrix * ciBlock0.uLightPos );
	LightDir			= normalize( LightPos - ( VertPos.xyz * ciBlock0.uLightPos.w ) );
	
	vec4 newVertPos		= ciBlock0.ciViewMatrix * VertPos;
	FogPer				= pow( 1.0 - min( max( length( newVertPos.xyz ) - ciBlock0.uFogNearDist, 0.0 ) / ciBlock0.uFogFarDist, 1.0 ), ciBlock0.uFogPower );
	
	gl_Position			= ciBlock0.ciModelViewProjection * VertPos;
	ShadowCoord			= ( ciBlock0.uShadowMvp * ciBlock0.ciModelMatrix ) * VertPos;
}

