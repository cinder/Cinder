#version 330 core

const int MATERIAL_COUNT	= 3;

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
	vec3	pad;
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

vec4 getColor( float offset )
{
	return texture( uSamplerAlbedo, vertex.uv + offset * uSize );
}

float getEmissive( float offset )
{
	int id = int( texture( uSamplerMaterial, vertex.uv + offset * uSize ).r );
	return uMaterials[ id ].emissive;
}

void main( void )
{
	vec4 sum = vec4( 0.0 );
	sum += getColor( -10.0 ) * getEmissive( -10.0 ) * 0.009167927656011385;
	sum += getColor(  -9.0 ) * getEmissive(  -9.0 ) * 0.014053461291849008;
	sum += getColor(  -8.0 ) * getEmissive(  -8.0 ) * 0.020595286319257878;
	sum += getColor(  -7.0 ) * getEmissive(  -7.0 ) * 0.028855245532226279;
	sum += getColor(  -6.0 ) * getEmissive(  -6.0 ) * 0.038650411513543079;
	sum += getColor(  -5.0 ) * getEmissive(  -5.0 ) * 0.049494378859311142;
	sum += getColor(  -4.0 ) * getEmissive(  -4.0 ) * 0.060594058578763078;
	sum += getColor(  -3.0 ) * getEmissive(  -3.0 ) * 0.070921288047096992;
	sum += getColor(  -2.0 ) * getEmissive(  -2.0 ) * 0.079358891804948081;
	sum += getColor(  -1.0 ) * getEmissive(  -1.0 ) * 0.084895951965930902;
	sum += getColor(   0.0 ) * getEmissive(   0.0 ) * 0.086826196862124602;
	sum += getColor(   1.0 ) * getEmissive(   1.0 ) * 0.084895951965930902;
	sum += getColor(   2.0 ) * getEmissive(   2.0 ) * 0.079358891804948081;
	sum += getColor(   3.0 ) * getEmissive(   3.0 ) * 0.070921288047096992;
	sum += getColor(   4.0 ) * getEmissive(   4.0 ) * 0.060594058578763078;
	sum += getColor(   5.0 ) * getEmissive(   5.0 ) * 0.049494378859311142;
	sum += getColor(   6.0 ) * getEmissive(   6.0 ) * 0.038650411513543079;
	sum += getColor(   7.0 ) * getEmissive(   7.0 ) * 0.028855245532226279;
	sum += getColor(   8.0 ) * getEmissive(   8.0 ) * 0.020595286319257878;
	sum += getColor(   9.0 ) * getEmissive(   9.0 ) * 0.014053461291849008;
	sum += getColor(  10.0 ) * getEmissive(  10.0 ) * 0.009167927656011385;

	oColor = vec4( ( uAttenuation * sum.rgb ), 1.0 );
}
