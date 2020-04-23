#version 150

uniform mat4 ciModelView;
uniform mat4 ciProjectionMatrix;
uniform mat4 ciModelViewProjection;
uniform mat3 ciNormalMatrix;

uniform bool uUseConcatenatedProjection = true;

in vec4 ciPosition;
in vec3 ciNormal;
in vec4 ciColor;

in vec4 iPosition; // xyz = position, w = scale

out vec4 vertPosition;
out vec3 vertNormal;
out vec4 vertColor;

void main( void )
{
	// Adjust the vertex position by the instance position and scale.
	vec4 position = ciPosition;
	position.xyz = position.xyz * iPosition.w + iPosition.xyz;

	vertPosition = ciModelView * position;
	vertNormal = ciNormalMatrix * ciNormal;
	vertColor = ciColor;

	if( uUseConcatenatedProjection ) {
		// This may introduce substantial rounding errors.
		gl_Position = ciModelViewProjection * position;
	}
	else {
		// This is better. Note that `vertPosition` = `ciModelView * ciPosition`.
		gl_Position = ciProjectionMatrix * vertPosition;
	}
}