//
//	Copyright (c) 2014 David Wicks, sansumbrella.com
//	All rights reserved.
//
//	Particle Sphere sample application, CPU integration.
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
 */
struct Particle
{
	vec3 		home;
	vec3 		ppos;
	vec3		pos;
	ColorA		color;
	float		damping;
};

// Home many particles to create. (200k)
const int NUM_PARTICLES = 200e3;

/**
	Simple particle simulation with Verlet integration and mouse interaction.
	A sphere of particles is deformed by mouse interaction.
	Simulation is run on the CPU in the update() function.
	Designed to have the same behavior as ParticleSphereGPU.
 */

class ParticleSphereCPUApp : public App {
  public:
	void setup() override;
	void update() override;
	void draw() override;

	// Push particles away from a point.
	void disturbParticles( const vec3 &center, float force );
  private:
	// Particle data on CPU.
	vector<Particle>	mParticles;
	// Buffer holding raw particle data on GPU, written to every update().
	gl::VboRef			mParticleVbo;
	// Batch for rendering particles with default shader.
	gl::BatchRef		mParticleBatch;
};

void ParticleSphereCPUApp::setup()
{
	// Create initial particle layout.
	mParticles.assign( NUM_PARTICLES, Particle() );
	const float azimuth = 128.0f * M_PI / mParticles.size();
	const float inclination = M_PI / mParticles.size();
	const float radius = 160.0f;
	vec3 center = vec3( getWindowCenter(), 0.0f );
	for( int i = 0; i < mParticles.size(); ++i )
	{	// assign starting values to particles.
		float x = radius * sin( inclination * i ) * cos( azimuth * i );
		float y = radius * cos( inclination * i );
		float z = radius * sin( inclination * i ) * sin( azimuth * i );

		auto &p = mParticles.at( i );
		p.pos = center + vec3( x, y, z );
		p.home = p.pos;
		p.ppos = p.home + Rand::randVec3() * 10.0f; // random initial velocity
		p.damping = Rand::randFloat( 0.965f, 0.985f );
		p.color = Color( CM_HSV, lmap<float>( i, 0.0f, mParticles.size(), 0.0f, 0.66f ), 1.0f, 1.0f );
	}

	// Create particle buffer on GPU and copy over data.
	// Mark as streaming, since we will copy new data every frame.
	mParticleVbo = gl::Vbo::create( GL_ARRAY_BUFFER, mParticles, GL_STREAM_DRAW );

	// Describe particle semantics for GPU.
	geom::BufferLayout particleLayout;
	particleLayout.append( geom::Attrib::POSITION, 3, sizeof( Particle ), offsetof( Particle, pos ) );
	particleLayout.append( geom::Attrib::COLOR, 4, sizeof( Particle ), offsetof( Particle, color ) );

	// Create mesh by pairing our particle layout with our particle Vbo.
	// A VboMesh is an array of layout + vbo pairs
	auto mesh = gl::VboMesh::create( mParticles.size(), GL_POINTS, { { particleLayout, mParticleVbo } } );
#if ! defined( CINDER_GL_ES )
	mParticleBatch = gl::Batch::create( mesh, gl::getStockShader( gl::ShaderDef().color() ) );
	gl::pointSize( 1.0f );
#else
	mParticleBatch = gl::Batch::create( mesh, gl::GlslProg::create( loadAsset( "draw_es3.vert" ), 
																		loadAsset( "draw_es3.frag" ) ) );
#endif

	// Disturb particles a lot on mouse down.
	getWindow()->getSignalMouseDown().connect( [this]( MouseEvent event ) {
		vec3 mouse( event.getPos(), 0.0f );
		disturbParticles( mouse, 500.0f );
	} );

	// Disturb particle a little on mouse drag.
	getWindow()->getSignalMouseDrag().connect( [this]( MouseEvent event ) {
		vec3 mouse( event.getPos(), 0.0f );
		disturbParticles( mouse, 120.0f );
	} );
}

void ParticleSphereCPUApp::disturbParticles( const vec3 &center, float force )
{
	for( auto &p : mParticles ) {
		vec3 dir = p.pos - center;
		float d2 = length2( dir );
		p.pos += force * dir / d2;
	}
}

void ParticleSphereCPUApp::update()
{
	// Run Verlet integration on all particles on the CPU.
	float dt2 = 1.0f / (60.0f * 60.0f);
	for( auto &p : mParticles ) {
		vec3 vel = (p.pos - p.ppos) * p.damping;
		p.ppos = p.pos;
		vec3 acc = (p.home - p.pos) * 32.0f;
		p.pos += vel + acc * dt2;
	}

	// Copy particle data onto the GPU.
	// Map the GPU memory and write over it.
	void *gpuMem = mParticleVbo->mapReplace();
	memcpy( gpuMem, mParticles.data(), mParticles.size() * sizeof(Particle) );
	mParticleVbo->unmap();
}

void ParticleSphereCPUApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatricesWindowPersp( getWindowSize(), 60.0f, 1.0f, 10000.0f );
	gl::enableDepthRead();
	gl::enableDepthWrite();

	mParticleBatch->draw();
}

CINDER_APP( ParticleSphereCPUApp, RendererGl, [] ( App::Settings *settings ) {
	settings->setWindowSize( 1280, 720 );
	settings->setMultiTouchEnabled( false );
} )
