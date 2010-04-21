#version 110

uniform sampler2D tex0;
uniform vec2 sampleOffset;

float weights[21];

void main()
{
	weights[0] = 0.0091679276560113852;
	weights[1] = 0.014053461291849008;
	weights[2] = 0.020595286319257878;
	weights[3] = 0.028855245532226279;
	weights[4] = 0.038650411513543079;
	weights[5] = 0.049494378859311142;
	weights[6] = 0.060594058578763078;
	weights[7] = 0.070921288047096992;
	weights[8] = 0.079358891804948081;
	weights[9] = 0.084895951965930902;
	weights[10] = 0.086826196862124602;
	weights[11] = 0.084895951965930902;
	weights[12] = 0.079358891804948081;
	weights[13] = 0.070921288047096992;
	weights[14] = 0.060594058578763092;
	weights[15] = 0.049494378859311121;
	weights[16] = 0.0386504115135431;
	weights[17] = 0.028855245532226279;
	weights[18] = 0.020595286319257885;
	weights[19] = 0.014053461291849008;
	weights[20] = 0.00916792765601138;


	vec3 sum = vec3( 0.0, 0.0, 0.0 );
	vec2 baseOffset = -10.0 * sampleOffset;
	vec2 offset = vec2( 0.0, 0.0 );
	for( int s = 0; s < 21; ++s ) {
		sum += texture2D( tex0, gl_TexCoord[0].st + baseOffset + offset ).rgb * weights[s];
		offset += sampleOffset;
	}
	gl_FragColor.rgb = sum;
	gl_FragColor.a = 1.0;
}