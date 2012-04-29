//
// Basic Texture shader for GL_TEXTURE_2D
//
// Studio Avante / Roger Sodre
//

void main()
{
	//gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
	gl_Position = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
