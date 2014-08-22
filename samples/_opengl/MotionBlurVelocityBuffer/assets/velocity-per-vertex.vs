#version 150 core

// to support per-vertex animation, we need to pass
// both the current and previous position of each vertex.
in vec4 ciPosition;
in vec4 PrevPosition;

uniform mat4 uModelMatrix;
uniform mat4 uPrevModelMatrix;
uniform mat4 uViewProjection;

smooth out vec4 vPosition;
smooth out vec4 vPrevPosition;

void main(void) {
  vPosition = uViewProjection * uModelMatrix * ciPosition;
  vPrevPosition = uViewProjection * uPrevModelMatrix * PrevPosition;

  gl_Position = vPosition;
}
