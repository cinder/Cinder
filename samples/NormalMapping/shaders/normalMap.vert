#version 110

attribute vec3 tangent;
varying vec3 lightVec;
varying vec3 eyeVec;

uniform vec3 lightLoc;
	
void main()
{
	// Building the matrix Eye Space -> Tangent Space
	vec3 n = normalize( gl_NormalMatrix * gl_Normal );
	vec3 t = normalize( gl_NormalMatrix * tangent );
	vec3 b = cross( n, t );
	
	vec3 vertexPosition = vec3( gl_ModelViewMatrix * gl_Vertex );
	vec3 lightDir = normalize( lightLoc - vertexPosition );
		
	// transform light and half angle vectors by tangent basis
	vec3 v;
	v.x = dot( lightDir, t );
	v.y = dot( lightDir, b );
	v.z = dot( lightDir, n );
	lightVec = normalize( v );
	
	v.x = dot( vertexPosition, t);
	v.y = dot( vertexPosition, b);
	v.z = dot( vertexPosition, n);
	eyeVec = normalize( v );
	
	gl_TexCoord[0] =  gl_MultiTexCoord0;	  
	gl_Position = ftransform();
}
