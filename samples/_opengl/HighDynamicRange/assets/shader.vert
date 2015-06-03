#version 150
 
// Attributes per vertex: position and texture coordinates
in vec4 ciPosition;
in vec4 ciTexCoord0;
 
uniform mat4   ciModelViewProjection;
 
// Color to fragment program
out vec2 TexCoord0;
 
void main(void) 
{ 
    TexCoord0 = ciTexCoord0.st;
    gl_Position = ciModelViewProjection * ciPosition;
}