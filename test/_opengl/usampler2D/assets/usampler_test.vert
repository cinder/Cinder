#version 150

in vec4 ciPosition;
in vec2 ciTexCoord0;

uniform mat4 ciModelViewProjection;

out vec2 vTexCoord0;

void main(){
    
    vTexCoord0 = ciTexCoord0;
    gl_Position = ciModelViewProjection * ciPosition;
    
}