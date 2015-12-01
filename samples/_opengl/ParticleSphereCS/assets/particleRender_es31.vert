#version 310 es 
#extension GL_ARB_shader_storage_buffer_object : require

layout( location = 0 ) in int particleId;
out vec3 vColor;

struct Particle
{
	vec3	pos;
	vec3	ppos;
	vec3	home;
	vec4	color;
	float	damping;
};

layout( std140, binding = 0 ) buffer Part
{
    Particle particles[];
};

uniform mat4 ciModelViewProjection;


void main()
{
	gl_Position = ciModelViewProjection * vec4( particles[particleId].pos, 1 );
	vColor = particles[particleId].color.rgb;
}
