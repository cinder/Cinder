#version 110

uniform sampler2D diffuseMap;
uniform sampler2D specStrengthMap;
uniform sampler2D normalMap;

varying vec3 lightVec;
varying vec3 eyeVec;

void main()
{
	// lookup normal from normal map, move from [0,1] to  [-1, 1] range, normalize
	vec3 normal = 2.0 * texture2D( normalMap, gl_TexCoord[0].st ).rgb - 1.0;
	normal = normalize( normal );

	vec3 eye = normalize( eyeVec );
	vec3 light = normalize( lightVec );

	// compute diffuse lighting
	float lambertFactor = max( dot( light, normal ), 0.0 );
	
	if( lambertFactor > 0.0 ) {
		vec3 diffuseMaterial = texture2D( diffuseMap, gl_TexCoord[0].st ).rgb;
		
		float specStrength = texture2D( specStrengthMap, gl_TexCoord[0].st ).r;
		vec3 reflect = normalize( 2.0 * lambertFactor * normal - light );
		float shininess = pow( max( dot( reflect, -eye ), 0.0 ), 8.0 ) * specStrength * 1.0;
		 
		gl_FragColor.rgb = diffuseMaterial * lambertFactor;
		gl_FragColor.rgb +=	vec3( shininess, shininess, shininess );
	}
	else
		gl_FragColor.rgb = vec3( 0, 0, 0 );
}