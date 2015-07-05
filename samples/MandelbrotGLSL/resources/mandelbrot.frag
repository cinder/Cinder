#version 150

// Sets the maximum number of iterations per pixel.
// Note: anything above 256 is a waste of energy,
//       because of the limited floating point precision.
const int kIterations = 256;

uniform sampler2D uTex0;
uniform vec2      uCenter;
uniform float     uAspectRatio = 1.33333;
uniform float     uScale = 1.0;

in vec2 vertTexCoord0;

out vec4 fragColor;

void main()
{
	vec2 texCoord = vec2( 0 );

	// Perform MandelBrot iteration.
	vec2 z, c;
	c.x = z.x = uAspectRatio * ( vertTexCoord0.x * 2.0 - 1.0 ) * uScale + uCenter.x;
	c.y = z.y = ( vertTexCoord0.y * 2.0 - 1.0 ) * uScale + uCenter.y;

	int i;
	for( i = 0; i < kIterations; i++ ) {
		z = vec2( z.x * z.x - z.y * z.y, z.y * z.x + z.x * z.y ) + c;

		if( dot( z, z ) > 4.0 ) {
			texCoord = vec2( float( i ) / 100.0, 0.0 );
			break;
		}
	}

	// Lookup color from LUT.
	vec3 color = texture( uTex0, texCoord ).rgb;

	// Output color.
	fragColor = vec4( color, 1.0 );
}