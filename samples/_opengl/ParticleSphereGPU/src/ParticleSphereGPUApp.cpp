//
//	Copyright (c) 2014 David Wicks, sansumbrella.com
//	All rights reserved.
//
//	Particle Sphere sample application, GPU integration.
//
//	Author: David Wicks
//	License: BSD Simplified
//

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

/**
 Particle type holds information for rendering and simulation.
 Used to buffer initial simulation values.
 */
struct Particle
{
	vec3	pos;
	vec3	ppos;
	vec3	home;
	ColorA  color;
	float	damping;
};

// How many particles to create. (600k default)
#if defined( CINDER_GL_ES ) // ES devices can't handle as many particles as the desktop
const int NUM_PARTICLES = 600e2;
#else
const int NUM_PARTICLES = 600e3;
#endif

/**
	Simple particle simulation with Verlet integration and mouse interaction.
	A sphere of particles is deformed by mouse interaction.
	Simulation is run using transform feedback on the GPU.
	particleUpdate.vs defines the simulation update step.
	Designed to have the same behavior as ParticleSphereCPU.
 */
class ParticleSphereGPUApp : public App {
  public:
	void setup() override;
	void update() override;
	void draw() override;
  private:
	gl::GlslProgRef mRenderProg;
	gl::GlslProgRef mUpdateProg;

	// Descriptions of particle data layout.
	gl::VaoRef		mAttributes[2];
	// Buffers holding raw particle data on GPU.
	gl::VboRef		mParticleBuffer[2];

	// Current source and destination buffers for transform feedback.
	// Source and destination are swapped each frame after update.
	std::uint32_t	mSourceIndex		= 0;
	std::uint32_t	mDestinationIndex	= 1;

	// Mouse state suitable for passing as uniforms to update program
	bool			mMouseDown = false;
	float			mMouseForce = 0.0f;
	vec3			mMousePos = vec3( 0, 0, 0 );
};

void ParticleSphereGPUApp::setup()
{
	// Create initial particle layout.
	vector<Particle> particles;
	particles.assign( NUM_PARTICLES, Particle() );
	const float azimuth = 256.0f * M_PI / particles.size();
	const float inclination = M_PI / particles.size();
	const float radius = 180.0f;
	vec3 center = vec3( getWindowCenter() + vec2( 0.0f, 40.0f ), 0.0f );
	for( int i = 0; i < particles.size(); ++i )
	{	// assign starting values to particles.
		float x = radius * sin( inclination * i ) * cos( azimuth * i );
		float y = radius * cos( inclination * i );
		float z = radius * sin( inclination * i ) * sin( azimuth * i );
		
		auto &p = particles.at( i );
		p.pos = center + vec3( x, y, z );
		p.home = p.pos;
		p.ppos = p.home + Rand::randVec3() * 10.0f; // random initial velocity
		p.damping = Rand::randFloat( 0.965f, 0.985f );
		p.color = Color( CM_HSV, lmap<float>( i, 0.0f, particles.size(), 0.0f, 0.66f ), 1.0f, 1.0f );
	}
	
	// Create particle buffers on GPU and copy data into the first buffer.
	// Mark as static since we only write from the CPU once.
	mParticleBuffer[mSourceIndex] = gl::Vbo::create( GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), particles.data(), GL_STATIC_DRAW );
	mParticleBuffer[mDestinationIndex] = gl::Vbo::create( GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), nullptr, GL_STATIC_DRAW );

	for( int i = 0; i < 2; ++i )
	{	// Describe the particle layout for OpenGL.
		mAttributes[i] = gl::Vao::create();
		gl::ScopedVao vao( mAttributes[i] );
		
		// Define attributes as offsets into the bound particle buffer
		gl::ScopedBuffer buffer( mParticleBuffer[i] );
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
	}
	
	// Load our update program.
	// Match up our attribute locations with the description we gave.
	
#if defined( CINDER_GL_ES_3 )
	mRenderProg = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "draw_es3.vert" ) )
									   .fragment( loadAsset( "draw_es3.frag" ) ) );
	mUpdateProg = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "particleUpdate_es3.vs" ) )
									   .fragment( loadAsset( "no_op_es3.fs" ) )
#else
	mRenderProg = gl::getStockShader( gl::ShaderDef().color() );
	mUpdateProg = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "particleUpdate.vs" ) )
#endif
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
												  mMouseForce = 500.0f;
												  mMousePos = vec3( event.getX(), event.getY(), 0.0f );
											  } );
	getWindow()->getSignalMouseDrag().connect( [this]( MouseEvent event )
											  {
												  mMousePos = vec3( event.getX(), event.getY(), 0.0f );
											  } );
	getWindow()->getSignalMouseUp().connect( [this]( MouseEvent event )
											{
												mMouseForce = 0.0f;
												mMouseDown = false;
											} );
}

void ParticleSphereGPUApp::update()
{
	// Update particles on the GPU
	gl::ScopedGlslProg prog( mUpdateProg );
	gl::ScopedState rasterizer( GL_RASTERIZER_DISCARD, true );	// turn off fragment stage
	mUpdateProg->uniform( "uMouseForce", mMouseForce );
	mUpdateProg->uniform( "uMousePos", mMousePos );

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

	// Update mouse force.
	if( mMouseDown ) {
		mMouseForce = 150.0f;
	}
}

void ParticleSphereGPUApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatricesWindowPersp( getWindowSize(), 60.0f, 1.0f, 10000.0f );
	gl::enableDepthRead();
	gl::enableDepthWrite();

	gl::ScopedGlslProg render( mRenderProg );
	gl::ScopedVao vao( mAttributes[mSourceIndex] );
	gl::context()->setDefaultShaderVars();
	gl::drawArrays( GL_POINTS, 0, NUM_PARTICLES );
}

CINDER_APP( ParticleSphereGPUApp, RendererGl, [] ( App::Settings *settings ) {
	settings->setWindowSize( 1280, 720 );
	settings->setMultiTouchEnabled( false );
} )
