#version 150

uniform mat4	ciModelView;

in vec4			ciPosition;

out vec3		vPosition;
out float		vLod;

void main()
{
	float distance = length( ciModelView * ciPosition );

	// vLod = step( 60, distance ) + step( 40, distance );

	if( distance < 40 ) {
		vLod = 0;
	} else if( distance < 60 ) {
		vLod = 1;
	} else {
		vLod = 2;
	}

	vPosition = ciPosition.xyz;
}
