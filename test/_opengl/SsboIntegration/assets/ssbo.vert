#version 430

layout( std430, binding=2 ) buffer transform_data
{
	mat4 transforms[];
};

in vec4 ciPosition;

flat out int vIndex;

uniform mat4 ciModelViewProjection;

void main()
{
	vIndex = gl_InstanceID;
	gl_Position = ciModelViewProjection * transforms[ gl_InstanceID ] * ciPosition;

#if CPU_UPLOAD == 0
	transforms[ gl_InstanceID ] = transforms[ gl_InstanceID ] * mat4( vec4(1,0,0,0), vec4(0,1,0,0), vec4(0,0,1,0), vec4( 1.001,1.001,1.,1 ));
#endif

}