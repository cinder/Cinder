varying vec3 ObjPos;
varying vec3 EyePos;
varying vec3 Normal;

void main()
{
	ObjPos = vec3( gl_Vertex );
 	EyePos = vec3( gl_ModelViewMatrix * gl_Vertex );
	Normal = gl_NormalMatrix * gl_Normal;
	
	gl_Position = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
