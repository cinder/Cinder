#version 400 core

const uint MATERIAL_COUNT = 3;

uniform float		uAttenuation;
uniform vec2		uSize;
uniform sampler2D	uSamplerAlbedo;
uniform isampler2D	uSamplerMaterial;

struct Material
{
	vec4	ambient;
	vec4	diffuse;
	vec4	emissive;
	vec4	specular;
	float	shininess;
	float	twoSided;
	uint 	pad0;
	uint 	pad1;
};

layout (std140) uniform Materials
{
	Material uMaterials[ MATERIAL_COUNT ];
};

in Vertex
{
	vec2 	uv;
} vertex;

out vec4 	oColor;

void main( void )
{
	vec4 sum = vec4( 0.0 );
	sum += texture( uSamplerAlbedo, vertex.uv + -10.0 * uSize ) * uMaterials[ texture( uSamplerMaterial, vertex.uv + -10.0 * uSize ).r ].emissive * 0.009167927656011385;
	sum += texture( uSamplerAlbedo, vertex.uv +  -9.0 * uSize ) * uMaterials[ texture( uSamplerMaterial, vertex.uv +  -9.0 * uSize ).r ].emissive * 0.014053461291849008;
	sum += texture( uSamplerAlbedo, vertex.uv +  -8.0 * uSize ) * uMaterials[ texture( uSamplerMaterial, vertex.uv +  -8.0 * uSize ).r ].emissive * 0.020595286319257878;
	sum += texture( uSamplerAlbedo, vertex.uv +  -7.0 * uSize ) * uMaterials[ texture( uSamplerMaterial, vertex.uv +  -7.0 * uSize ).r ].emissive * 0.028855245532226279;
	sum += texture( uSamplerAlbedo, vertex.uv +  -6.0 * uSize ) * uMaterials[ texture( uSamplerMaterial, vertex.uv +  -6.0 * uSize ).r ].emissive * 0.038650411513543079;
	sum += texture( uSamplerAlbedo, vertex.uv +  -5.0 * uSize ) * uMaterials[ texture( uSamplerMaterial, vertex.uv +  -5.0 * uSize ).r ].emissive * 0.049494378859311142;
	sum += texture( uSamplerAlbedo, vertex.uv +  -4.0 * uSize ) * uMaterials[ texture( uSamplerMaterial, vertex.uv +  -4.0 * uSize ).r ].emissive * 0.060594058578763078;
	sum += texture( uSamplerAlbedo, vertex.uv +  -3.0 * uSize ) * uMaterials[ texture( uSamplerMaterial, vertex.uv +  -3.0 * uSize ).r ].emissive * 0.070921288047096992;
	sum += texture( uSamplerAlbedo, vertex.uv +  -2.0 * uSize ) * uMaterials[ texture( uSamplerMaterial, vertex.uv +  -2.0 * uSize ).r ].emissive * 0.079358891804948081;
	sum += texture( uSamplerAlbedo, vertex.uv +  -1.0 * uSize ) * uMaterials[ texture( uSamplerMaterial, vertex.uv +  -1.0 * uSize ).r ].emissive * 0.084895951965930902;
	sum += texture( uSamplerAlbedo, vertex.uv +   0.0 * uSize ) * uMaterials[ texture( uSamplerMaterial, vertex.uv +   0.0 * uSize ).r ].emissive * 0.086826196862124602;
	sum += texture( uSamplerAlbedo, vertex.uv +   1.0 * uSize ) * uMaterials[ texture( uSamplerMaterial, vertex.uv +   1.0 * uSize ).r ].emissive * 0.084895951965930902;
	sum += texture( uSamplerAlbedo, vertex.uv +   2.0 * uSize ) * uMaterials[ texture( uSamplerMaterial, vertex.uv +   2.0 * uSize ).r ].emissive * 0.079358891804948081;
	sum += texture( uSamplerAlbedo, vertex.uv +   3.0 * uSize ) * uMaterials[ texture( uSamplerMaterial, vertex.uv +   3.0 * uSize ).r ].emissive * 0.070921288047096992;
	sum += texture( uSamplerAlbedo, vertex.uv +   4.0 * uSize ) * uMaterials[ texture( uSamplerMaterial, vertex.uv +   4.0 * uSize ).r ].emissive * 0.060594058578763078;
	sum += texture( uSamplerAlbedo, vertex.uv +   5.0 * uSize ) * uMaterials[ texture( uSamplerMaterial, vertex.uv +   5.0 * uSize ).r ].emissive * 0.049494378859311142;
	sum += texture( uSamplerAlbedo, vertex.uv +   6.0 * uSize ) * uMaterials[ texture( uSamplerMaterial, vertex.uv +   6.0 * uSize ).r ].emissive * 0.038650411513543079;
	sum += texture( uSamplerAlbedo, vertex.uv +   7.0 * uSize ) * uMaterials[ texture( uSamplerMaterial, vertex.uv +   7.0 * uSize ).r ].emissive * 0.028855245532226279;
	sum += texture( uSamplerAlbedo, vertex.uv +   8.0 * uSize ) * uMaterials[ texture( uSamplerMaterial, vertex.uv +   8.0 * uSize ).r ].emissive * 0.020595286319257878;
	sum += texture( uSamplerAlbedo, vertex.uv +   9.0 * uSize ) * uMaterials[ texture( uSamplerMaterial, vertex.uv +   9.0 * uSize ).r ].emissive * 0.014053461291849008;
	sum += texture( uSamplerAlbedo, vertex.uv +  10.0 * uSize ) * uMaterials[ texture( uSamplerMaterial, vertex.uv +  10.0 * uSize ).r ].emissive * 0.009167927656011385;

	oColor = vec4( ( uAttenuation * sum.rgb ), 1.0 );
}
