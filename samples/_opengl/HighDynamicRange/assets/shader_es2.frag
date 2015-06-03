#version 100
 
uniform sampler2D 	uTex0;
uniform highp float uExposure;
 
varying highp vec2 	TexCoord0;
 
void main(void)
{ 
	highp vec4 color = texture2D( uTex0, TexCoord0 );
 
	gl_FragColor = vec4( color.xyz * uExposure, 1.0 );
}