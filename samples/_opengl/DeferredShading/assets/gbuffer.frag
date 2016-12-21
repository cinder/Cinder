#if __VERSION__ == 300
	precision highp float;
	precision highp sampler2DShadow;
#endif

uniform float uEmissive;

in vec4 color;
in vec3 normal;
in vec4 position;

layout (location = 0) out vec4 oAlbedo;
layout (location = 1) out vec4 oNormalEmissive;
layout (location = 2) out vec4 oPosition;

void main( void )
{
	oAlbedo 		= color;
	oNormalEmissive	= vec4( normalize( normal ), uEmissive );
	oPosition 		= position;
}
