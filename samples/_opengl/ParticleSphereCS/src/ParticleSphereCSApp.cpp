//
//	Copyright (c) 2014 David Wicks, sansumbrella.com
//	All rights reserved.
//
//	Particle Sphere sample application, GPU integration.
//
//	Author: David Wicks
//	License: BSD Simplified
//

#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"

#include "cinder/Rand.h"

#include "cinder/gl/gl.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Vao.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/BufferObj.h"
#include "cinder/Utilities.h"
#include "cinder/CinderAssert.h"
#include "Ssbo.h"
#include "ScopedBufferBase.h"


using namespace ci;
using namespace ci::app;
using namespace std;


// Particle type holds information for rendering and simulation.
// Used to buffer initial simulation values.
// Using std140 in ssbo requires we have members on 4 byte alignment. 
#pragma pack( push, 1 )
struct Particle
{
	vec3	pos;
	float   pad1;
	vec3	ppos;
	float   pad2;
	vec3	home;
	float   pad3;
	vec4    color;
	float	damping;
	vec3    pad4;
};
#pragma pack( pop )

// Home many particles to create. (600k default)
const int NUM_PARTICLES = static_cast<int>( 600e3 );

/**
	Simple particle simulation with Verlet integration and mouse interaction.
	A sphere of particles is deformed by mouse interaction.
	Simulation is run using transform feedback on the GPU.
	particleUpdate.vs defines the simulation update step.
	Designed to have the same behavior as ParticleSphereCPU.
 */
class ParticleSphereCSApp : public AppNative {
  public:
	void prepareSettings( Settings *settings ) override;
	void setup() override;
	void update() override;
	void draw() override;

  private:
	ivec3 getMaxComputeWorkGroupCount();
	ivec3 getMaxComputeWorkGroupSize();

	enum { WORK_GROUP_SIZE = 128, };
	typedef  SsboT<Particle> ParticleSsbo;
	typedef ParticleSsbo::Ref ParticleSsboRef;
	gl::GlslProgRef mRenderProg;
	gl::GlslProgRef mUpdateProg;

	// Buffers holding raw particle data on GPU.
	ParticleSsboRef mParticleBuffer;
	gl::VboRef mIdsVbo;
	gl::VaoRef mAttributes;

	// Mouse state suitable for passing as uniforms to update program
	bool			mMouseDown = false;
	float			mMouseForce = 0.0f;
	vec3			mMousePos = vec3( 0 );
};

void ParticleSphereCSApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 1280, 720 );
}

void ParticleSphereCSApp::setup()
{
	// Create initial particle layout.
	vector<Particle> particles;
	particles.assign( NUM_PARTICLES, Particle() );
	const float azimuth = 256.0f * static_cast<float>( M_PI ) / particles.size();
	const float inclination = static_cast<float>( M_PI ) / particles.size();
	const float radius = 180.0f;
	vec3 center = vec3( getWindowCenter() + vec2( 0.0f, 40.0f ), 0.0f );
	for( unsigned int i = 0; i < particles.size(); ++i )
	{	// assign starting values to particles.
		float x = radius * math<float>::sin( inclination * i ) * math<float>::cos( azimuth * i );
		float y = radius * math<float>::cos( inclination * i );
		float z = radius * math<float>::sin( inclination * i ) * math<float>::sin( azimuth * i );

		auto &p = particles.at( i );
		p.pos = center + vec3( x, y, z );
		p.home = p.pos;
		p.ppos = p.home + Rand::randVec3f() * 10.0f; // random initial velocity
		p.damping = Rand::randFloat( 0.965f, 0.985f );
		Color c( CM_HSV, lmap<float>( static_cast<float>(i), 0.0f, static_cast<float>( particles.size() ), 0.0f, 0.66f ), 1.0f, 1.0f );
		p.color = vec4( c.r, c.g, c.b, 1.0f );
	}

	ivec3 count = getMaxComputeWorkGroupCount();
	CI_ASSERT( count.x >= ( NUM_PARTICLES / WORK_GROUP_SIZE ) );

	// Create particle buffers on GPU and copy data into the first buffer.
	// Mark as static since we only write from the CPU once.
	mParticleBuffer = ParticleSsbo::create( particles, GL_STATIC_DRAW );
	gl::ScopedBuffer scopedParticleSsbo( mParticleBuffer );
	mParticleBuffer->bindBase( 2 );
	
	// Create a default color shader.
	try {
		mRenderProg = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "particleRender.vs.glsl" ) )
			.fragment( loadAsset( "particleRender.fs.glsl" ) )
			.attribLocation( "particleId", 0 ) );
	}
	catch( gl::GlslProgCompileExc e ) {
		ci::app::console() << e.what() << std::endl;
		shutdown();
	}

	std::vector<GLuint> ids( NUM_PARTICLES );
	GLuint currId = 0;
	std::generate( ids.begin(), ids.end(), [&currId]() -> GLuint { return currId++; } );
	
	mIdsVbo = gl::Vbo::create<GLuint>( GL_ARRAY_BUFFER, ids, GL_STATIC_DRAW );
    mAttributes = gl::Vao::create();
	gl::ScopedVao vao( mAttributes );
	gl::ScopedBuffer scopedIds( mIdsVbo );
	gl::enableVertexAttribArray( 0 );
	gl::vertexAttribIPointer( 0, 1, GL_UNSIGNED_INT, sizeof( GLuint ), 0 );
	
	try {
		//// Load our update program.
		mUpdateProg = gl::GlslProg::
			create( gl::GlslProg::Format().shaderStage( loadAsset( "particleUpdate.cs.glsl" ), GL_COMPUTE_SHADER ) );
	}
	catch( gl::GlslProgCompileExc e ) {
		ci::app::console() << e.what() << std::endl;
		shutdown();
	}

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

void ParticleSphereCSApp::update()
{
	// Update particles on the GPU
	gl::ScopedGlslProg prog( mUpdateProg );
	
	mUpdateProg->uniform( "uMouseForce", mMouseForce );
	mUpdateProg->uniform( "uMousePos", mMousePos );
	gl::ScopedBuffer scopedParticleSsbo( mParticleBuffer );

	glDispatchCompute( NUM_PARTICLES / WORK_GROUP_SIZE, 1, 1 );
	glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

	// Update mouse force.
	if( mMouseDown ) {
		mMouseForce = 150.0f;
	}
}

void ParticleSphereCSApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatricesWindowPersp( getWindowSize() );
	gl::enableDepthRead();
	gl::enableDepthWrite();

	gl::ScopedGlslProg render( mRenderProg );
	gl::ScopedBuffer scopedParticleSsbo( mParticleBuffer );
	gl::ScopedVao vao( mAttributes );
	
	gl::context()->setDefaultShaderVars();

	gl::drawArrays( GL_POINTS, 0, NUM_PARTICLES );
	
	gl::setMatricesWindow( app::getWindowSize() );
	gl::drawString( toString( static_cast<int>( getAverageFps() ) ) + " fps", vec2( 32.0f, 52.0f ) );
}

ivec3 ParticleSphereCSApp::getMaxComputeWorkGroupCount()
{
	ivec3 count;
	glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &count.x );
	glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &count.y );
	glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &count.z );
	return count;
}

ivec3 ParticleSphereCSApp::getMaxComputeWorkGroupSize()
{
	ivec3 size;
	glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &size.x );
	glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &size.y );
	glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &size.z );
	return size;
}

CINDER_APP_NATIVE( ParticleSphereCSApp, RendererGl )
