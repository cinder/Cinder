#version 150

uniform sampler2D	uTex0;
uniform vec4		uColorLeft;
uniform vec4		uColorRight;

in vec4 vertColor;
in vec2 vertTexCoord0;

out vec4 fragColor;

// find the correct half of the texture to sample (side-by-side FBO)
const vec4	kOffset = vec4(0.5, 1.0, 0.5, 0.0);

void main()
{
	fragColor = uColorLeft * texture( uTex0, vertTexCoord0 * kOffset.xy );
	fragColor += uColorRight * texture( uTex0, vertTexCoord0 * kOffset.xy + kOffset.zw );
}