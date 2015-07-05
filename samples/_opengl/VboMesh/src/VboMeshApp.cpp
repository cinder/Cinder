//
// This sample demonstrates basic usage of the VboMesh class by creating a simple Plane mesh
// with a texture mapped onto it. The mesh has static indices and texture coordinates, but
// its vertex positions are dynamic.
//

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/GeomIO.h"
#include "cinder/ImageIo.h"
#include "cinder/CameraUi.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;

using std::vector;

class VboMeshApp : public App {
 public:
	void	setup() override;
	void	update() override;
	void	draw() override;

	void	keyDown( KeyEvent event ) override;

  private:
	gl::VboMeshRef	mVboMesh;
	gl::TextureRef	mTexture;
	
	CameraPersp		mCamera;
	CameraUi		mCamUi;
};

void VboMeshApp::setup()
{
	// create some geometry using a geom::Plane
	auto plane = geom::Plane().size( vec2( 20, 20 ) ).subdivisions( ivec2( 200, 50 ) );

	// Specify two planar buffers - positions are dynamic because they will be modified
	// in the update() loop. Tex Coords are static since we don't need to update them.
	vector<gl::VboMesh::Layout> bufferLayout = {
		gl::VboMesh::Layout().usage( GL_DYNAMIC_DRAW ).attrib( geom::Attrib::POSITION, 3 ),
		gl::VboMesh::Layout().usage( GL_STATIC_DRAW ).attrib( geom::Attrib::TEX_COORD_0, 2 )
	};

	mVboMesh = gl::VboMesh::create( plane, bufferLayout );

	mTexture = gl::Texture::create( loadImage( loadResource( RES_IMAGE ) ), gl::Texture::Format().loadTopDown() );
	
	mCamUi = CameraUi( &mCamera, getWindow() );
}

void VboMeshApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'w' )
		gl::setWireframeEnabled( ! gl::isWireframeEnabled() );
}

void VboMeshApp::update()
{
	float offset = getElapsedSeconds() * 4.0f;

	// Dynmaically generate our new positions based on a sin(x) + cos(z) wave
	// We set 'orphanExisting' to false so that we can also read from the position buffer, though keep
	// in mind that this isn't the most efficient way to do cpu-side updates. Consider using VboMesh::bufferAttrib() as well.
	auto mappedPosAttrib = mVboMesh->mapAttrib3f( geom::Attrib::POSITION, false );
	for( int i = 0; i < mVboMesh->getNumVertices(); i++ ) {
		vec3 &pos = *mappedPosAttrib;
		mappedPosAttrib->y = sinf( pos.x * 1.1467f + offset ) * 0.323f + cosf( pos.z * 0.7325f + offset ) * 0.431f;
		++mappedPosAttrib;
	}
	mappedPosAttrib.unmap();
}

void VboMeshApp::draw()
{
	gl::clear( Color( 0.15f, 0.15f, 0.15f ) );

	gl::setMatrices( mCamera );

	gl::ScopedGlslProg glslScope( gl::getStockShader( gl::ShaderDef().texture() ) );
	gl::ScopedTextureBind texScope( mTexture );

	gl::draw( mVboMesh );
}


CINDER_APP( VboMeshApp, RendererGl( RendererGl::Options().msaa( 16 ) ) )
