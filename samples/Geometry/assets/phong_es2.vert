#version 100

uniform mat4	ciModelViewProjection;
uniform mat4	ciModelView;
uniform mat3	ciNormalMatrix;

attribute vec4	ciPosition;
attribute vec3	ciNormal;
attribute vec4	ciColor;
attribute vec2	ciTexCoord0;

varying vec4 Position;
varying vec3 Normal;
varying vec4 Color;
varying vec2 TexCoord;

void main()
{
	Position = ciModelView * ciPosition;
	Normal = ciNormalMatrix * ciNormal;
	Color = ciColor;
	TexCoord = ciTexCoord0;

	gl_Position = ciModelViewProjection * ciPosition;
}
