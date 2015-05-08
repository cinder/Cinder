uniform mat4 	ciModelViewProjection;

in vec4 		ciPosition;

void main( void )
{
	gl_Position = ciModelViewProjection * ciPosition;
}
