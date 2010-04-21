varying vec4 ecPosition; 
varying vec3 ecPosition3; 
varying vec3 normal;
varying vec4 ambientGlobal;

void main()
{
	ecPosition		= gl_ModelViewMatrix * gl_Vertex; 
	ecPosition3		= vec3( ecPosition ) / ecPosition.w;

	normal			= gl_NormalMatrix * gl_Normal;
	gl_Position		= gl_ModelViewProjectionMatrix * gl_Vertex;
	
	ambientGlobal	= gl_LightModel.ambient * gl_FrontMaterial.ambient;
}