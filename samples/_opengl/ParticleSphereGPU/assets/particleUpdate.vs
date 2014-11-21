#version 150 core

uniform float uMouseForce;
uniform vec3  uMousePos;

in vec3   iPosition;
in vec3   iPPostion;
in vec3   iHome;
in float  iDamping;
in vec4   iColor;

out vec3  position;
out vec3  pposition;
out vec3  home;
out float damping;
out vec4  color;

const float dt2 = 1.0 / (60.0 * 60.0);

void main()
{
	position =  iPosition;
	pposition = iPPostion;
	damping =   iDamping;
	home =      iHome;
	color =     iColor;

	// mouse interaction
	if( uMouseForce > 0.0 ) {
		vec3 dir = position - uMousePos;
		float d2 = length( dir );
		d2 *= d2;
		position += uMouseForce * dir / d2;
	}

	vec3 vel = (position - pposition) * damping;
	pposition = position;
	vec3 acc = (home - position) * 32.0f;
	position += vel + acc * dt2;
}