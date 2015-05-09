#version 330 core

uniform sampler2D uColorMap; // texture we're blurring
uniform sampler2D uVelocityMap;
uniform sampler2D uNeighborMaxMap;
uniform float     uNoiseFactor = 0.1; // relative to total velocity (so 1.0 is a lot)

uniform int uSamples;

out vec4 fColor;

float rand( in vec2 seed )
{
  return fract( sin( dot( seed.xy, vec2( 12.9898, 78.233 ) ) ) * 43758.5453 );
}

vec4 calcColor()
{
  // generate texture coord from screen information (since this is a fullscreen effect)
  vec2 texelSize = 1.0 / vec2(textureSize(uColorMap, 0));
  vec2 halfPixel = texelSize * 0.5;
  vec2 screenTexCoords = gl_FragCoord.xy * texelSize;

  vec2 velocity = texture( uNeighborMaxMap, screenTexCoords ).xy;
  vec2 localVelocity = texture( uVelocityMap, screenTexCoords ).xy;

  // if there isn't significant motion, just return color at pos
  if( length( velocity ) < halfPixel.x ) {
    return texture( uColorMap, screenTexCoords );
  }

  // accumulate color
  vec4 color = vec4( 0.0 );

  // jitter our samples along their velocity vector
  vec2 jitter = vec2(rand(screenTexCoords) * 0.5 - 1.0) * velocity * uNoiseFactor;

  for( int i = 0; i < uSamples; ++i ) {
    // sample along dominant region motion and actual pixel motion
    float t = mix( -1.0, 1.0, float(i) / float(uSamples - 1.0) );

    // Alternate sampling between local and regional velocity.
    if( i % 2 == 0 ) {
      // region sample
      color += texture( uColorMap, screenTexCoords + jitter + velocity * t + halfPixel ).rgba;
    }
    else {
      // local sample
      color += texture( uColorMap, screenTexCoords + jitter + localVelocity * t + halfPixel ).rgba;
    }
  }
  return color / vec4(uSamples);
}

void main(void)
{
  fColor = calcColor();
}
