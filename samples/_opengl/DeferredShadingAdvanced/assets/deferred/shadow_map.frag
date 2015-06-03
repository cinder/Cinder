layout (location = 0) out vec4 oColor;

void main( void )
{
	oColor = vec4( gl_FragCoord.zzz, 1.0 );
}
 