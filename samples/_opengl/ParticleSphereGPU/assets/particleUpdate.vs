#version 330 core

uniform float uMouseStrength;
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
{ // copy base values
  position =  iPosition;
  pposition = iPPostion;
  damping =   iDamping;
  home =      iHome;
  color =     iColor;

  // mouse interaction
  if( uMouseStrength > 0.0 )
  {
    vec3 dir = position - uMousePos;
    float d2 = length( dir );
    d2 *= d2;
    position += vec3(uMouseStrength) * dir / d2;
  }

  // verlet integration
  vec3 vel = (position - pposition) * vec3(damping);
  pposition = position;
  vec3 acc = (home - position) * vec3(32.0f);
  position += vel + acc * vec3(dt2);
}