#version 150

uniform mat4	ciModelViewProjection;
uniform mat3	ciNormalMatrix;
uniform float	uNormalsLength;

in vec4		ciPosition;
in vec2		ciTexCoord0;
in float	vNormalWeight;
in vec4		ciColor;

out lowp vec4	Color;

uniform float uA1, uA2;
uniform float uB1, uB2;
uniform float uM1, uM2;
uniform float uN11, uN12;
uniform float uN21, uN22;
uniform float uN31, uN32;

float superformula( float o, float a, float b, float m, float n1, float n2, float n3 )
{
	return pow(pow(abs(cos(m * o / 4)) / a, n2) + pow(abs(sin(m * o / 4)) / b, n3), 1 / n1);
}

float superformulaDeriv( float o, float a, float b, float m, float n1, float n2, float n3 )
{
	float f = n1;
	float g = n2;
	float h = n3;
	
	float subExpA = h*m/4*pow(1/b,h)*sin(m*o/4)*cos(m*o/4)*pow(abs(sin(m*o/4)),h-2);
	float subExpB = g*m/4*pow(1/a,g)*sin(m*o/4)*cos(m*o/4)*pow(abs(cos(m*o/4)),g-2);
	
	return (1/f) * (subExpA - subExpB )
			*
			pow( pow(1/a,g)*pow(abs(cos(m*o/4)),g) + pow(1/b,h) * pow(abs(sin(m*o/4)),h),   1/f-1 );
}

vec3 superformulaNormal( float t, float p )
{
	vec3 derivT, derivP;

	float rTheta = superformula( t, uA1, uB1, uM1, uN11, uN21, uN31 );
	float rPhi = superformula( p, uA2, uB2, uM2, uN12, uN22, uN32 );
	float rThetaPrime = superformulaDeriv( t, uA1, uB1, uM1, uN11, uN21, uN31 );
	float rPhiPrime = superformulaDeriv( p, uA2, uB2, uM2, uN12, uN22, uN32 );

	// deriv t
	derivT.x = -( cos(p) * ( cos(t) * rThetaPrime + rTheta * sin(t) ) ) / ( rPhi * rTheta * rTheta );
	derivT.y = ( cos(p) * ( rTheta * cos(t) - sin(t) * rThetaPrime ) ) / ( rPhi * rTheta * rTheta );
	derivT.z = 0.0;
	// deriv p
	derivP.x = -( cos(t) * ( cos(p) * rPhiPrime + rPhi * sin(p) ) ) / ( rPhi * rPhi * rTheta );
	derivP.y = -( sin(t) * ( cos(p) * rPhiPrime + rPhi * sin(p) ) ) / ( rPhi * rPhi * rTheta );
	derivP.z = ( rPhi * cos(p) - sin(p) * rPhiPrime ) / ( rPhi * rPhi );
	
	return cross( normalize(derivT), normalize(derivP) );
}

void main( void )
{
	float p = -3.14159 / 2.0 + ciTexCoord0.s * 3.14159;
	float t = -3.14159 + ciTexCoord0.t * 3.14159 * 2.0;
	float r1 = superformula( t, uA1, uB1, uM1, uN11, uN21, uN31 );
	float r2 = superformula( p, uA2, uB2, uM2, uN12, uN22, uN32 );
	float x = cos(t) * cos(p) / r1 / r2;
	float y = sin(t) * cos(p) / r1 / r2;
	float z = sin(p) / r2;

	vec3 position = vec3( x, y, z );
	vec3 normal = normalize( superformulaNormal( t, p ) ) * uNormalsLength;

	gl_Position	= ciModelViewProjection * vec4( position + normal * vNormalWeight, 1.0 );
	Color 		= ciColor;
}
