#version 150

uniform sampler2D tex0;

in vec4 vVertex;
in vec3 vNormal;
in vec2 vTexCoord0;

out vec4 oColor;

void main()
{	
	const vec4	ambient = vec4( 0.1, 0.1, 0.1, 1 );
	const vec4	diffuse = vec4( 0.9, 0.9, 0.9, 1 );
	const vec4	specular = vec4( 1, 1, 1, 1 );
	const float shinyness = 50.0;
	
	vec3 L = normalize( vec3( 30.0 ) - vVertex.xyz );
	vec3 E = normalize( -vVertex.xyz );
	vec3 R = normalize( -reflect( L, vNormal ) );

	// ambient term 
	vec4 Iamb = ambient;    

	// diffuse term
	vec4 Idiff = texture( tex0, vTexCoord0 ) * diffuse;
	Idiff *= max( dot( vNormal, L ), 0.0 );
	Idiff = clamp( Idiff, 0.0, 1.0 );

	// specular term
	vec4 Ispec = specular; 
	Ispec *= pow( max( dot(R,E), 0.0 ), shinyness );
	Ispec = clamp( Ispec, 0.0, 1.0 );

	// final color 
	oColor = Iamb + Idiff + Ispec;	
}