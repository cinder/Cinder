#include "vertex_in.glsl"
#include "decode.glsl"

#define M_PI 3.1415926535897932384626433832795

uniform sampler2D uSamplerNormal;

out vec4 	oColor;

// sampling radius is in view space
#define SAMPLING_RADIUS 0.5
#define NUM_SAMPLING_DIRECTIONS 8
// sampling step is in texture space
#define SAMPLING_STEP 0.0005
#define NUM_SAMPLING_STEPS 4
#define TANGENT_BIAS 0.05

void main()
{
    float start_Z = texture( uSamplerDepth, vertex.uv ).r; // returns value (z/w+1)/2
    vec3 start_Pos = vec3( vertex.uv, start_Z);
    vec3 ndc_Pos = (2.0 * start_Pos) - 1.0; // transform to normalized device coordinates xyz/w

    // reconstruct view space position
    vec3 viewPos = decodePosition( vertex.uv ).xyz; // 3d view space position P
    vec3 viewNorm = decodeNormal( texture( uSamplerNormal, vertex.uv ).xy ); // 3d view space normal N
    float total = 0.0;
    float sample_direction_increment = 2 * M_PI / NUM_SAMPLING_DIRECTIONS;
    for ( int i = 0; i < NUM_SAMPLING_DIRECTIONS; i++) {
        // no jittering or randomization of sampling direction just yet
        float sampling_angle = i * sample_direction_increment; // azimuth angle theta in the paper
        vec2 sampleDir = vec2(cos(sampling_angle), sin(sampling_angle));
        // we will now march along sampleDir and calculate the horizon
        // horizon starts with the tangent plane to the surface, whose angle we can get from the normal
        float tangentAngle = acos(dot(vec3(sampleDir, 0), viewNorm)) - (0.5 * M_PI) + TANGENT_BIAS;
        float horizonAngle = tangentAngle;
        vec3 lastDiff = vec3(0);
        for (int j = 0; j < NUM_SAMPLING_STEPS; j++) {
            // march along the sampling direction and see what the horizon is
            vec2 sampleOffset = float(j+1) * SAMPLING_STEP * sampleDir;
            vec2 offTex = vertex.uv + sampleOffset;

            vec3 off_viewPos = decodePosition( offTex.st ).xyz;
            // we now have the view space position of the offset point
            vec3 diff = off_viewPos.xyz - viewPos.xyz;
            if (length(diff) < SAMPLING_RADIUS) {
                // skip samples which are outside of our local sampling radius
                lastDiff = diff;
                float elevationAngle = atan(diff.z / length(diff.xy));
                horizonAngle = max(horizonAngle, elevationAngle);
            }
        }
        // the paper uses this attenuation but I like the other way better
        //float normDiff = length(lastDiff) / SAMPLING_RADIUS;
        //float attenuation = 1 - normDiff*normDiff;
        float attenuation = 1.0 / (1 + length(lastDiff));
        // now compare horizon angle to tangent angle to get ambient occlusion
        float occlusion = clamp(attenuation * (sin(horizonAngle) - sin(tangentAngle)), 0.0, 1.0);
        total += 1.0 - occlusion;
    }
    total /= NUM_SAMPLING_DIRECTIONS;

    oColor = vec4(total, total, total, 1.0);
}
 