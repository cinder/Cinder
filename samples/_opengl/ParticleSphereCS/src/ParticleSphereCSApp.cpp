#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
#include "cinder/gl/Ssbo.h"


using namespace ci;
using namespace ci::app;
using namespace std;


// Particle type holds information for rendering and simulation.
// Used to buffer initial simulation values.
// Using std140 in ssbo requires we have members on 4 byte alignment. 
#pragma pack( push, 1 )
//__declspec( align( 4 ) )
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

	This sample is the same as ParticleSphereGPU.  The only difference is
	that it uses a compute shader instead of transform feedback.
 */
class ParticleSphereCSApp : public App {
  public:
	void setup() override;
	void update() override;
	void draw() override;

  private:
	enum { WORK_GROUP_SIZE = 128, };
	gl::GlslProgRef mRenderProg;
	gl::GlslProgRef mUpdateProg;

	// Buffers holding raw particle data on GPU.
	gl::SsboRef mParticleBuffer;
	gl::VboRef mIdsVbo;
	gl::VaoRef mAttributes;

	// Mouse state suitable for passing as uniforms to update program
	bool			mMouseDown = false;
	float			mMouseForce = 0.0f;
	vec3			mMousePos = vec3( 0 );
};

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
		p.ppos = p.home + Rand::randVec3() * 10.0f; // random initial velocity
		p.damping = Rand::randFloat( 0.965f, 0.985f );
		Color c( CM_HSV, lmap<float>( static_cast<float>(i), 0.0f, static_cast<float>( particles.size() ), 0.0f, 0.66f ), 1.0f, 1.0f );
		p.color = vec4( c.r, c.g, c.b, 1.0f );
	}

	ivec3 count = gl::getMaxComputeWorkGroupCount();
	CI_ASSERT( count.x >= ( NUM_PARTICLES / WORK_GROUP_SIZE ) );

	// Create particle buffers on GPU and copy data into the first buffer.
	// Mark as static since we only write from the CPU once.
	mParticleBuffer = gl::Ssbo::create( particles.size() * sizeof(Particle), particles.data(), GL_STATIC_DRAW );
	gl::ScopedBuffer scopedParticleSsbo( mParticleBuffer );
	mParticleBuffer->bindBase( 0 );
	
	// Create a default color shader.
	try {
#if CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3_1 
		mRenderProg = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "particleRender_es31.vert" ) )
			.fragment( loadAsset( "particleRender_es31.frag" ) )
			.attribLocation( "particleId", 0 ) );
#else
		mRenderProg = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "particleRender.vert" ) )
			.fragment( loadAsset( "particleRender.frag" ) )
			.attribLocation( "particleId", 0 ) );
#endif
	}
	catch( gl::GlslProgCompileExc e ) {
		ci::app::console() << e.what() << std::endl;
		quit();
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
#if CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3_1 
		mUpdateProg = gl::GlslProg::
			create( gl::GlslProg::Format().compute( loadAsset( "particleUpdate_es31.comp" ) ) );
#else
		mUpdateProg = gl::GlslProg::
			create( gl::GlslProg::Format().compute( loadAsset( "particleUpdate.comp" ) ) );
#endif
	}
	catch( gl::GlslProgCompileExc e ) {
		ci::app::console() << e.what() << std::endl;
		quit();
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

	gl::dispatchCompute( NUM_PARTICLES / WORK_GROUP_SIZE, 1, 1 );
	gl::memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

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

CINDER_APP( ParticleSphereCSApp, RendererGl, []( App::Settings *settings ) {
	settings->setWindowSize( 1280, 720 );
	settings->setMultiTouchEnabled( false );
} )
