#version 330 core

in vec4 ciPosition;
in vec2 ciTexCoord0;

uniform mat4 ciModelViewProjection;

smooth out vec2 vTexCoord;

void main() {
  gl_Position = ciModelViewProjection * ciPosition;
  vTexCoord = ciTexCoord0;
}