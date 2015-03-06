#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Vao.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Fbo.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ObjectTrackingApp : public App {
  public:
	void setup();
	void draw();

	vector<gl::TextureRef>		mTexs;
	vector<gl::VboRef>			mVbos;
	vector<gl::GlslProgRef>		mGlslProgs;
	vector<gl::VaoRef>			mVaos;
	vector<gl::FboRef>			mFbos;
};

template<typename T>
void generateRandom( vector<T> *v, size_t ct );

// generateRandom Textures
template<>
void generateRandom( vector<gl::TextureRef> *v, size_t ct )
{
	for( size_t i = 0; i < ct; ++i ) {
		Surface temp( 128, 128, false );
		v->push_back( gl::Texture::create( temp ) );
	}
}

// generateRandom VBOs
template<>
void generateRandom( vector<gl::VboRef> *v, size_t ct )
{
	for( size_t i = 0; i < ct; ++i )
		v->push_back( gl::Vbo::create( GL_ARRAY_BUFFER, 123 ) );
}

// generateRandom GlslProgs
template<>
void generateRandom( vector<gl::GlslProgRef> *v, size_t ct )
{
	for( size_t i = 0; i < ct; ++i ) {
		auto temp = gl::GlslProg::create( gl::GlslProg::Format()
#if ! defined( CINDER_GL_ES )
			.vertex(	CI_GLSL( 150,
				uniform mat4	ciModelViewProjection;
				in vec4			ciPosition;
				
				void main( void ) {
					gl_Position	= ciModelViewProjection * ciPosition;
				}
			 ) )
			.fragment(	CI_GLSL( 150,
				out vec3	oColor;
				
				void main( void ) {
					oColor = vec3( 1, 1, 1 );
				}
			) ) );
			
#else
			.vertex(	"void main(void) { gl_Position = vec4( 1, 1, 1, 1 ); }")
			.fragment( "void main( void ) { gl_FragColor = vec4( 1, 1, 1, 1 ); }" )
			);
#endif
		v->push_back( temp );
	}
}

// generateRandom VAOs
template<>
void generateRandom( vector<gl::VaoRef> *v, size_t ct )
{
	for( size_t i = 0; i < ct; ++i )
		v->push_back( gl::Vao::create() );
}

// generateRandom FBOs
template<>
void generateRandom( vector<gl::FboRef> *v, size_t ct )
{
	for( size_t i = 0; i < ct; ++i )
		v->push_back( gl::Fbo::create( 128, 128 ) );
}

template<typename T>
void eraseRandom( vector<T> *v, size_t ct )
{
	ct = std::min( v->size(), ct );
	for( size_t i = 0; i < ct; ++i )
		v->erase( v->begin() + randInt( v->size() ) );
}

template<typename T, typename F>
void processObjects( vector<T> *v, const F &f, int defaultSize )
{
	// verify we're starting with 'defaultSize' T's
	// There is always a default VAO and default GlslProg, so these are 1's
	assert( f().size() == defaultSize );

	// Create some random T's, then delete a random number, then create a number
	// and verify tracking thinks we have the same #	
	generateRandom( v, randInt( 10 ) );
	assert( v->size() == f().size() - defaultSize );
	
	generateRandom( v, randInt( 10 ) );
	eraseRandom( v, randInt( 10 ) );
	assert( v->size() == f().size() - defaultSize );
	
	// echo all remaining T's to the console
	for( const auto &t : f() )
		console() << *t << std::endl;

	// verify clearing T's results in 'defaultSize' remaining
	v->clear();
	assert( f().size() == defaultSize );
}

void ObjectTrackingApp::setup()
{
	console() << "Textures" << std::endl;
	processObjects( &mTexs, []() { return gl::context()->getLiveTextures(); }, 0 );
	console() << "Buffers" << std::endl;
	processObjects( &mVbos, []() { return gl::context()->getLiveBuffers(); }, 0 );
	console() << "GlslProgs" << std::endl;
	processObjects( &mGlslProgs, []() { return gl::context()->getLiveGlslProgs(); }, 1 );
	console() << "VAOs" << std::endl;
#if ! defined( CINDER_GL_ES )
	processObjects( &mVaos, []() { return gl::context()->getLiveVaos(); }, 1 );
#else
	processObjects( &mVaos, []() { return gl::context()->getLiveVaos(); }, 0 );
#endif
	console() << "FBOs" << std::endl;
	processObjects( &mFbos, []() { return gl::context()->getLiveFbos(); }, 0 );
}

void ObjectTrackingApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( ObjectTrackingApp, RendererGl( RendererGl::Options().objectTracking() ) )
