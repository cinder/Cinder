uniform vec3 BaseColor;
uniform float MixRatio;

uniform samplerCube	EnvMap;

varying vec3	ReflectDir;
varying float	LightIntensity;

void main()
{
	vec3 envColor	= textureCube( EnvMap, ReflectDir ).rgb;
	
	vec3 base	= LightIntensity * BaseColor;
	envColor	= mix( envColor, base, MixRatio );
	
	gl_FragColor.rgb = envColor;
}