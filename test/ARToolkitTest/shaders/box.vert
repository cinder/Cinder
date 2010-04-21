#version 110

varying vec3 normal;
varying vec4 worldPosition;
varying vec4 worldN;
varying vec3 viewDir;

uniform vec4 camLoc;

void main()
{
	normal = gl_NormalMatrix * gl_Normal;
	
	gl_Position = ftransform();
	worldPosition = gl_ModelViewMatrix * gl_Vertex;
	worldN = normalize( vec4( gl_NormalMatrix * gl_Normal, 0 ) );
	vec4 viewDir4 = normalize( camLoc - worldPosition );
	viewDir = vec3( viewDir4.xyz );
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
