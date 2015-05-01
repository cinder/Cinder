#version 150

uniform sampler2D uTex0;
uniform vec2 uCenter;
uniform float uScale;
uniform int uIter;

in vec2 TexCoord0;
out vec4 oColor;

void main()
{
    vec2 z, c;

    c.x = 1.3333 * ( TexCoord0.x - 0.5) * uScale - uCenter.x;
    c.y = ( TexCoord0.y - 0.5) * uScale - uCenter.y;

    int i;
    z = c;
    for( i = 0; i < uIter; i++ ) {
        float x = (z.x * z.x - z.y * z.y) + c.x;
        float y = (z.y * z.x + z.x * z.y) + c.y;

        if( ( x * x + y * y ) > 4.0 ) break;
        z.x = x;
        z.y = y;
    }
	vec2 texCoord = vec2( (i == uIter ? 0.0 : float(i)) / 100.0, 0.0 );
	vec3 color = texture( uTex0, texCoord ).rgb;

	oColor = vec4( color, 1.0 );
}