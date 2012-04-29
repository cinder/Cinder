//
// Stereographic blending for GL_TEXTURE_2D
//
// Studio Avante / Roger Sodre
//

uniform sampler2D	u_texLeft;
uniform sampler2D	u_texRight;
uniform float		u_delta;

void main()
{
	float d = (u_delta / (1. + u_delta));
	vec2 st = gl_TexCoord[0].st;
	vec2 st_left  = vec2( mix(d, 1., st.x), st.t );
	vec2 st_right = vec2( mix(0., 1.-d, st.x), st.t );
	vec4 colorLeft	= texture2D(u_texLeft, st_left );
	vec4 colorRight = texture2D(u_texRight, st_right );
	vec4 colorMix	= vec4( colorLeft.r, colorRight.g, colorRight.b, colorLeft.a+colorRight.a );
	gl_FragColor = colorMix;
}
