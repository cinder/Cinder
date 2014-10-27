#version 300 es

uniform mat4	ciModelViewProjection;
uniform mat3	ciNormalMatrix;

in vec2		ciTexCoord0;
in vec3		ciNormal;
in vec4		ciColor;

out highp vec2	TexCoord;
out lowp vec4	Color;
out highp vec3	Normal;

layout (std140) uniform FormulaParams {
	float	uA1, uA2;
	float	uB1, uB2;
	float	uM1, uM2;
	float	uN11, uN12;
	float	uN21, uN22;
	float	uN31, uN32;
};

float superformula( float o, float a, float b, float m, float n1, float n2, float n3 )
{
	return pow(pow(abs(cos(m * o / 4.0)) / a, n2) + pow(abs(sin(m * o / 4.0)) / b, n3), 1.0 / n1);
}

float superformulaDeriv( float o, float a, float b, float m, float n1, float n2, float n3 )
{
	highp float subExpA = n3*m/4.0*pow(1.0/b,n3)*sin(m*o/4.0)*cos(m*o/4.0)*pow(abs(sin(m*o/4.0)),n3-2.0);
	highp float subExpB = n2*m/4.0*pow(1.0/a,n2)*sin(m*o/4.0)*cos(m*o/4.0)*pow(abs(cos(m*o/4.0)),n2-2.0);
	highp float subExpC = pow(1.0/a,n2)*pow(abs(cos(m*o/4.0)),n2) + pow(1.0/b,n3) * pow(abs(sin(m*o/4.0)),n3);
	
	return (1.0f / n1) * (subExpA - subExpB ) * pow( subExpC, 1.0 / n1 - 1.0 );
}

vec3 superformulaNormal( float theta, float phi, float rTheta, float rPhi )
{
	highp vec3 derivT, derivP;

	highp float rThetaPrime = superformulaDeriv( theta, uA1, uB1, uM1, uN11, uN21, uN31 );
	highp float rPhiPrime = superformulaDeriv( phi, uA2, uB2, uM2, uN12, uN22, uN32 );

	// deriv theta
	derivT.x = -( cos(phi) * ( cos(theta) * rThetaPrime + rTheta * sin(theta) ) ) / ( rPhi * rTheta * rTheta );
	derivT.y = ( cos(phi) * ( rTheta * cos(theta) - sin(theta) * rThetaPrime ) ) / ( rPhi * rTheta * rTheta );
	derivT.z = 0.0;
	// deriv phi
	derivP.x = -( cos(theta) * ( cos(phi) * rPhiPrime + rPhi * sin(phi) ) ) / ( rPhi * rPhi * rTheta );
	derivP.y = -( sin(theta) * ( cos(phi) * rPhiPrime + rPhi * sin(phi) ) ) / ( rPhi * rPhi * rTheta );
	derivP.z = ( rPhi * cos(phi) - sin(phi) * rPhiPrime ) / ( rPhi * rPhi );
	
	return cross( normalize(derivT), normalize(derivP) );
}

void main( void )
{
	highp float theta = mix( -3.1415926535, 3.1415926535, ciTexCoord0.s ); // theta domain = [-pi,pi]
	highp float phi = mix( -3.1415926535 / 2.0, 3.1415926535 / 2.0, ciTexCoord0.t ); // phi domain = [-pi/2,pi/2]
	highp float rTheta = superformula( theta, uA1, uB1, uM1, uN11, uN21, uN31 );
	highp float rPhi = superformula( phi, uA2, uB2, uM2, uN12, uN22, uN32 );
	highp float x = cos(theta) * cos(phi) / rTheta / rPhi;
	highp float y = sin(theta) * cos(phi) / rTheta / rPhi;
	highp float z = sin(phi) / rPhi;

	highp vec3 position = vec3( x, y, z );
	highp vec3 normal = superformulaNormal( theta, phi, rTheta, rPhi );

	gl_Position	= ciModelViewProjection * vec4( position, 1.0 );
	Normal		= ciNormalMatrix * normal;
	Color 		= ciColor;
	TexCoord	= ciTexCoord0;
}
