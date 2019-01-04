
in vec3 VertexPosition;
in float VertexStartTime;
in vec4 VertexColor;

out float Transp; // To Fragment Shader

uniform float MinParticleSize;
uniform float MaxParticleSize;

uniform float Time; 
uniform float ParticleLifetime;

uniform mat4 ciModelViewProjection;

void main() {
	float age = Time - VertexStartTime;
	Transp = 0.0;
	gl_Position = ciModelViewProjection * vec4(VertexPosition, 1.0);
	if( Time >= VertexStartTime ) {
		float agePct = age / ParticleLifetime;
		Transp = 1.0 - agePct;
		gl_PointSize = mix( MinParticleSize, MaxParticleSize, agePct );
	}
}