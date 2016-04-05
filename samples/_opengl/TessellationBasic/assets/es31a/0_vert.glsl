#version 310 es
#extension GL_ANDROID_extension_pack_es31a : require

in vec4 ciPosition;
in vec4 ciColor;

out vec4 VsColor;

uniform float uRadius;

void main ()
{
	gl_Position = vec4( ciPosition.xyz * uRadius, 1.0 ); // control points out == vertex points in
	VsColor = ciColor;
}
