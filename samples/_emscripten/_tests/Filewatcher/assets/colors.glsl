precision highp float;
in vec2 vUv;
uniform vec2 resolution;
uniform float time;
out vec4 glFragColor;

void main(  )
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = gl_FragCoord.xy/resolution.xy;

    // Time varying pixel color
    vec3 col = 0.5 + 0.5*cos(time+vUv.xyx+vec3(0,2,4));

    // Output to screen
    glFragColor = vec4(col,1.0);
}