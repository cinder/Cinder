#version 130

uniform sampler2D	tex0;

in vec4		vVertex;
in vec3		vNormal;
in vec2		vTexCoord0;
in vec4		vColor;

out vec4	oColor;

void main()
{	
	const vec4	ambient = vec4(0.1, 0.1, 0.1, 1);
	const vec4	specular = vec4(0.7, 0.7, 0.4, 1);
	const float shininess = 50.0;
	
	vec3 N = normalize( vNormal );
	vec3 L = normalize( -vVertex.xyz );   
	vec3 E = normalize( -vVertex.xyz ); 
	vec3 R = normalize( -reflect( L, N ) );  

	// ambient term 
	vec4 Iamb = ambient;    

	// diffuse term
	vec4 Idiff = vColor;
	Idiff *= max( dot( N, L ), 0.0 );
	Idiff = clamp ( Idiff, 0.0, 1.0 );     

	// specular term
	vec4 Ispec = specular; 
	Ispec *= pow( max( dot( R, E ), 0.0 ), shininess );
	Ispec = clamp( Ispec, 0.0, 1.0 ); 

	// reflection term (fake ground reflection)
	float m = 2.0 * sqrt(R.x * R.x + R.y * R.y + (R.z + 1.0) * (R.z + 1.0) );
	float f = pow(0.5 - R.y / m, 5.0);
	vec4 Irefl = vec4(f, f, f, 1.0) * specular;

	// final color 
	oColor = Iamb + Irefl + Idiff + Ispec;
}