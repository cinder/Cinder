#version 300 es

uniform float uMouseForce;
uniform vec3  uMousePos;

in highp vec3   iPosition;
in highp vec3   iPPostion;
in highp vec3   iHome;
in highp vec4   iColor;
in highp float  iDamping;

out highp vec3  position;
out highp vec3  pposition;
out highp vec3  home;
out highp float damping;
out highp vec4  color;

const highp float dt2 = 1.0 / (60.0 * 60.0);

void main()
{
	position =  iPosition;
	pposition = iPPostion;
	damping =   iDamping;
	home =      iHome;
	color =     iColor;

	// mouse interaction
	if( uMouseForce > 0.0 ) {
		highp vec3 dir = position - uMousePos;
		highp float d2 = length( dir );
		d2 *= d2;
		position += uMouseForce * dir / d2;
	}

	highp vec3 vel = (position - pposition) * damping;
	pposition = position;
	highp vec3 acc = (home - position) * 32.0f;
	position += vel + acc * dt2;
}