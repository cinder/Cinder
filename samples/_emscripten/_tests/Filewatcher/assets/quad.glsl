
uniform mat4 ciModelViewProjection;
in vec3 ciPosition;
in vec2 ciTexCoord0;

out vec2 vUv;

void main(){
	vUv = ciTexCoord0;
	gl_Position = ciModelViewProjection * vec4(ciPosition,1.);
}
