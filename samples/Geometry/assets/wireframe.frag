#version 150

uniform float uBrightness;

in VertexData {
	noperspective vec3 distance;
	vec4 color;
	vec2 texcoord;
} vVertexIn;

out vec4 oFragColor;

void main()
{
	// determine frag distance to closest edge
	float fNearest = min( min( vVertexIn.distance[0], vVertexIn.distance[1] ), vVertexIn.distance[2] );
	float fEdgeIntensity = exp2( -1.0 * fNearest * fNearest );

	// blend between edge color and face color
	vec3 vFaceColor = vVertexIn.color.rgb;
	vec3 vEdgeColor = vec3( 0.2 );

	oFragColor.rgb = mix( vFaceColor, vEdgeColor, fEdgeIntensity ) * uBrightness;
	oFragColor.a = 0.65;
}
