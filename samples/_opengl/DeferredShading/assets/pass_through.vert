uniform mat4 ciModelViewProjection;

in vec4 	ciPosition;
in vec2 	ciTexCoord0;

out vec2    uv;

void main( void ) 
{
    uv = ciTexCoord0;
	gl_Position = ciModelViewProjection * ciPosition;
}
