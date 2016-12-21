#version 420

uniform float uCurrentLod;
uniform float uNear, uFar, uTanFov, uAspectRatio, uDelta;
uniform vec3 uCameraPos, uCameraX, uCameraY, uCameraZ;

#ifdef USE_ATOMIC_COUNTER
layout( binding = 0, offset = 4 ) uniform atomic_uint uInstanceCounter;
#endif

layout( points ) in;
layout( points, max_vertices = 1 ) out;

in vec3 vPosition[];
in float vLod[];

out vec3 oPosition;
out float oLod;

bool insideViewFrustum( vec3 point )
{
	vec3 v = point - uCameraPos;
	float VdotZ = dot( v, uCameraZ );

	if( VdotZ > uFar || VdotZ < uNear )
		return false;

	float VdotY = dot( v, uCameraY );
	float h = VdotZ * uTanFov + uDelta;

	if( VdotY > h || VdotY < -h )
		return false;

	float VdotX = dot( v, uCameraX );
	float w = h * uAspectRatio + uDelta;

	if( VdotX > w || VdotX < -w )
		return false;

	return true;
}

void main()
{
	if( vLod[0] == uCurrentLod && insideViewFrustum( vPosition[0] ) ) {
		oPosition = vPosition[0];
		oLod = vLod[0];

		EmitVertex();
		EndPrimitive();

#ifdef USE_ATOMIC_COUNTER
		atomicCounterIncrement( uInstanceCounter );
#endif
	}
}
