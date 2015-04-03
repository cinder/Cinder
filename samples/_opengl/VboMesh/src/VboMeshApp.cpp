//
// This sample demonstrates basic usage of the VboMesh class by creating a simple Plane mesh
// with a texture mapped onto it. The mesh has static indices and texture coordinates, but
// its vertex positions are dynamic.
//

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/GeomIO.h"
#include "cinder/ImageIo.h"
#include "cinder/MayaCamUI.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;

using std::vector;

class VboSampleApp : public App {
 public:
	void	setup() override;
	void	update() override;
	void	draw() override;

	void	keyDown( KeyEvent event ) override;
	void	mouseDown( MouseEvent event ) override;
	void	mouseDrag( MouseEvent event ) override;

  private:
	gl::VboMeshRef	mVboMesh;
	gl::TextureRef	mTexture;
	MayaCamUI		mMayaCam;
};

void VboSampleApp::setup()
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
}

void VboSampleApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'w' )
		gl::setWireframeEnabled( ! gl::isWireframeEnabled() );
}

void VboSampleApp::mouseDown( MouseEvent event )
{
	mMayaCam.mouseDown( event.getPos() );
}

void VboSampleApp::mouseDrag( MouseEvent event )
{
#if defined( CINDER_MAC )
	// remapping key modifiers so mayacam is usable on a mac touchpad
	bool isLeftDown = event.isLeftDown();
	bool isMiddleDown = event.isAltDown();
	bool isRightDown = event.isMetaDown();

	if( isMiddleDown )
		isLeftDown = false;

	mMayaCam.mouseDrag( event.getPos(), isLeftDown, isMiddleDown, isRightDown );
#else
	mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
#endif
}

void VboSampleApp::update()
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

void VboSampleApp::draw()
{
	gl::clear( Color( 0.15f, 0.15f, 0.15f ) );

	gl::setMatrices( mMayaCam.getCamera() );

	gl::ScopedGlslProg glslScope( gl::getStockShader( gl::ShaderDef().texture() ) );
	gl::ScopedTextureBind texScope( mTexture );

	gl::draw( mVboMesh );
}


CINDER_APP( VboSampleApp, RendererGl )
