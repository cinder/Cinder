#if __VERSION__ == 300
	precision highp float;
	precision highp sampler2DShadow;
#endif

out vec4 oColor;

void main( void )
{
	oColor = vec4( gl_FragCoord.zzz, 1.0 );
}
 