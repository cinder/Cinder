#version 150 core

smooth in vec4 vPosition;
smooth in vec4 vPrevPosition;

out vec4 fVelocity;

void main(void) {
	// place each position in range (0.0,1.0)
  vec2 a = (vPosition.xy / vPosition.w) * 0.5 + 0.5;
  vec2 b = (vPrevPosition.xy / vPrevPosition.w) * 0.5 + 0.5;
	// store velocity in range (-1.0,1.0)
  fVelocity.xy = (a - b);
  fVelocity.zw = vec2( 1.0 );
}