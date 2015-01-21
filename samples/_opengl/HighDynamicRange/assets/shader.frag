#version 150
 
out vec4 oColor;
 
uniform sampler2D 	uTex0;
uniform float 		uExposure;
 
in vec2 			TexCoord0;
 
void main(void)
{ 
	vec4 color = texture( uTex0, TexCoord0 );
 
	oColor = color * uExposure;
	oColor.a = 1.0;
}