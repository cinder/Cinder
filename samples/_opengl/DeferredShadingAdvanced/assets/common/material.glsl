#if !defined ( NUM_MATERIALS )
#define NUM_MATERIALS 1
#endif

struct Material
{
	vec4	ambient;
	vec4	diffuse;
	vec4	emissive;
	vec4	specular;
	float	shininess;
	uint	pad0;
	uint	pad1;
	uint	pad2;
};

layout (std140) uniform Materials
{
	Material uMaterials[ NUM_MATERIALS ];
};

uniform isampler2D uSamplerMaterial;
 