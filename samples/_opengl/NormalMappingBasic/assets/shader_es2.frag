#version 100

precision highp float;

uniform sampler2D	uDiffuseMap, uNormalMap;
uniform vec3		uLightLocViewSpace;

// inputs passed from the vertex shader
varying highp vec4		VertexViewSpace;
varying highp vec3		NormalViewSpace, TangentViewSpace, BitangentViewSpace;
varying highp vec2		TexCoord0;

void main()
{
	// fetch the normal from the normal map
	highp vec3 vMappedNormal = texture2D(uNormalMap, TexCoord0.st).rgb * 2.0 - 1.0;

	// modify it using the normal & tangents from the 3D mesh (normal mapping)
	highp vec3 normal = normalize((TangentViewSpace * vMappedNormal.x) + (BitangentViewSpace * vMappedNormal.y) + (NormalViewSpace * vMappedNormal.z));

	highp vec3 vToCamera = normalize( -VertexViewSpace.xyz );
	highp vec3 light = normalize( uLightLocViewSpace - VertexViewSpace.xyz );
	highp vec3 reflection = normalize(-reflect(light, normal));

	// calculate diffuse term
	float fDiffuse = max( dot(normal, light), 0.0 );
	fDiffuse = clamp( fDiffuse, 0.1, 1.0 );

	// calculate specular term
	float fSpecularPower = 20.0;
	float fSpecular = pow( max( dot(reflection, vToCamera), 0.0), fSpecularPower );
	fSpecular = clamp(fSpecular, 0.0, 1.0);

	highp vec3 vDiffuseColor = vec3( fDiffuse ) * texture2D( uDiffuseMap, TexCoord0 ).rgb;
	highp vec3 vSpecularColor = vec3( fSpecular );

	gl_FragColor = vec4( (vDiffuseColor + vSpecularColor).rgb, 1.0 );
}