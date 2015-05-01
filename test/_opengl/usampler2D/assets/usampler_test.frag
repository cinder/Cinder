#version 150

in vec2 vTexCoord0;

uniform usampler2D test;

out vec4 FragColor;

void main(){
    
    uint ui = texture( test, vTexCoord0 ).r ;
    
    float c = 0.;
    
    if( ui == 65000u ) c = 1.;
    
    FragColor = vec4( c, c, c, 1. );
    
}