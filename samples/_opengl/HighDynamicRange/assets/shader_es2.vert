#version 100
 
// Attributes per vertex: position and texture coordinates
attribute vec4 ciPosition;
attribute vec4 ciTexCoord0;
 
uniform mat4   ciModelViewProjection;
 
// Color to fragment program
varying highp vec2 TexCoord0;
 
void main(void) 
{ 
    TexCoord0 = ciTexCoord0.st;
    gl_Position = ciModelViewProjection * ciPosition;
}