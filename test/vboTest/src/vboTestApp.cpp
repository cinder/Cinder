#include "flint/app/AppBasic.h"
#include "flint/gl/Texture.h"
#include "flint/gl/VBO.h"
#include "flint/Rand.h"
#include "Resources.h"
using namespace fli;
using namespace fli::app;

#define FLINT_VBO

class vboTestApp : public AppBasic {
 public:
	void setup();
	void draw();
	
	void keyDown( KeyEvent event );
	
	void drawImmediate();
	void drawVertexArrays();
	void drawVBOs();
	

	static const int BATCHES = 4;
	static const int BATCH_SIZE = 10000;
	
	gl::Texture		mTexture;
	float			*mVAVertices, *mVATexCoords;
	int				mDrawMode;

#if defined( FLINT_VBO )
	gl::VBO			mStaticTexCoordsVBO;
	gl::VBOMesh		mVBOMeshes[BATCHES];
#else
	GLuint			mVBOID[BATCHES];
	GLuint			mTexCoordVBO;
#endif
};

void vboTestApp::setup()
{
	mTexture = gl::Texture( loadImage( loadResource( "Beyonce.jpg", RES_BEYONCE_JPG, "JPG" ) ) );
	
	mVAVertices = new float[BATCH_SIZE*3*4];
	mVATexCoords = new float[BATCH_SIZE*2*4];

	std::vector<Vec2f> texCoords;
	for( size_t s = 0; s < BATCH_SIZE; ++s ) {
		texCoords.push_back( Vec2f( 0, 1 ) );
		texCoords.push_back( Vec2f( 1, 1 ) );
		texCoords.push_back( Vec2f( 1, 0 ) );		
		texCoords.push_back( Vec2f( 0, 0 ) );
	}

#if defined( FLINT_VBO )	
	mStaticTexCoordsVBO = gl::VBO( GL_ARRAY_BUFFER );
	mStaticTexCoordsVBO.bufferData( texCoords.size() * sizeof(Vec2f), &texCoords[0], GL_STATIC_DRAW );
	gl::VBOMesh::Layout layout;
	layout.setStaticTexCoords2d();
	layout.setDynamicPositions();
	for( size_t s = 0; s < BATCHES; ++s ) {
		mVBOMeshes[s] = gl::VBOMesh( BATCH_SIZE * 4, 0, layout, GL_QUADS, NULL, &mStaticTexCoordsVBO, NULL );
	}
#else
	glGenBuffers( BATCHES, &mVBOID[0] );
	glGenBuffers( 1, &mTexCoordVBO );
	glBindBuffer( GL_ARRAY_BUFFER, mTexCoordVBO );
	glBufferData( GL_ARRAY_BUFFER, texCoords.size() * sizeof(Vec2f), &texCoords[0], GL_STATIC_DRAW );
#endif

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	
	mDrawMode = 0;
}

void vboTestApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'm' ) {
		mDrawMode = ( mDrawMode + 1 ) % 3;
		std::cout << "Rendering mode " << mDrawMode << std::endl;
	}
}

void vboTestApp::drawImmediate()
{
	float maxX = getWindowWidth();
	float maxY = getWindowHeight();
	
	mTexture.bind();
	for( int t = 0; t < BATCHES; ++t ) {
		glBegin( GL_QUADS );
		for( int s = 0; s < BATCH_SIZE; ++s ) {
			Vec3f pos = Vec3f( Rand::randFloat( maxX ), Rand::randFloat( maxY ), 0 );
			float size = Rand::randFloat( 5, 25 );
			glTexCoord2f( 0, 0 );
			gl::vertex( pos + Vec3f( -size, -size, 0 ) );
			glTexCoord2f( 1, 0 );
			gl::vertex( pos + Vec3f( size, -size, 0 ) );
			glTexCoord2f( 1, 1 );
			gl::vertex( pos + Vec3f( size, size, 0 ) );
			glTexCoord2f( 0, 1 );
			gl::vertex( pos + Vec3f( -size, size, 0 ) );			
		}
		glEnd();
	}
}

void vboTestApp::drawVertexArrays()
{
	float maxX = getWindowWidth();
	float maxY = getWindowHeight();

	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glTexCoordPointer( 2, GL_FLOAT, 0, &mVATexCoords[0] );
	glVertexPointer( 3, GL_FLOAT, 0, &mVAVertices[0] );
	mTexture.bind();
	for( int t = 0; t < BATCHES; ++t ) {
		float *vp = &mVAVertices[0];
		float *tp = &mVATexCoords[0];
		for( int s = 0; s < BATCH_SIZE; ++s ) {
			Vec3f pos = Vec3f( Rand::randFloat( maxX ), Rand::randFloat( maxY ), 0 );
			float size = Rand::randFloat( 5, 25 );
			tp[0+0] = 0; tp[0+1] = 1;
			vp[0+0] = pos.x - size; vp[0+1] = pos.y - size; vp[0+2] = 0;
			tp[2+0] = 1; tp[2+1] = 1;
			vp[3+0] = pos.x + size; vp[3+1] = pos.y - size; vp[3+2] = 0;
			tp[4+0] = 1; tp[4+1] = 0;
			vp[6+0] = pos.x + size; vp[6+1] = pos.y + size; vp[6+2] = 0;
			tp[6+0] = 0; tp[6+1] = 0;
			vp[9+0] = pos.x - size; vp[9+1] = pos.y + size; vp[9+2] = 0;
			tp += 8;
			vp += 12;
		}
		glDrawArrays( GL_QUADS, 0, BATCH_SIZE * 4 );
	}

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );	
}

#if ! defined( FLINT_VBO )
void vboTestApp::drawVBOs()
{
	float maxX = getWindowWidth();
	float maxY = getWindowHeight();

	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glBindBuffer( GL_ARRAY_BUFFER, mTexCoordVBO );
	glTexCoordPointer( 2, GL_FLOAT, 0, (const GLvoid*)0 );
	
	mTexture.bind();
	for( int t = 0; t < BATCHES; ++t ) {
		glBindBuffer( GL_ARRAY_BUFFER, mVBOID[t] );
		glBufferData( GL_ARRAY_BUFFER, 3 * 4 * sizeof(float) * BATCH_SIZE, NULL, GL_STREAM_DRAW );
		float *ptr = (float*)glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );
		for( int s = 0; s < BATCH_SIZE; ++s ) {
			Vec3f pos = Vec3f( Rand::randFloat( maxX ), Rand::randFloat( maxY ), 0 );
			float size = Rand::randFloat( 5, 25 );
			ptr[3*0+0] = pos.x - size; ptr[3*0+1] = pos.y - size; ptr[3*0+2] = 0;
			ptr[3*1+0] = pos.x + size; ptr[3*1+1] = pos.y - size; ptr[3*1+2] = 0;
			ptr[3*2+0] = pos.x + size; ptr[3*2+1] = pos.y + size; ptr[3*2+2] = 0;
			ptr[3*3+0] = pos.x - size; ptr[3*3+1] = pos.y + size; ptr[3*3+2] = 0;
			ptr += 4 * 3;
		}
		glUnmapBuffer( GL_ARRAY_BUFFER );
		glVertexPointer( 3, GL_FLOAT, 3 * sizeof(float), (const GLvoid*)0 );		
		glDrawArrays( GL_QUADS, 0, BATCH_SIZE * 4 );
	}

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );		
}
#else
void vboTestApp::drawVBOs()
{
	float maxX = getWindowWidth();
	float maxY = getWindowHeight();

	glClientActiveTexture( GL_TEXTURE0 );
	mTexture.enableAndBind();
	for( int t = 0; t < BATCHES; ++t ) {
		{
			gl::VBOMesh::VertexIter iter = mVBOMeshes[t].mapVertexBuffer();
			float *ptr = (float*)iter.getPointer();
			for( int s = 0; s < BATCH_SIZE; ++s ) {
				Vec3f pos = Vec3f( Rand::randFloat( maxX ), Rand::randFloat( maxY ), 0 );
				float size = Rand::randFloat( 5, 25 );
				ptr[3*0+0] = pos.x - size; ptr[3*0+1] = pos.y - size; ptr[3*0+2] = 0;
				ptr[3*1+0] = pos.x + size; ptr[3*1+1] = pos.y - size; ptr[3*1+2] = 0;
				ptr[3*2+0] = pos.x + size; ptr[3*2+1] = pos.y + size; ptr[3*2+2] = 0;
				ptr[3*3+0] = pos.x - size; ptr[3*3+1] = pos.y + size; ptr[3*3+2] = 0;
				ptr += 4 * 3;
			}
		}
		gl::draw( mVBOMeshes[t] );
	}

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );		
}
#endif

void vboTestApp::draw()
{
	// this pair of lines is the standard way to clear the screen in OpenGL
	glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	gl::enableDepthRead();
	gl::enableDepthWrite();	

	if( mDrawMode == 0 )
		drawImmediate();
	else if( mDrawMode == 1 )
		drawVertexArrays();
	else if( mDrawMode == 2 )
		drawVBOs();
}

// This line tells Flint to actually create the application
FLI_APP_BASIC( vboTestApp )
