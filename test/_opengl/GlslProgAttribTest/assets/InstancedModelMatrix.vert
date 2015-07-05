precision highp float;

in vec4 ciPosition;
in vec3 ciColor;
in mat4 aModelMatrix;

uniform mat4 ciViewProjection;

out vec4 vColor;

void main() {
	gl_Position = ciViewProjection * aModelMatrix * ciPosition;
	vColor = vec4(ciColor, 1);
}