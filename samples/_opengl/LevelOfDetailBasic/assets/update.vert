#version 150

uniform mat4 ciModelView;

in vec4 ciPosition;

out vec3 vPosition;
out float vLod;

void main()
{
	float distance = length( ciModelView * ciPosition );

	// use the step function to avoid GPU branching
	// vLod = step( 40.0f, distance ) + step( 60.0f, distance );

	if( distance < 40.0f ) {
		vLod = 0;
	} else if( distance < 60.0f ) {
		vLod = 1;
	} else {
		vLod = 2;
	}

	vPosition = ciPosition.xyz;
}
