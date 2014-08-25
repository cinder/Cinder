#version 330 core

in vec4 ciPosition;

uniform mat4 uModelMatrix;
uniform mat4 uPrevModelMatrix;
uniform mat4 uViewProjection;

smooth out vec4 vPosition;
smooth out vec4 vPrevPosition;

void main(void) {
  vPosition = uViewProjection * uModelMatrix * ciPosition;
  vPrevPosition = uViewProjection * uPrevModelMatrix * ciPosition;

  gl_Position = vPosition;
}
