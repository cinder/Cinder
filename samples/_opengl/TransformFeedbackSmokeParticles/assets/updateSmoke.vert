

in vec3 VertexPosition;
in vec3 VertexVelocity;
in float VertexStartTime;
in vec3 VertexInitialVelocity;
in vec4 VertexColor;

out vec3 Position; // To Transform Feedback
out vec3 Velocity; // To Transform Feedback
out vec4 Color; // To Transform Feedback
out float StartTime; // To Transform Feedback


uniform float Time; // Time
uniform float H;	// Elapsed time between frames
uniform vec3 Accel; // Particle Acceleration
uniform float ParticleLifetime; // Particle lifespan

void main() {
	
	// Update position & velocity for next frame
	Position = VertexPosition;
	Velocity = VertexVelocity;
	StartTime = VertexStartTime;
	
	if( Time >= StartTime ) {
		
		float age = Time - StartTime;
		
		if( age > ParticleLifetime ) {
			// The particle is past it's lifetime, recycle.
			Position = vec3(0.0);
			Velocity = VertexInitialVelocity;
			StartTime = Time;
		}
		else {
			// The particle is alive, update.
			Position += Velocity * H;
			Velocity += Accel * H;
		}
	}
}