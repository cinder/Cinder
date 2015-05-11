#version 330 core

in vec4 ciPosition;
in vec4 ciColor;

uniform mat4 uModelMatrix;
uniform mat4 uPrevModelMatrix;
uniform mat4 uViewProjection;

smooth out vec4 vPosition;
smooth out vec4 vPrevPosition;
smooth out vec4 vColor;

void main(void) {
  vPosition = uViewProjection * uModelMatrix * ciPosition;
  vPrevPosition = uViewProjection * uPrevModelMatrix * ciPosition;

  gl_Position = vPosition;
  vColor = ciColor;
}
