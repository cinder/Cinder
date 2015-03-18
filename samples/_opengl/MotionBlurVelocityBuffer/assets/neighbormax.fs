#version 330 core

uniform sampler2D uTileMap;	// downsampled velocity
out vec2 fVelocity;

void main() {
	vec2 texelSize = 1.0 / vec2(textureSize(uTileMap, 0));
	vec2 screenTexCoords = gl_FragCoord.xy * texelSize;

	vec2 velocity = vec2( 0.0 );
	float magnitude = 0.0;
	// sample non-diagonal neighbors (and current position)
	for( int offset = -1; offset <= 1; ++offset ) {
		vec2 vertical = texture( uTileMap, screenTexCoords + vec2(offset, 0.0) * texelSize ).xy;
		vec2 horizontal = texture( uTileMap, screenTexCoords + vec2(0.0, offset) * texelSize ).xy;
		float mv = length( vertical );
		float mh = length( horizontal );
		if( mv > magnitude ) {
			velocity = vertical;
			magnitude = mv;
		}
		if( mh > magnitude ) {
			velocity = horizontal;
			magnitude = mh;
		}
	}
	// sample diagonals if they point to center
	vec2 ul = texture( uTileMap, screenTexCoords + texelSize * vec2(-1.0) ).xy;
	if( dot( ul, vec2( 0.71, 0.71 ) ) > 0.0 ) {
		float m = length(ul);
		if( m > magnitude ) {
			magnitude = m;
			velocity = ul;
		}
	}
	vec2 ll = texture( uTileMap, screenTexCoords + texelSize * vec2(-1.0,1.0) ).xy;
	if( dot( ll, vec2( 0.71, -0.71 ) ) > 0.0 ) {
		float m = length(ll);
		if( m > magnitude ) {
			magnitude = m;
			velocity = ll;
		}
	}
	vec2 ur = texture( uTileMap, screenTexCoords + texelSize * vec2(1.0,-1.0) ).xy;
	if( dot( ur, vec2( -0.71, 0.71 ) ) > 0.0 ) {
		float m = length(ur);
		if( m > magnitude ) {
			magnitude = m;
			velocity = ur;
		}
	}
	vec2 lr = texture( uTileMap, screenTexCoords + texelSize * vec2(1.0,1.0) ).xy;
	if( dot( lr, vec2( -0.71, -0.71 ) ) > 0.0 ) {
		float m = length(lr);
		if( m > magnitude ) {
			magnitude = m;
			velocity = lr;
		}
	}
	// store screen-space velocity
	fVelocity = velocity;
}
