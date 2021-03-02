uniform mat4 ciProjectionMatrix;
uniform mat4 ciModelView;
uniform float time;
in vec3 ciPosition;

in vec2 instanceUV;
in vec3 instancePosition;

out vec2 vUv;

vec3 rotateX(vec3 p, float theta) {
  float s = sin(theta);
  float c = cos(theta);
  return vec3(p.x, p.y * c - p.z * s, p.z * c + p.y * s);
}
vec3 rotateY(vec3 p, float theta) {
  float s = sin(theta);
  float c = cos(theta);
  return vec3(p.x * c + p.z * s, p.y, p.z * c - p.x * s);
}

vec3 rotateZ(vec3 p, float theta) {
  float s = sin(theta);
  float c = cos(theta);
  return vec3(p.x * c - p.y * s, p.y * c + p.x * s, p.z);
}
void main(){
  vec3 pos = ciPosition;
  pos = rotateX(pos,time * 0.5);
  pos = rotateY(pos,time * 0.5);
  pos = rotateZ(pos,time * 0.5);
  
  gl_Position = ciProjectionMatrix * ciModelView * vec4(pos + instancePosition,1.);
  vUv = instanceUV;
}
