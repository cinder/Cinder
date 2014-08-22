#version 150 core

in vec4 ciPosition;
in vec2 ciTexCoord0;
in vec4 ciColor;

uniform mat4 uModelMatrix;
uniform mat4 uViewProjection;

smooth out vec4 vPosition;
smooth out vec2 vTexCoord;
smooth out vec4 vColor;

void main(void) {
  vPosition = uViewProjection * uModelMatrix * ciPosition;
  vTexCoord = ciTexCoord0;
  vColor = ciColor;

  gl_Position = vPosition;
}
