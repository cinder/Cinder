#version 150 core

uniform sampler2D uColorMap; // texture we're blurring
uniform sampler2D uVelocityMap;
uniform sampler2D uNeighborMaxMap;

uniform int uSamples;

out vec4 oColor;

const float kFactor = 100.0;

float rand( in vec2 seed )
{
  return fract( sin( dot( seed.xy, vec2( 12.9898, 78.233 ) ) ) * 43758.5453 );
}

vec4 calcColor()
{
  vec2 texelSize = 1.0 / vec2(textureSize(uColorMap, 0));
  vec2 halfPixel = texelSize * 0.5;
  vec2 screenTexCoords = gl_FragCoord.xy * texelSize;

  vec2 velocity = texture( uNeighborMaxMap, screenTexCoords ).xy;
  vec2 localVelocity = texture( uVelocityMap, screenTexCoords ).xy;
  if( length( velocity ) < halfPixel.x ) {
    // if there isn't significant motion, just return color at pos
    return texture( uColorMap, screenTexCoords );
  }
  // accumulating color
  vec4 color = vec4( 0.0 );
  // pseudorandom offset to jitter our samples
  vec2 jitter = vec2(rand(screenTexCoords) * 0.5 - 1.0) * texelSize;
  for( int i = 0; i < uSamples; ++i ) {
    // sample along dominant region motion and actual pixel motion
    float t = mix( -1.0, 1.0, float(i) / float(uSamples - 1.0) );
    if( i % 2 == 0 ) {  // use dominant region velocity
      color += texture( uColorMap, screenTexCoords + jitter + velocity * t + halfPixel ).rgba;
    }
    else {  // use local velocity
      color += texture( uColorMap, screenTexCoords + jitter + localVelocity * t + halfPixel ).rgba;
    }
  }
  return color / vec4(uSamples);
}

void main(void)
{
  oColor = calcColor();
  // oColor.rg = screenTexCoords;
  // oColor.ba = vec2(1.0);
}
