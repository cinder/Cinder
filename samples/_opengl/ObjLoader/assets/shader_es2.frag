precision highp float;

varying lowp vec4		Color;
varying highp vec3		Normal;

void main( void )
{
	vec3 normal = normalize( -Normal );
	float diffuse = max( dot( normal, vec3( 0, 0, -1 ) ), 0.0 );
	gl_FragColor = Color * diffuse;
}