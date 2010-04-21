
varying vec3 normal;
varying vec3 position;

void main()
{
	normal = normalize( gl_NormalMatrix * gl_Normal );
	vec4 v = vec4( gl_Vertex );

	position = v.xyz;
	gl_Position = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
