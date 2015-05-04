#if !defined ( NUM_LIGHTS )
#define NUM_LIGHTS 1
#endif

struct Light
{
	vec4	ambient;
	vec4	diffuse;
	vec4	specular;
	vec3	position;
	float	intensity;
	float	radius;
	float	volume;
	uint	pad0;
	uint	pad1;
};

layout (std140) uniform Lights
{
	Light uLights[ NUM_LIGHTS ];
};
 