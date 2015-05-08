#include "../../common/vertex_in.glsl"
#include "../../common/unpack.glsl"

// SAO http://graphics.cs.williams.edu/papers/SAOHPG12/

uniform float uNear;

layout (location = 0) out vec4 oColor;

void main( void )
{
	oColor = vec4( vec3( uNear / ( 1.0 - texture( uSamplerDepth, vertex.uv ).r ) ), 1.0 );
}
 