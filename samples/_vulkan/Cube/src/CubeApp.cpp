#include "cinder/app/App.h"
#include "cinder/app/RendererVk.h"
#include "cinder/vk/vk.h"
#include "cinder/ImageIo.h"

using namespace ci;
using namespace ci::app;

class RotatingCubeApp : public App {
  public:	
	void	setup() override;
	void	resize() override;
	void	update() override;
	void	draw() override;
	
	CameraPersp			mCam;
	vk::BatchRef		mBatch;
	vk::TextureRef		mTexture;
	vk::GlslProgRef		mGlsl;
	mat4				mCubeRotation;
};

void RotatingCubeApp::setup()
{
	mCam.lookAt( vec3( 3, 2, 4 ), vec3( 0 ) );

	try {
		auto surface = Surface::create( loadImage( loadAsset( "texture.jpg" ) ) );
		auto texFmt = vk::Texture::Format();
		texFmt.setMipmapEnabled();
		mTexture = vk::Texture::create( *surface, texFmt );
		console() << "Loaded texture" << std::endl;
	}
	catch( const std::exception& e ) {
		console() << "Texture Error: " << e.what() << std::endl;
	}

	try {
		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.vertex( loadAsset("shader.vert") )
			.fragment( loadAsset("shader.frag") )
			.binding( "ciBlock0", 0 )
			.binding( "uTex0", 1 )
			.attribute( geom::Attrib::POSITION,    0, 0, vk::glsl_attr_vec4 )
			.attribute( geom::Attrib::TEX_COORD_0, 1, 0, vk::glsl_attr_vec2 )
			.attribute( geom::Attrib::NORMAL,      2, 0, vk::glsl_attr_vec3 )
			.attribute( geom::Attrib::COLOR,       3, 0, vk::glsl_attr_vec4 );

		mGlsl = vk::GlslProg::create( format );
		mGlsl->uniform( "uTex0", mTexture );
		console() << "Loaded shader" << std::endl;
	}
	catch( const std::exception& e ) {
		console() << "Shader Error: " << e.what() << std::endl;
	}

	try {
		ColorAf c = ColorAf( 1, 1, 1, 1 );
		mBatch = vk::Batch::create( geom::Cube().colors( c, c, c, c, c, c ), mGlsl );
		console() << "Created batch" << std::endl;
	}
	catch( const std::exception& e ) {
		console() << "Batch Error: " << e.what() << std::endl;
	}

	vk::enableDepthWrite();
	vk::enableDepthRead();
}

void RotatingCubeApp::resize()
{
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );

	vk::setMatrices( mCam );
}

void RotatingCubeApp::update()
{
	// Rotate the cube by 0.2 degrees around the y-axis
	mCubeRotation *= rotate( toRadians( 0.2f ), normalize( vec3( 0, 1, 0 ) ) );
}

void RotatingCubeApp::draw()
{
	vk::setMatrices( mCam );

	vk::ScopedModelMatrix modelScope;
	vk::multModelMatrix( mCubeRotation );

	mBatch->draw();
}

//CINDER_APP( RotatingCubeApp, RendererGl( Renderervk::Options().msaa( 16 ) ) )
CINDER_APP( RotatingCubeApp, RendererVk( RendererVk::Options().setSamples( VK_SAMPLE_COUNT_8_BIT ) ) )
