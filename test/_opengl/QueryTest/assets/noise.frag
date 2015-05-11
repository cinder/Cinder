#version 150

uniform float	uTime;
in vec2			vTexCoord0;
out vec4		oColor;

float rand(vec2 co) {
    return fract( sin(dot(co.xy ,vec2( 12.9898, 78.233 ))) * 43758.5453 );
}

void main( void )
{
	float gray = 0.0;
	int iterationNum = 500;
	for( int i=0; i<iterationNum; i++ ) {
		gray += rand( uTime * vTexCoord0 ) / float(iterationNum);
	}
	oColor = vec4(gray);
}