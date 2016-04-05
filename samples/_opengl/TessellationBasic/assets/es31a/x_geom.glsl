#version 310 es
#extension GL_ANDROID_extension_pack_es31a : require

layout(triangles) in;
layout(line_strip, max_vertices = 4) out;

in vec4 TesColor[];
sample out vec4 GeomColor;

void main()
{
    gl_Position = gl_in[0].gl_Position;
    GeomColor = TesColor[0];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    GeomColor = TesColor[1];
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    GeomColor = TesColor[2];
    EmitVertex();

    gl_Position = gl_in[0].gl_Position;
    GeomColor = TesColor[0];
    EmitVertex();
}