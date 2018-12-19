precision highp float;

uniform sampler2D uTex0;

in vec2 vUv;
out vec4 glFragColor;

void main(){

  vec4 tex = texture(uTex0,vUv);

  glFragColor = tex;
}
