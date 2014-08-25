#version 330 core

uniform sampler2D uTex0;

smooth in vec2 vTexCoord;

out vec4 fColor;


void main()
{
  vec4 color = vec4(1.0);
  color.rg = texture( uTex0, vTexCoord ).rg * 255.0 + vec2(0.5);

  fColor = color;
}
