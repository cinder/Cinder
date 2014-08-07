#version 150 core
#extension all : warn

// This input vector contains the vertex position in xyz, and the
// mass of the vertex in w
in vec4 position_mass;
// This is the current velocity of the vertex
in vec3 velocity;
// This is our connection vector
in ivec4 connection;

// This is a TBO that will be bound to the same buffer as the
// position_mass input attribute
uniform samplerBuffer tex_position;

uniform vec2 mouse_pos;

// The outputs of the vertex shader are the same as the inputs
out vec4 tf_position_mass;
out vec3 tf_velocity;

// A uniform to hold the timestep. The application can update this
uniform float t = 0.07;

// The global spring constant
uniform float k = 10.0; 

// Gravity
const vec3 gravity = vec3(0.0, -0.15, 0.0);

// Global damping constant
uniform float c = 2.8;

// Spring resting length
uniform float rest_length = 0.88;

void main(void)
{
	vec3 p = position_mass.xyz;		// p can be our position
	if (mouse_pos.x > p.x - 1 && mouse_pos.x < p.x + 1 && mouse_pos.y > p.y - 1 && mouse_pos.y < p.y + 1 ) {
		p = vec3(mouse_pos, 0);
	}
	float m = position_mass.w;		// m is the mass of our vertex
	vec3 u = velocity;				// u is the initial velocity
	vec3 F = gravity * m - c * u;	// F is the force on the mass
	bool fixed_node = true;			// Becomes false when force is applied
	
	for ( int i = 0; i < 4; ++i ) {
		if(connection[i] != -1) {
			// q is the position of the other vertex
			vec3 q = texelFetch(tex_position, connection[i]).xyz;
			vec3 d = q - p;
			float x = length(d);
			F += -k * (rest_length - x) * normalize(d);
			fixed_node = false;
		}
	}
	
	// If this is a fixed node, reset force to zero
	if(fixed_node) {
		F = vec3(0.0);
	}
	
	// Accelleration due to force
	vec3 a = F / m;
	
	// Displacement
	vec3 s = u * t + 0.5 * a * t * t;
	
	// Final velocity
	vec3 v = u + a * t;
	
	// Constrain the absolute value of the displacement per step
	s = clamp( s, vec3(-25.0), vec3(25.0) );
	
	// Write the outputs
	tf_position_mass = vec4(p + s, m);
	tf_velocity = v;
}
