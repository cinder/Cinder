#version 330 core

uniform sampler2D uVelocityMap; // full-resolution velocity
uniform int uTileSize;

out vec2 fVelocity;

void main()
{	// texel size is dependent on our output dimensions
	// size of input texel, for stepping within a tile
	vec2 inputTexel = 1.0 / vec2(textureSize(uVelocityMap, 0));
	// size of output texel, for getting tile start coord
	vec2 outputTexel = 1.0 / (vec2(textureSize(uVelocityMap, 0)) / uTileSize);
	vec2 screenTexCoords = gl_FragCoord.xy * outputTexel;

	// there is something wrong here; must be
	vec2 maxTileVelocity = vec2( 0.0 );
	float maxMagnitude = 0.0;
	for( int u = 0; u < uTileSize; ++u ) {
		for( int v = 0; v < uTileSize; ++v ) {
			vec2 velocity = texture( uVelocityMap, screenTexCoords + (vec2(u,v) * inputTexel) ).xy;
			float magnitude = length( velocity );
			if( magnitude > maxMagnitude ) {
				maxMagnitude = magnitude;
				maxTileVelocity = velocity;
			}
		}
	}

	fVelocity = maxTileVelocity;
}