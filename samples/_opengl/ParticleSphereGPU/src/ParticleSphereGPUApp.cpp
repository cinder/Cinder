#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"

#include "cinder/Rand.h"

#include "cinder/gl/gl.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Vao.h"
#include "cinder/gl/GlslProg.h"

using namespace ci;
using namespace ci::app;
using namespace std;

/**
 Particle:

 A simple type holding information for rendering and simulation.
 Used to buffer initial simulation values.
 */
struct Particle
{
	vec3	pos;
	vec3	ppos;
	vec3	home;
	ColorA  color;	// Q: how to specify type of default attributes to Cinder? e.g. switch to Color8u
	float	damping;
};

// Home many particles to create. (600k)
const int NUM_PARTICLES = 600e3;

/**
	Simple particle simulation with Verlet integration and mouse interaction.
	A sphere of particles is deformed by mouse interaction.
	Simulation is run using transform feedback on the GPU.
	particleUpdate.vs defines the simulation update step.
	Designed to have the same behavior as ParticleSphereCPU.
 */
class ParticleSphereGPUApp : public AppNative {
public:
	void prepareSettings( Settings *settings );
	void setup();
	void update();
	void draw();
private:
	// program for drawing the particles
	gl::GlslProgRef	mRenderProg;
	// program for updating the particle simulation
	gl::GlslProgRef mUpdateProg;
	// descriptions of particle data layout
	gl::VaoRef		mAttributes[2];
	// buffers holding raw particle data on GPU
	gl::VboRef		mParticleBuffer[2];
	// current source and destination buffers for transform feedback
	// source and destination are swapped each frame after update
	uint32_t		mSourceIndex		= 0;
	uint32_t		mDestinationIndex	= 1;

	// mouse state suitable for passing as uniforms to update program
	bool			mMouseDown = false;
	vec3			mMousePos = vec3( 0, 0, 0 );
};

void ParticleSphereGPUApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 1280, 720 );
}

void ParticleSphereGPUApp::setup()
{
	// Create initial particle layout.
	vector<Particle> particles;
	particles.assign( NUM_PARTICLES, Particle() );
	const float azimuth = 128.0f * M_PI / particles.size();
	const float inclination = M_PI / particles.size();
	const float radius = 160.0f;
	vec3 center = vec3( getWindowCenter(), 0.0f );
	for( int i = 0; i < particles.size(); ++i )
	{	// assign starting values to particles.
		float x = radius * sin( inclination * i ) * cos( azimuth * i );
		float y = radius * cos( inclination * i );
		float z = radius * sin( inclination * i ) * sin( azimuth * i );

		auto &p = particles.at( i );
		p.pos = center + vec3( x, y, z );
		p.home = p.pos;
		p.ppos = p.home + Rand::randVec3f() * 10.0f; // random initial velocity
		p.damping = Rand::randFloat( 0.965f, 0.985f );
		p.color = Color( CM_HSV, lmap<float>( i, 0.0f, particles.size(), 0.0f, 0.66f ), 1.0f, 1.0f );
	}

	// Create particle buffers on GPU and copy data into the first buffer.
	// Mark as static since we only write from the CPU once.
	mParticleBuffer[mSourceIndex] = gl::Vbo::create( GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), particles.data(), GL_STATIC_DRAW );
	mParticleBuffer[mDestinationIndex] = gl::Vbo::create( GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), nullptr, GL_STATIC_DRAW );

	// Create a default color shader.
	mRenderProg = gl::getStockShader( gl::ShaderDef().color() );

	for( int i = 0; i < 2; ++i )
	{	// Describe the particle layout for OpenGL.
		mAttributes[i] = gl::Vao::create();
		mAttributes[i]->bind();

		// Define attributes as offsets into the bound particle buffer
		mParticleBuffer[i]->bind();
		gl::enableVertexAttribArray( 0 );
		gl::enableVertexAttribArray( 1 );
		gl::enableVertexAttribArray( 2 );
		gl::enableVertexAttribArray( 3 );
		gl::enableVertexAttribArray( 4 );
		gl::vertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, pos) );
		gl::vertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, color) );
		gl::vertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, ppos) );
		gl::vertexAttribPointer( 3, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, home) );
		gl::vertexAttribPointer( 4, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, damping) );
		mAttributes[i]->unbind();
	}

	// Load our update program.
	// Match up our attribute locations with the description we gave.
	mUpdateProg = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "particleUpdate.vs" ) )
		.feedbackFormat( GL_INTERLEAVED_ATTRIBS )
		.feedbackVaryings( { "position", "pposition", "home", "color", "damping" } )
		.attribLocation( "iPosition", 0 )
		.attribLocation( "iColor", 1 )
		.attribLocation( "iPPosition", 2 )
		.attribLocation( "iHome", 3 )
		.attribLocation( "iDamping", 4 )
									   );

	// Listen to mouse events so we can send data as uniforms.
	getWindow()->getSignalMouseDown().connect( [this]( MouseEvent event )
											  {
												  mMouseDown = true;
												  mMousePos = vec3( event.getX(), event.getY(), 0.0f );
											  } );
	getWindow()->getSignalMouseDrag().connect( [this]( MouseEvent event )
											  {
												  mMouseDown = true;
												  mMousePos = vec3( event.getX(), event.getY(), 0.0f );
											  } );
	getWindow()->getSignalMouseUp().connect( [this]( MouseEvent event )
											{
												mMouseDown = false;
											} );
}

void ParticleSphereGPUApp::update()
{
	// Update particles on the GPU
	gl::ScopedGlslProg prog( mUpdateProg );
	gl::ScopedState rasterizer( GL_RASTERIZER_DISCARD, true );	// turn off fragment stage
	mUpdateProg->uniform( "mouseDown", mMouseDown );
	mUpdateProg->uniform( "mousePos", mMousePos );

	// Bind the source data (Attributes refer to specific buffers).
	gl::ScopedVao source( mAttributes[mSourceIndex] );
	// Bind destination as buffer base.
	gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 0, mParticleBuffer[mDestinationIndex] );
	gl::beginTransformFeedback( GL_POINTS );

	// Draw source into destination, performing our vertex transformations.
	gl::drawArrays( GL_POINTS, 0, NUM_PARTICLES );

	gl::endTransformFeedback();

	// Swap source and destination for next loop
	std::swap( mSourceIndex, mDestinationIndex );
}

void ParticleSphereGPUApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatricesWindowPersp( getWindowSize() );

	gl::ScopedGlslProg render( mRenderProg );
	gl::ScopedVao vao( mAttributes[mSourceIndex] );
	gl::context()->setDefaultShaderVars();
	gl::drawArrays( GL_POINTS, 0, NUM_PARTICLES );
}

CINDER_APP_NATIVE( ParticleSphereGPUApp, RendererGl )
