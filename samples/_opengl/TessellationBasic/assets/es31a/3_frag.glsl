#version 310 es
#extension GL_ANDROID_extension_pack_es31a : require
precision highp float;

out vec4 oColor;
sample in vec4 GeomColor;

void main()
{
	oColor = GeomColor;
}
