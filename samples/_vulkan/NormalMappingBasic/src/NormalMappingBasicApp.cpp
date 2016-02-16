#include "cinder/app/App.h"
#include "cinder/app/RendererVk.h"
#include "cinder/vk/vk.h"
#include "cinder/ImageIo.h"

using namespace ci;
using namespace ci::app;

class NormalMappingBasicApp : public App {
  public:	
	void	setup() override;
	void	resize() override;
	void	update() override;
	void	draw() override;
	
	CameraPersp			mCam;
	vk::BatchRef		mBatch;
	vk::TextureRef		mDiffuseTex, mNormalTex;
	vk::GlslProgRef		mGlsl;
	mat4				mCubeRotation;
	
	vec3				mLightPosWorldSpace;
};

void NormalMappingBasicApp::setup()
{
	mCam.lookAt( vec3( 3, 2, 4 ), vec3( 0 ) );
	
	auto texFmt = vk::Texture::Format();
	texFmt.setMipmapEnabled();
	mDiffuseTex = vk::Texture::create( *Surface::create( loadImage( loadAsset( "diffuseMap.jpg" ) ) ), texFmt );
	mNormalTex  = vk::Texture::create( *Surface::create( loadImage( loadAsset( "normalMap.png"  ) ) ), texFmt );

	try {
		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.vertex( loadAsset("shader.vert") )
			.fragment( loadAsset("shader.frag") )
			.binding( "ciBlock0", 0 )
			.binding( "uDiffuseMap", 1 )
			.binding( "uNormalMap", 2 )
			.binding( "ciBlock1", 3 )
			.attribute( geom::Attrib::POSITION,    0, 0, vk::glsl_attr_vec4 )
			.attribute( geom::Attrib::NORMAL,      1, 0, vk::glsl_attr_vec3 )
			.attribute( geom::Attrib::TANGENT,     2, 0, vk::glsl_attr_vec3 )
			.attribute( geom::Attrib::TEX_COORD_0, 3, 0, vk::glsl_attr_vec2 );
			//.uniformLayout( uniformLayout );

		mGlsl = vk::GlslProg::create( format );

		mBatch = vk::Batch::create( geom::Cube() >> geom::Transform( scale( vec3( 1.5f ) ) ), mGlsl );
		mBatch->uniform( "uDiffuseMap", mDiffuseTex );
		mBatch->uniform( "uNormalMap", mNormalTex );
		mBatch->uniform( "uLightLocViewSpace", vec3( 0, 0, 1 ) );
	}
	catch( const std::exception& e ) {
		console() << "Load Error: " << e.what() << std::endl;
	}

	//vk::enableDepthWrite();
	//vk::enableDepthRead();
}

void NormalMappingBasicApp::resize()
{
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
	vk::setMatrices( mCam );
}

void NormalMappingBasicApp::update()
{
	mCubeRotation *= rotate( 0.01f, normalize( vec3( 0.75f, cos( getElapsedSeconds() ), 0.33f ) ) );

	mLightPosWorldSpace = vec3( 4, 4, 4 );
}

void NormalMappingBasicApp::draw()
{
	//vk::clear();

	vk::setMatrices( mCam );

	vk::ScopedModelMatrix modelScope;
	vk::multModelMatrix( mCubeRotation );
	mBatch->uniform( "uLightLocViewSpace", vec3( mCam.getViewMatrix() * vec4( mLightPosWorldSpace, 1 )) );
	mBatch->draw();
}

CINDER_APP( NormalMappingBasicApp, RendererVk )