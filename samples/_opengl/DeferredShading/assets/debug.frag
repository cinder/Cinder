#if __VERSION__ == 300
	precision highp float;
	precision highp sampler2DShadow;
#endif

const int MODE_ALBEDO	= 0;
const int MODE_NORMAL	= 1;
const int MODE_POSITION	= 2;
const int MODE_EMISSIVE	= 3;

uniform sampler2D uSamplerAlbedo;
uniform sampler2D uSamplerNormalEmissive;
uniform sampler2D uSamplerPosition;

uniform int uMode;

in vec2     uv;
out vec4    oColor;

void main( void )
{
	vec4 color = vec4( 1.0 );
	switch( uMode ) {
	case MODE_ALBEDO:
		color = texture( uSamplerAlbedo, uv );
		break;
	case MODE_NORMAL:
		color = vec4( texture( uSamplerNormalEmissive, uv ).xyz, 1.0 );
		break;
	case MODE_POSITION:
		color = texture( uSamplerPosition, uv );
		break;
	case MODE_EMISSIVE:
		color = vec4( vec3( texture( uSamplerNormalEmissive, uv ).a ), 1.0 );
		break;
	}
	oColor = color;
}
