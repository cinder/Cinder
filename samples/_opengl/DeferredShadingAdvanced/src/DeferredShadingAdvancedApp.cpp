/*
 * Deferred Shading Advanced sample application by:
 * Stephen Schieberl - Wieden+Kennedy
 * Michael Latzoni - Wieden+Kennedy
 *
 * Additional code:
 * Simon Geilfus
 *
 * License: BSD Simplified
 *
 * This sample is a grab-bag of rendering techniques built upon the
 * foundation of a deferred shading pipeline. You may use it as an
 * out-of-the-box rendering engine or cherry pick or tweak it to make
 * your own.
 * 
 * Detailed notes on the pipeline and specific techniques may be found 
 * in the ::draw() method.
 */

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"
#include "cinder/CinderImGui.h"

#include "Light.h"
#include "Material.h"
#include "Model.h"

class DeferredShadingAdvancedApp : public ci::app::App
{
private:
	enum : int32_t
	{
		Ao_None,
		Ao_Hbao,
		Ao_Sao
	} typedef Ao;
public:
	DeferredShadingAdvancedApp();
	
	void						draw() override;
	void						resize() override;
	void						update() override;
	void						updateGui();
private:
	ci::CameraPersp				mCamera;
	ci::CameraUi				mCamUi;
	ci::CameraPersp				mShadowCamera;
	
	std::vector<Light>			mLights;
	std::vector<Material>		mMaterials;
	int32_t						mNumBouncingLights;
	int32_t						mNumIcosahedrons;
	int32_t						mNumSpheres;
	ci::gl::VboRef				mVboInstancedIcosahedrons;
	ci::gl::VboRef				mVboInstancedSpheres;
	
	ci::gl::UboRef				mUboLight;
	ci::gl::UboRef				mUboMaterial;
	
	ci::gl::FboRef				mFboAo;
	ci::gl::FboRef				mFboAccum;
	ci::gl::FboRef				mFboCsz;
	ci::gl::FboRef				mFboGBuffer;
	ci::gl::FboRef				mFboPingPong;
	ci::gl::FboRef				mFboRayColor;
	ci::gl::FboRef				mFboRayDepth;
	ci::gl::FboRef				mFboShadowMap;
	
	ci::gl::Texture2dRef		mTextureFboAo[ 2 ];
	ci::gl::Texture2dRef		mTextureFboAccum[ 3 ];
	ci::gl::Texture2dRef		mTextureFboGBuffer[ 3 ];
	ci::gl::Texture2dRef		mTextureFboPingPong[ 2 ];
	ci::gl::Texture2dRef		mTextureFboRayColor[ 2 ];
	
	void						createBatches();

	ci::gl::BatchRef			mBatchDebugRect;
	ci::gl::BatchRef			mBatchEmissiveRect;
	ci::gl::BatchRef			mBatchGBufferCylinder;
	ci::gl::BatchRef			mBatchGBufferIcosahedron;
	ci::gl::BatchRef			mBatchGBufferLightSourceSphere;
	ci::gl::BatchRef			mBatchGBufferSphere;
	ci::gl::BatchRef			mBatchLBufferLightCube;
	ci::gl::BatchRef			mBatchLBufferShadowRect;
	ci::gl::BatchRef			mBatchShadowMapIcosahedron;
	ci::gl::BatchRef			mBatchShadowMapSphere;
	
	ci::gl::BatchRef			mBatchAoCompositeRect;
	ci::gl::BatchRef			mBatchHbaoAoRect;
	ci::gl::BatchRef			mBatchHbaoBlurRect;
	ci::gl::BatchRef			mBatchSaoAoRect;
	ci::gl::BatchRef			mBatchSaoBlurRect;
	ci::gl::BatchRef			mBatchSaoCszRect;
	
	ci::gl::BatchRef			mBatchBloomBlurRect;
	ci::gl::BatchRef			mBatchBloomCompositeRect;
	ci::gl::BatchRef			mBatchBloomHighpassRect;
	
	ci::gl::BatchRef			mBatchColorRect;
	ci::gl::BatchRef			mBatchDofRect;
	ci::gl::BatchRef			mBatchFogRect;
	ci::gl::BatchRef			mBatchFxaaRect;

	ci::gl::BatchRef			mBatchRayCompositeRect;
	ci::gl::BatchRef			mBatchRayOccludeRect;
	ci::gl::BatchRef			mBatchRayScatterRect;
	
	ci::gl::BatchRef			mBatchStockColorRect;
	ci::gl::BatchRef			mBatchStockColorSphere;
	ci::gl::BatchRef			mBatchStockTextureRect;
	
	void						setUniforms();
	
	bool						mEnabledAoBlur;
	bool						mEnabledColor;
	bool						mEnabledBloom;
	bool						mEnabledDoF;
	bool						mEnabledFog;
	bool						mEnabledFxaa;
	bool						mEnabledRay;
	bool						mEnabledRayPrev;
	bool						mEnabledShadow;

	bool						mPaused;
	
	ci::vec3					mSpherePosition;
	float						mSphereVelocity;
	
	bool						mDrawAo;
	bool						mDrawDebug;
	bool						mDrawLightVolume;
	
	bool						mHighQuality;
	bool						mHighQualityPrev;
	
	int32_t						mAo;
	int32_t						mAoPrev;
	int32_t						mMipmapLevels;
	ci::vec2					mOffset;
	
	float						mFrameRate;
	bool						mFullScreen;
	void						screenShot();
};

#include "cinder/app/RendererGl.h"
#include "cinder/ImageIo.h"
#include "cinder/Log.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

DeferredShadingAdvancedApp::DeferredShadingAdvancedApp()
{
	// Set default values for all properties
	mAo					= Ao_Sao;
	mAoPrev				= mAo;
	mDrawLightVolume	= false;
	mDrawAo				= false;
	mDrawDebug			= false;
	mEnabledAoBlur		= true;
	mEnabledColor		= true;
	mEnabledBloom		= true;
	mEnabledDoF			= true;
	mEnabledFog			= true;
	mEnabledFxaa		= true;
	mEnabledRay			= true;
	mEnabledRayPrev		= mEnabledRay;
	mEnabledShadow		= true;
	mFrameRate			= 0.0f;
	mFullScreen			= isFullScreen();
	mHighQuality		= false;
	mHighQualityPrev	= mHighQuality;
	mMipmapLevels		= 5;
	mNumBouncingLights	= 11;
	mNumIcosahedrons	= 7;
	mNumSpheres			= 3;
	mOffset				= vec2( 0.0f );
	mPaused				= false;
	mSpherePosition		= vec3( 0.0f, 4.0f, 0.0f );
	mSphereVelocity		= -0.1f;

	// Set up camera
	const vec2 windowSize = toPixels( getWindowSize() );
	mCamera = CameraPersp( (int32_t)windowSize.x, (int32_t)windowSize.y, 60.0f, 0.01f, 100.0f );
	mCamera.lookAt( vec3( 0.0f, 0.5f, 7.0f ), vec3( 0.0f, 2.7f, 0.0f ) );
	mCamUi = CameraUi( &mCamera, getWindow(), -1 );
	
	// Bouncing lights
	for ( size_t i = 0; i < mNumBouncingLights; ++i ) {
		const float t = (float)i / (float)mNumBouncingLights;
		ColorAf c( 0.91f + t * 0.1f, 0.5f + t * 0.5f, 0.9f - t * 0.25f, 1.0f );
		mLights.push_back( Light().colorDiffuse( c )
						  .intensity( 0.5f ).radius( 0.1f ).volume( 5.0f ) );
	}
	
	// Perimeter lights
	const float d	= ( (float)M_PI * 2.0f ) / 5.0f;
	const float r	= 9.0f;
	float t			= 0.0f;
	for ( size_t i = 0; i < 5; ++i, t += d ) {
		vec3 p( glm::cos( t ) * r, 0.5f, glm::sin( t ) * r );
		mLights.push_back( Light().colorDiffuse( ColorAf( 0.85f, 0.7f, 1.0f, 1.0f ) )
						  .intensity( 1.0f ).position( p ).radius( 0.1f ).volume( 3.0f ) );
	}
	
	// Primary light
	mLights.push_back( Light().colorDiffuse( ColorAf( 1.0f, 0.95f, 0.9f, 1.0f ) )
					  .intensity( 1.2f ).radius( 0.3f ).volume( 15.0f ) );
	
	// Set up materials
	mMaterials.push_back( Material().colorDiffuse( Colorf::white() ).colorSpecular( Colorf::white() )
						 .shininess( 300.0f ) ); // Sphere
	mMaterials.push_back( Material().colorAmbient( ColorAf::gray( 0.18f ) )
						 .colorDiffuse( Colorf::gray( 0.6f ) ).colorSpecular( Colorf::gray( 0.22f ) )
						 .shininess( 20.0f ) ); // Room
	mMaterials.push_back( Material().colorAmbient( ColorAf::black() )
						 .colorDiffuse( Colorf::black() ).colorEmission( Colorf::white() )
						 .shininess( 100.0f ) ); // Light
	
	// Create uniform buffer objects for lights and materials
	mUboLight		= gl::Ubo::create( sizeof( Light )		* mLights.size(),		mLights.data() );
	mUboMaterial	= gl::Ubo::create( sizeof( Material )	* mMaterials.size(),	mMaterials.data() );
	mUboLight->bindBufferBase( 0 );
	mUboMaterial->bindBufferBase( 1 );
	
	// Load shaders and create batches
	createBatches();
	
	// Call resize to create FBOs
	resize();

	ImGui::Initialize();
	
	gl::enableVerticalSync();
	gl::color( ColorAf::white() );
}

void DeferredShadingAdvancedApp::createBatches()
{
	/*
	 * In Cinder, a "batch" is a combination of geometry and shader code
	 * which makes drawing easy, but there is a bit of initial set up
	 * involved. This routine does the following:
	 *
	 * - Loads shader files from disk
	 * - Creates GLSL programs from the shader files
	 * - Generates VBO meshes from primitive geometry
	 * - Combines the geometry and GLSL programs into batches
	 * - Sets up special instancing data to reduce the overhead of draw calls
	 */
	
	// Shortcut for shader loading and error handling
	auto loadGlslProg = [ & ]( const gl::GlslProg::Format& format ) -> gl::GlslProgRef
	{
		string names = format.getVertexPath().string() + " + " +
		format.getFragmentPath().string();
		gl::GlslProgRef glslProg;
		try {
			glslProg = gl::GlslProg::create( format );
		} catch ( const Exception& ex ) {
			CI_LOG_EXCEPTION( names, ex );
			quit();
		}
		return glslProg;
	};
	
	// Load shader files
	DataSourceRef fragAoComposite			= loadAsset( "ao/composite.frag" );
	DataSourceRef fragAoHbaoAo				= loadAsset( "ao/hbao/ao.frag" );
	DataSourceRef fragAoHbaoBlur			= loadAsset( "ao/hbao/blur.frag" );
	DataSourceRef fragAoSaoAo				= loadAsset( "ao/sao/ao.frag" );
	DataSourceRef fragAoSaoBlur				= loadAsset( "ao/sao/blur.frag" );
	DataSourceRef fragAoSaoCsz				= loadAsset( "ao/sao/csz.frag" );
	DataSourceRef fragBloomBlur				= loadAsset( "bloom/blur.frag" );
	DataSourceRef fragBloomComposite		= loadAsset( "bloom/composite.frag" );
	DataSourceRef fragBloomHighpass			= loadAsset( "bloom/highpass.frag" );
	DataSourceRef fragDeferredDebug			= loadAsset( "deferred/debug.frag" );
	DataSourceRef fragDeferredEmissive		= loadAsset( "deferred/emissive.frag" );
	DataSourceRef fragDeferredGBuffer		= loadAsset( "deferred/gbuffer.frag" );
	DataSourceRef fragDeferredLBufferLight	= loadAsset( "deferred/lbuffer_light.frag" );
	DataSourceRef fragDeferredLBufferShadow	= loadAsset( "deferred/lbuffer_shadow.frag" );
	DataSourceRef fragDeferredShadowMap		= loadAsset( "deferred/shadow_map.frag" );
	DataSourceRef fragPostColor				= loadAsset( "post/color.frag" );
	DataSourceRef fragPostDof				= loadAsset( "post/dof.frag" );
	DataSourceRef fragPostFog				= loadAsset( "post/fog.frag" );
	DataSourceRef fragPostFxaa				= loadAsset( "post/fxaa.frag" );
	DataSourceRef fragRayComposite			= loadAsset( "ray/composite.frag" );
	DataSourceRef fragRayOcclude			= loadAsset( "ray/occlude.frag" );
	DataSourceRef fragRayScatter			= loadAsset( "ray/scatter.frag" );
	
	DataSourceRef vertDeferredGBuffer		= loadAsset( "deferred/gbuffer.vert" );
	DataSourceRef vertDeferredLBufferLight	= loadAsset( "deferred/lbuffer_light.vert" );
	DataSourceRef vertPassThrough			= loadAsset( "pass_through.vert" );

	// Create GLSL programs
	string numLights				= toString( mLights.size() );
	string numMaterials				= toString( mMaterials.size() );
	int32_t version					= 330;
	gl::GlslProgRef aoComposite		= loadGlslProg( gl::GlslProg::Format().version( version )
												   .vertex( vertPassThrough ).fragment( fragAoComposite )
												   .define( "TEX_COORD" ) );
	gl::GlslProgRef aoHbao			= loadGlslProg( gl::GlslProg::Format().version( version )
												   .vertex( vertPassThrough ).fragment( fragAoHbaoAo )
												   .define( "TEX_COORD" ) );
	gl::GlslProgRef aoHbaoBlur		= loadGlslProg( gl::GlslProg::Format().version( version )
												   .vertex( vertPassThrough ).fragment( fragAoHbaoBlur )
												   .define( "TEX_COORD" ) );
	gl::GlslProgRef aoSaoAo			= loadGlslProg( gl::GlslProg::Format().version( version )
												   .vertex( vertPassThrough ).fragment( fragAoSaoAo ) );
	gl::GlslProgRef aoSaoBlur		= loadGlslProg( gl::GlslProg::Format().version( version )
													.vertex( vertPassThrough ).fragment( fragAoSaoBlur )
													.define( "TEX_COORD" ) );
	gl::GlslProgRef aoSaoCsz		= loadGlslProg( gl::GlslProg::Format().version( version )
												   .vertex( vertPassThrough ).fragment( fragAoSaoCsz )
												   .define( "TEX_COORD" ) );
	gl::GlslProgRef bloomBlur		= loadGlslProg( gl::GlslProg::Format().version( version )
												   .vertex( vertPassThrough ).fragment( fragBloomBlur )
												   .define( "TEX_COORD" ) );
	gl::GlslProgRef bloomComposite	= loadGlslProg( gl::GlslProg::Format().version( version )
												   .vertex( vertPassThrough ).fragment( fragBloomComposite )
												   .define( "TEX_COORD" ) );
	gl::GlslProgRef bloomHighpass	= loadGlslProg( gl::GlslProg::Format().version( version )
												   .vertex( vertPassThrough ).fragment( fragBloomHighpass )
												   .define( "TEX_COORD" ) );
	gl::GlslProgRef debug			= loadGlslProg( gl::GlslProg::Format().version( version )
												   .vertex( vertPassThrough ).fragment( fragDeferredDebug )
												   .define( "TEX_COORD" ).define( "NUM_MATERIALS", numMaterials ) );
	gl::GlslProgRef emissive		= loadGlslProg( gl::GlslProg::Format().version( version )
												   .vertex( vertPassThrough ).fragment( fragDeferredEmissive )
												   .define( "TEX_COORD" ).define( "NUM_MATERIALS", numMaterials ) );
	gl::GlslProgRef gBuffer			= loadGlslProg( gl::GlslProg::Format().version( version )
												   .vertex( vertDeferredGBuffer ).fragment( fragDeferredGBuffer ) );
	gl::GlslProgRef gBufferInvNorm	= loadGlslProg( gl::GlslProg::Format().version( version )
												   .vertex( vertDeferredGBuffer ).fragment( fragDeferredGBuffer )
												   .define( "INVERT_NORMAL" ) );
	gl::GlslProgRef gBufferInst		= loadGlslProg( gl::GlslProg::Format().version( version )
												   .vertex( vertDeferredGBuffer ).fragment( fragDeferredGBuffer )
												   .define( "INSTANCED_MODEL" ) );
	gl::GlslProgRef gBufferInstLS	= loadGlslProg( gl::GlslProg::Format().version( version )
												   .vertex( vertDeferredGBuffer ).fragment( fragDeferredGBuffer )
												   .define( "INSTANCED_LIGHT_SOURCE" ).define( "NUM_LIGHTS", numLights ) );
	gl::GlslProgRef lBufferLight	= loadGlslProg( gl::GlslProg::Format().version( version )
												   .vertex( vertDeferredLBufferLight ).fragment( fragDeferredLBufferLight )
												   .define( "NUM_MATERIALS", numMaterials )
												   .define( "NUM_LIGHTS", numLights ) );
	gl::GlslProgRef lBufferShadow	= loadGlslProg( gl::GlslProg::Format().version( version )
												   .vertex( vertPassThrough ).fragment( fragDeferredLBufferShadow ) );
	gl::GlslProgRef shadowMapInst	= loadGlslProg( gl::GlslProg::Format().version( version )
												   .vertex( vertPassThrough ).fragment( fragDeferredShadowMap )
												   .define( "INSTANCED_MODEL" ) );
	gl::GlslProgRef postColor		= loadGlslProg( gl::GlslProg::Format().version( version )
												   .vertex( vertPassThrough ).fragment( fragPostColor )
												   .define( "TEX_COORD" ) );
	gl::GlslProgRef postDof			= loadGlslProg( gl::GlslProg::Format().version( version )
												   .vertex( vertPassThrough ).fragment( fragPostDof )
												   .define( "TEX_COORD" ) );
	gl::GlslProgRef postFog			= loadGlslProg( gl::GlslProg::Format().version( version )
												   .vertex( vertPassThrough ).fragment( fragPostFog )
												   .define( "TEX_COORD" ) );
	gl::GlslProgRef postFxaa		= loadGlslProg( gl::GlslProg::Format().version( version )
												   .vertex( vertPassThrough ).fragment( fragPostFxaa )
												   .define( "TEX_COORD" ) );
	gl::GlslProgRef rayComposite	= loadGlslProg( gl::GlslProg::Format().version( version )
												   .vertex( vertPassThrough ).fragment( fragRayComposite )
												   .define( "TEX_COORD" ) );
	gl::GlslProgRef rayOcclude		= loadGlslProg( gl::GlslProg::Format().version( version )
												   .vertex( vertPassThrough ).fragment( fragRayOcclude )
												   .define( "TEX_COORD" ) );
	gl::GlslProgRef rayScatter		= loadGlslProg( gl::GlslProg::Format().version( version )
												   .vertex( vertPassThrough ).fragment( fragRayScatter )
												   .define( "TEX_COORD" ) );
	gl::GlslProgRef stockColor		= gl::context()->getStockShader( gl::ShaderDef().color() );
	gl::GlslProgRef stockTexture	= gl::context()->getStockShader( gl::ShaderDef().texture( GL_TEXTURE_2D ) );
	
	// Unused in this sample - use this shader to draw a shadow caster without instancing
	gl::GlslProgRef shadowMap		= loadGlslProg( gl::GlslProg::Format().version( version )
												   .vertex( vertPassThrough ).fragment( fragDeferredShadowMap ) );
	
	// Create geometry as VBO meshes
	gl::VboMeshRef cylinder		= gl::VboMesh::create( geom::Cylinder().subdivisionsAxis( 5 ).subdivisionsHeight( 1 ) );
	gl::VboMeshRef cube			= gl::VboMesh::create( geom::Cube().size( vec3( 2.0f ) ) );
	gl::VboMeshRef icosahedron	= gl::VboMesh::create( geom::Icosahedron() );
	gl::VboMeshRef rect			= gl::VboMesh::create( geom::Rect() );
	gl::VboMeshRef sphere		= gl::VboMesh::create( geom::Sphere().subdivisions( 64 ) );
	gl::VboMeshRef sphereInst	= gl::VboMesh::create( geom::Sphere().subdivisions( 64 ) );
	gl::VboMeshRef sphereLow	= gl::VboMesh::create( geom::Sphere().subdivisions( 12 ) );
	
	// Create batches of VBO meshes and GLSL programs
	mBatchAoCompositeRect			= gl::Batch::create( rect,		aoComposite );
	mBatchBloomBlurRect				= gl::Batch::create( rect,		bloomBlur );
	mBatchBloomCompositeRect		= gl::Batch::create( rect,		bloomComposite );
	mBatchBloomHighpassRect			= gl::Batch::create( rect,		bloomHighpass );
	mBatchColorRect					= gl::Batch::create( rect,		postColor );
	mBatchDebugRect					= gl::Batch::create( rect,		debug );
	mBatchDofRect					= gl::Batch::create( rect,		postDof );
	mBatchFogRect					= gl::Batch::create( rect,		postFog );
	mBatchEmissiveRect				= gl::Batch::create( rect,		emissive );
	mBatchFxaaRect					= gl::Batch::create( rect,		postFxaa );
	mBatchGBufferCylinder			= gl::Batch::create( cylinder,	gBufferInvNorm );
	mBatchGBufferLightSourceSphere	= gl::Batch::create( sphere,	gBufferInstLS );
	mBatchHbaoAoRect				= gl::Batch::create( rect,		aoHbao );
	mBatchHbaoBlurRect				= gl::Batch::create( rect,		aoHbaoBlur );
	mBatchLBufferLightCube			= gl::Batch::create( cube,		lBufferLight );
	mBatchLBufferShadowRect			= gl::Batch::create( rect,		lBufferShadow );
	mBatchRayCompositeRect			= gl::Batch::create( rect,		rayComposite );
	mBatchRayOccludeRect			= gl::Batch::create( rect,		rayOcclude );
	mBatchRayScatterRect			= gl::Batch::create( rect,		rayScatter );
	mBatchSaoAoRect					= gl::Batch::create( rect,		aoSaoAo );
	mBatchSaoBlurRect				= gl::Batch::create( rect,		aoSaoBlur );
	mBatchSaoCszRect				= gl::Batch::create( rect,		aoSaoCsz );
	mBatchStockColorRect			= gl::Batch::create( rect,		stockColor );
	mBatchStockColorSphere			= gl::Batch::create( sphereLow,	stockColor );
	mBatchStockTextureRect			= gl::Batch::create( rect,		stockTexture );
	
	// Set up instancing for the shadow casters. Each instance has its own 
	// model matrix and normal matrix. These come through the shader as custom 
	// per-instance attributes. We use the Model class to represent these attributes, 
	// which makes them interleaved, and faster than using separate VBOs for 
	// each attribute.  
	geom::BufferLayout bufferLayout;
	size_t stride = sizeof( Model );
	bufferLayout.append( geom::Attrib::CUSTOM_0, 16, stride, 0, 1 );
	bufferLayout.append( geom::Attrib::CUSTOM_1, 9, stride, sizeof( mat4 ), 1 );
	vector<Model> spheres( mNumSpheres );
	vector<Model> icosahedrons( mNumIcosahedrons );
	mVboInstancedIcosahedrons	= gl::Vbo::create( GL_ARRAY_BUFFER, icosahedrons.size() * stride, icosahedrons.data(), GL_DYNAMIC_DRAW );
	mVboInstancedSpheres		= gl::Vbo::create( GL_ARRAY_BUFFER, spheres.size() * stride, spheres.data(), GL_DYNAMIC_DRAW );
	icosahedron->appendVbo( bufferLayout,	mVboInstancedIcosahedrons );
	sphereInst->appendVbo( bufferLayout,	mVboInstancedSpheres );
	
	// Create instanced batches
	mBatchGBufferIcosahedron		= gl::Batch::create( icosahedron, gBufferInst, {
		{ geom::Attrib::CUSTOM_0, "vInstanceModelMatrix" }, 
		{ geom::Attrib::CUSTOM_1, "vInstanceNormalMatrix" }
	} );
	mBatchGBufferSphere				= gl::Batch::create( sphereInst, gBufferInst, {
		{ geom::Attrib::CUSTOM_0, "vInstanceModelMatrix" },
		{ geom::Attrib::CUSTOM_1, "vInstanceNormalMatrix" }
	} );
	mBatchShadowMapIcosahedron		= gl::Batch::create( icosahedron, shadowMapInst, {
		{ geom::Attrib::CUSTOM_0, "vInstanceModelMatrix" },
		{ geom::Attrib::CUSTOM_1, "vInstanceNormalMatrix" }
	} );
	mBatchShadowMapSphere			= gl::Batch::create( sphereInst, shadowMapInst, {
		{ geom::Attrib::CUSTOM_0, "vInstanceModelMatrix" },
		{ geom::Attrib::CUSTOM_1, "vInstanceNormalMatrix" }
	} );
	
	// Set uniforms that don't need per-frame updates
	setUniforms();
}

void DeferredShadingAdvancedApp::draw()
{
	//////////////////////////////////////////////////////////////////////////////////////////////
	/* DEFERRED SHADING PIPELINE
	 *
	 * Deferred shading is a technique where a 3D scene's geometry data
	 * is rendered into screen space and shading is deferred until
	 * a second pass when lights are drawn.
	 *
	 * This scene is rendered into a frame buffer with multiple attachments
	 * (G-buffer). Uniform buffer objects are used to store a database of
	 * material and light data on the GPU; reducing drawing overhead.
	 * Shadow casters are rendered into a shadow map FBO. The buffers are
	 * read while drawing light volumes into the light buffer (L-buffer)
	 * to create the shaded scene. Then shadows are subtracted from the image.
	 *
	 * An ambient occlusion (AO) pass provides extra shading detail.
	 * Volumetric light scattering broadcasts rays from our primary light.
	 * Lights are accumulated to leave subtle trails, then bloomed to appear
	 * that they are glowing. We follow these with some post-processing
	 * passes, including depth of field to mimic camera focus, color tweaks,
	 * and anti-aliasing.
	 */
	
	const float f					= mCamera.getFarClip();
	const float n					= mCamera.getNearClip();
	const vec2 projectionParams		= vec2( f / ( f - n ), ( -f * n ) / ( f - n ) );
	const mat4 projMatrixInverse	= glm::inverse( mCamera.getProjectionMatrix() );
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	/* G-BUFFER
	 * 
	 * The geometry buffer, or G-buffer, captures our 3D scene's data in 2D screen space.
	 * A G-buffer can store pretty much anything you want. Position, normal, color, velocity, 
	 * material, luminance data. You name it. However, it's best to keep this information to 
	 * a minimum to improve performance. Our G-buffer stores depth, normals encoded to 8-bit 
	 * values in two channels, and material IDs. We also render everything with instancing to 
	 * keep draw calls to a minimum.
	 * 
	 * "unpack.glsl" contains methods for decoding normals and calculating 3D positions from 
	 * depth and camera data. The material ID represents the index of a material in our
	 * UBO. This allows models to access information for diffuse, specular, shininess, etc 
	 * values without having to store them in a texture.
	 */
	
	{
		const gl::ScopedFramebuffer scopedFrameBuffer( mFboGBuffer );
		const static GLenum buffers[] = {
			GL_COLOR_ATTACHMENT0,	// Albedo (color)
			GL_COLOR_ATTACHMENT1, 	// Encoded normal
			GL_COLOR_ATTACHMENT2 	// Material ID 
		};
		gl::drawBuffers( 3, buffers );
		const gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboGBuffer->getSize() );
		gl::clear();
		const gl::ScopedMatrices scopedMatrices;
		gl::setMatrices( mCamera );
		gl::enableDepthRead();
		gl::enableDepthWrite();
		
		////// BEGIN DRAW STUFF ////////////////////////////////////////////////
		
		// Draw room
		{
			const gl::ScopedFaceCulling scopedFaceCulling( true, GL_FRONT );
			const gl::ScopedModelMatrix scopedModelMatrix;
			gl::rotate( quat( vec3( 0.0f, (float)M_PI * 0.2f, 0.0f ) ) );
			gl::scale( vec3( 10.0f, 4.5f, 10.0f ) );
			mBatchGBufferCylinder->getGlslProg()->uniform( "uMaterialId", 1 );
			mBatchGBufferCylinder->draw();
		}
		
		// Draw shadow casters
		const gl::ScopedFaceCulling scopedFaceCulling( true, GL_BACK );
		mBatchGBufferSphere->getGlslProg()->uniform( "uMaterialId", 0 );
		mBatchGBufferIcosahedron->getGlslProg()->uniform( "uMaterialId", 0 );
		mBatchGBufferSphere->drawInstanced( mNumSpheres );
		mBatchGBufferIcosahedron->drawInstanced( mNumIcosahedrons );
		
		// Draw light sources
		mBatchGBufferLightSourceSphere->getGlslProg()->uniform( "uMaterialId", 2 );
		mBatchGBufferLightSourceSphere->drawInstanced( (GLsizei)mLights.size() );
		
		////// END DRAW STUFF //////////////////////////////////////////////////
		
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	/* SHADOW MAP
	 * 
	 * In order to get quality soft shadows, we have have to re-draw all shadow casters into a 
	 * shadow map. Instancing allows us to perform a second draw with very little penalty.
	 */

	// Draw shadow casters into framebuffer from view of shadow camera
	if ( mEnabledShadow ) {
		const gl::ScopedFramebuffer scopedFrameBuffer( mFboShadowMap );
		const gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboShadowMap->getSize() );
		const gl::ScopedMatrices scopedMatrices;
		gl::enableDepthRead();
		gl::enableDepthWrite();
		gl::clear();
		gl::setMatrices( mShadowCamera );
		
		mBatchShadowMapSphere->drawInstanced( mNumSpheres );
		mBatchShadowMapIcosahedron->drawInstanced( mNumIcosahedrons );
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	/* L-BUFFER
	 *
	 * In this pass, we draw light volumes as cubes. We are only using point lights in this 
	 * scene, which are represented spherically. We use cubes because they have simpler geometry 
	 * than spheres. As the lights are drawn, the screen position of each fragment is used to 
	 * read the G-buffer to calculate the shaded color.
	 * 
	 * After the light is rendered, we draw a large cube covering the scene to calculate shadows
	 * and subtract color. Using one large cube at the end gives us depth information and keeps 
	 * the overhead of implementing shadows low.
	 */
	
	size_t ping = 0;
	size_t pong = 1;
	
	{
		const gl::ScopedFramebuffer scopedFrameBuffer( mFboPingPong );
		const gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboPingPong->getSize() );
		{
			const static GLenum buffers[] = {
				GL_COLOR_ATTACHMENT0,
				GL_COLOR_ATTACHMENT1
			};
			gl::drawBuffers( 2, buffers );
			gl::clear();
		}
		
		gl::drawBuffer( GL_COLOR_ATTACHMENT0 + (GLenum)ping );
		gl::enableDepthRead();
		
		// Draw light volumes into L-buffer, reading G-buffer to perform shading
		{
			gl::enableDepthWrite();
			const gl::ScopedMatrices scopedMatrices;
			gl::setMatrices( mCamera );
			const gl::ScopedFaceCulling scopedFaceCulling( true, GL_FRONT );
			const gl::ScopedBlendAdditive scopedBlendAdditive;
			const gl::ScopedTextureBind scopedTextureBind0( mTextureFboGBuffer[ 0 ],		0 );
			const gl::ScopedTextureBind scopedTextureBind1( mTextureFboGBuffer[ 1 ],		1 );
			const gl::ScopedTextureBind scopedTextureBind2( mTextureFboGBuffer[ 2 ],		2 );
			const gl::ScopedTextureBind scopedTextureBind3( mFboGBuffer->getDepthTexture(),	3 );
			
			mBatchLBufferLightCube->getGlslProg()->uniform( "uProjMatrixInverse",	projMatrixInverse );
			mBatchLBufferLightCube->getGlslProg()->uniform( "uProjectionParams",	projectionParams );
			mBatchLBufferLightCube->getGlslProg()->uniform( "uViewMatrix",			mCamera.getViewMatrix() );
			mBatchLBufferLightCube->drawInstanced( (GLsizei)mLights.size() );
		}
		
		// Draw shadows onto L-buffer
		if ( mEnabledShadow ) {
			gl::disableDepthWrite();
			const gl::ScopedTextureBind scopedTextureBind0( mFboShadowMap->getDepthTexture(),	0 );
			const gl::ScopedTextureBind scopedTextureBind1( mFboGBuffer->getDepthTexture(),		1 );
			
			mBatchLBufferShadowRect->getGlslProg()->uniform( "uProjMatrixInverse",	projMatrixInverse );
			mBatchLBufferShadowRect->getGlslProg()->uniform( "uProjectionParams",	projectionParams );
			mBatchLBufferShadowRect->getGlslProg()->uniform( "uProjView",			mShadowCamera.getProjectionMatrix() * mShadowCamera.getViewMatrix() );
			mBatchLBufferShadowRect->getGlslProg()->uniform( "uViewMatrixInverse",	mCamera.getInverseViewMatrix() );
			
			const gl::ScopedBlendAlpha scopedBlendAlpha;
			const gl::ScopedModelMatrix scopedModelMatrix;
			gl::translate( getWindowSize() / 2 );
			gl::scale( getWindowSize() );
			mBatchLBufferShadowRect->draw();
		}
		
		ping = pong;
		pong = ( ping + 1 ) % 2;
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////
	/* LIGHT ACCUMULATION AND BLOOM
	 * 
	 * The bloom technique basically blurs any light sources in the image to make them appear that
	 * they are glowing. Light is drawn into an "accumulation" buffer. This allows us to separate 
	 * it from the rest of the image. If we want light trails, we tint the buffer first, which 
	 * contains the results of the last frame, instead of clearing it.
	 * 
	 * Anything that should be illuminated goes into this buffer. In this example, we render 
	 * light sources using the G-buffer and material data. If luminance textures were in use
	 * to draw light onto models, they would be rendered here. Next, we run a filter on the 
	 * L-buffer to find the brightest areas, which we will assume indicate reflected light. 
	 * The accumulation buffer is painted onto filter image, and then blurred. The results 
	 * of this process are later added on top of the final image.
	 * 
	 * The accumulation buffer is half the size of the window to improve performance. 
	 * While it can look great even at half size, it looks amazing if you have have the GPU 
	 * to pull off the full size window in real time. Try changing the size of mFboAccum in 
	 * ::resize() to see how works for you.
	 */
	
	{
		const gl::ScopedFramebuffer scopedFrameBuffer( mFboAccum );
		gl::drawBuffer( GL_COLOR_ATTACHMENT0 );
		const gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboAccum->getSize() );
		const gl::ScopedMatrices scopedMatrices;
		gl::setMatricesWindow( mFboAccum->getSize() );
		gl::disableDepthRead();
		gl::disableDepthWrite();
		gl::translate( mFboAccum->getSize() / 2 );
		gl::scale( mFboAccum->getSize() );
		
		// Dim the light accumulation buffer to produce trails. Lower alpha 
		// makes longer trails.
		{
			const gl::ScopedBlendAlpha scopedBlendAlpha;
			const gl::ScopedColor scopedColor( ColorAf( Colorf::black(), 0.43f ) );
			mBatchStockColorRect->draw();
		}
		
		// Paint light sources onto the accumulation buffer.
		{
			const gl::ScopedBlendAdditive scopedBlendAdditive;
			const gl::ScopedTextureBind scopedTextureBind0( mTextureFboGBuffer[ 0 ], 0 );
			const gl::ScopedTextureBind scopedTextureBind1( mTextureFboGBuffer[ 1 ], 1 );
			mBatchEmissiveRect->draw();
		}
		
		if ( mEnabledBloom ) {
			
			// First, we run a highpass filter on the L-buffer to draw out luminance.
			gl::drawBuffer( GL_COLOR_ATTACHMENT2 );
			{
				const gl::ScopedTextureBind scopedTextureBind( mTextureFboPingPong[ pong ], 0 );
				mBatchBloomHighpassRect->draw();
			}
			
			// Next, we add light from the accumulation buffer onto the filtered image.
			{
				const gl::ScopedBlendAdditive scopedBlendAdditive;
				const gl::ScopedTextureBind scopedTextureBind( mTextureFboAccum[ 0 ], 0 );
				mBatchStockTextureRect->draw();
			}
			
			// Run a horizontal blur pass
			{
				gl::drawBuffer( GL_COLOR_ATTACHMENT1 );
				mBatchBloomBlurRect->getGlslProg()->uniform( "uAxis", vec2( 1.0f, 0.0f ) );
				const gl::ScopedTextureBind scopedTextureBind( mTextureFboAccum[ 2 ], 0 );
				mBatchBloomBlurRect->draw();
			}
			
			// Run a vertical blur pass
			{
				gl::drawBuffer( GL_COLOR_ATTACHMENT2 );
				mBatchBloomBlurRect->getGlslProg()->uniform( "uAxis", vec2( 0.0f, 1.0f ) );
				const gl::ScopedTextureBind scopedTextureBind( mTextureFboAccum[ 1 ], 0 );
				mBatchBloomBlurRect->draw();
			}
		}
		
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	/* LIGHT RAYS (VOLUMETRIC LIGHT SCATTERING)
	 *
	 * Volumetric light scattering is a process which produces rays of light, simulating 
	 * atmospheric haze. First, we draw our primary light source into a depth buffer.
	 * Next, the G-buffer's depth is compared to the light source's depth to produce
	 * an image of the light with any occluders in front of it.
	 * 
	 * Then we perform the light scattering pass. We tell the shader where the light 
	 * is in screen space, and it will sample the image along a line between the light 
	 * position and each fragment; resulting in some very pretty streaks.
	 *
	 * This is a fairly expensive operation. If you are getting poor performance, turn it off 
	 * or reduce kNumSamples in scatter.frag.
	 */

	if ( mEnabledRay ) {
		
		// Get the primary light
		const Light& light = mLights.back();

		// Draw primary light into depth buffer
		{
			const gl::ScopedFramebuffer scopedFrameBuffer( mFboRayDepth );
			const gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboRayDepth->getSize() );
			gl::clear();
			gl::enableDepthRead();
			gl::enableDepthWrite();
			const gl::ScopedMatrices scopedMatrices;
			gl::setMatrices( mCamera );
			gl::translate( light.getPosition() );
			gl::scale( vec3( light.getRadius() ) );
			mBatchStockColorSphere->draw();
		}

		{
			const gl::ScopedFramebuffer scopedFrameBuffer( mFboRayColor );
			const gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboRayColor->getSize() );
			gl::clear();
			gl::enableDepthRead();
			gl::disableDepthWrite();

			// Draw light source into color buffer
			gl::drawBuffer( GL_COLOR_ATTACHMENT0 );
			{
				const gl::ScopedMatrices scopedMatrices;
				gl::setMatrices( mCamera );
				const gl::ScopedColor scopedColor( light.getColorDiffuse() );
				const gl::ScopedModelMatrix scopedModelMatrix;
				gl::translate( light.getPosition() );
				gl::scale( vec3( light.getRadius() ) );
				mBatchStockColorSphere->draw();
			}

			const gl::ScopedMatrices scopedMatrices;
			gl::setMatricesWindow( mFboRayColor->getSize() );
			gl::translate( mFboRayColor->getSize() / 2 );
			gl::scale( mFboRayColor->getSize() );

			// Draw occluders in front of light source by comparing 
			// scene depth with the light's depth
			{
				const gl::ScopedTextureBind scopedTextureBind0( mFboGBuffer->getDepthTexture(),		0 );
				const gl::ScopedTextureBind scopedTextureBind1( mFboRayDepth->getDepthTexture(),	1 );
				mBatchRayOccludeRect->draw();
			}

			// Perform light scattering
			{
				gl::drawBuffer( GL_COLOR_ATTACHMENT1 );

				// Calculate the primary light's position in screen space
				vec4 lightPosition = mCamera.getProjectionMatrix() * mCamera.getViewMatrix() * vec4( mLights.back().getPosition(), 1.0f );
				lightPosition /= lightPosition.w;

				const gl::ScopedTextureBind scopedTextureBind( mTextureFboRayColor[ 0 ], 0 );
				mBatchRayScatterRect->getGlslProg()->uniform( "uLightPosition", vec2( lightPosition ) * 0.5f + 0.5f );
				mBatchRayScatterRect->draw();
			}
		}
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	/* AMBIENT OCCLUSION
	 *
	 * Ambient occlusion is a technique used to approximate the effect of
	 * environment lighting by introducing soft, local shadows. This sample
	 * demonstrates two AO techniques which may be selected or turned off
	 * in the parameters UI. They each have their advantages. HBAO looks
	 * cleaner and is more portable. SAO has a nice, textured look and can
	 * cover a wider sampling radius to produce larger shadows at little cost.
	 * 
	 * Open the relevant shader files for links to papers on each technique.
	 */
	
	if ( mAo == Ao_Sao ) {
		
		// Convert depth to clip-space Z if we're performing SAO
		const gl::ScopedFramebuffer scopedFrameBuffer( mFboCsz );
		const gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboCsz->getSize() );
		gl::drawBuffer( GL_COLOR_ATTACHMENT0 );
		gl::clear();
		gl::disableDepthWrite();
		const gl::ScopedMatrices scopedMatrices;
		gl::setMatricesWindow( mFboCsz->getSize() );
		gl::translate( mFboCsz->getSize() / 2 );
		gl::scale( mFboCsz->getSize() );
		gl::enableDepthRead();
		
		const gl::ScopedTextureBind scopedTextureBind( mFboGBuffer->getDepthTexture(), 0 );
		mBatchSaoCszRect->getGlslProg()->uniform( "uNear", n );
		mBatchSaoCszRect->draw();
		
		gl::disableDepthRead();
	}
	
	{
		// Clear AO buffer whether we use it or not
		const gl::ScopedFramebuffer scopedFrameBuffer( mFboAo );
		const gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboAo->getSize() );
		gl::clear();
		
		if ( mAo != Ao_None ) {
			
			// Draw next pass into AO buffer's first attachment
			const gl::ScopedMatrices scopedMatrices;
			gl::setMatricesWindow( mFboAo->getSize() );
			gl::enableDepthRead();
			gl::disableDepthWrite();
			gl::translate( mFboAo->getSize() / 2 );
			gl::scale( mFboAo->getSize() );
			const gl::ScopedBlendPremult scopedBlendPremult;
			
			if ( mAo == Ao_Hbao ) {
				
				// HBAO (Horizon-based Ambient Occlusion)
				{
					const gl::ScopedTextureBind scopedTextureBind0( mFboGBuffer->getDepthTexture(), 0 );
					const gl::ScopedTextureBind scopedTextureBind1( mTextureFboGBuffer[ 2 ],		1 );
					mBatchHbaoAoRect->getGlslProg()->uniform( "uProjMatrixInverse",	projMatrixInverse );
					mBatchHbaoAoRect->getGlslProg()->uniform( "uProjectionParams",	projectionParams );
					mBatchHbaoAoRect->draw();
				}
				
				// Bilateral blur
				if ( mEnabledAoBlur ) {
					mBatchHbaoBlurRect->getGlslProg()->uniform( "uNear", n );
					{
						gl::drawBuffer( GL_COLOR_ATTACHMENT1 );
						const gl::ScopedTextureBind scopedTextureBind( mTextureFboAo[ 0 ], 0 );
						mBatchHbaoBlurRect->getGlslProg()->uniform( "uAxis", vec2( 1.0f, 0.0f ) );
						mBatchHbaoBlurRect->draw();
					}
					{
						gl::drawBuffer( GL_COLOR_ATTACHMENT0 );
						const gl::ScopedTextureBind scopedTextureBind( mTextureFboAo[ 1 ], 0 );
						mBatchHbaoBlurRect->getGlslProg()->uniform( "uAxis", vec2( 0.0f, 1.0f ) );
						mBatchHbaoBlurRect->draw();
					}
				}
				
			} else if ( mAo == Ao_Sao ) {
				
				// SAO (Scalable Ambient Obscurance)
				const gl::ScopedTextureBind scopedTextureBind( mFboCsz->getColorTexture(), 0 );
				const int32_t h	= mFboPingPong->getHeight();
				const int32_t w	= mFboPingPong->getWidth();
				const mat4& m	= mCamera.getProjectionMatrix();
				const vec4 p	= vec4( -2.0f / ( w * m[ 0 ][ 0 ] ),
									   -2.0f / ( h * m[ 1 ][ 1 ] ),
									   ( 1.0f - m[ 0 ][ 2 ] ) / m[ 0 ][ 0 ],
									   ( 1.0f + m[ 1 ][ 2 ] ) / m[ 1 ][ 1 ] );
				mBatchSaoAoRect->getGlslProg()->uniform( "uProj",		p );
				mBatchSaoAoRect->getGlslProg()->uniform( "uProjScale",	(float)h );
				mBatchSaoAoRect->draw();
			
				// Bilateral blur
				if ( mEnabledAoBlur ) {
					{
						gl::drawBuffer( GL_COLOR_ATTACHMENT1 );
						const gl::ScopedTextureBind scopedTextureBind( mTextureFboAo[ 0 ], 0 );
						mBatchSaoBlurRect->getGlslProg()->uniform( "uAxis", ivec2( 1, 0 ) );
						mBatchSaoBlurRect->draw();
					}
					{
						gl::drawBuffer( GL_COLOR_ATTACHMENT0 );
						const gl::ScopedTextureBind scopedTextureBind( mTextureFboAo[ 1 ], 0 );
						mBatchSaoBlurRect->getGlslProg()->uniform( "uAxis", ivec2( 0, 1 ) );
						mBatchSaoBlurRect->draw();
					}
				}
			}
		}
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	/* DEBUG VIEW
	 * 
	 * Displays buffer and material information.
	 * 
	 * ALBEDO		NORMAL			POSITION		DEPTH
	 * AMBIENT		DIFFUSE			EMISSIVE		SPECULAR
	 * SHININESS	MATERIAL ID		ACCUMULATION	AO
	 * RAY SOURCE	RAY SCATTERED
	 */
	 
	if ( mDrawDebug ) {
		const gl::ScopedFramebuffer scopedFramebuffer( mFboPingPong );
		gl::drawBuffer( GL_COLOR_ATTACHMENT0 + (GLenum)ping );
		const gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboPingPong->getSize() );
		const gl::ScopedMatrices scopedMatrices;
		gl::setMatricesWindow( mFboPingPong->getSize() );
		gl::disableDepthRead();
		gl::disableDepthWrite();
		
		const size_t columns = 4;
		
		vec2 sz;
		sz.x = (float)mFboPingPong->getWidth() / (float)columns;
		sz.y = sz.x / mFboPingPong->getAspectRatio();
		
		const gl::ScopedTextureBind scopedTextureBind0( mTextureFboGBuffer[ 0 ],					0 );
		const gl::ScopedTextureBind scopedTextureBind1( mTextureFboGBuffer[ 1 ],					1 );
		const gl::ScopedTextureBind scopedTextureBind2( mTextureFboGBuffer[ 2 ],					2 );
		const gl::ScopedTextureBind scopedTextureBind3( mFboGBuffer->getDepthTexture(),				3 );
		const gl::ScopedTextureBind scopedTextureBind4( mTextureFboAo[ 0 ],							4 );
		const gl::ScopedTextureBind scopedTextureBind5( mTextureFboAccum[ mEnabledBloom ? 2 : 0 ],	5 );
		if ( mTextureFboRayColor[ 1 ] ) {
			mTextureFboRayColor[ 1 ]->bind( 6 );
		}
		
		mBatchDebugRect->getGlslProg()->uniform( "uFar",				f );
		mBatchDebugRect->getGlslProg()->uniform( "uProjectionParams",	projectionParams );
		mBatchDebugRect->getGlslProg()->uniform( "uProjMatrixInverse",	projMatrixInverse );
		size_t count = mEnabledRay ? 12 : 11;
		for ( int32_t i = 0; i <= count; ++i ) {
			const gl::ScopedModelMatrix scopedModelMatrix;
			const vec2 pos( ( i % columns ) * sz.x, glm::floor( (float)i / (float)columns ) * sz.y );
			gl::translate( pos + sz * 0.5f );
			gl::scale( sz );
			mBatchDebugRect->getGlslProg()->uniform( "uMode", i );
			mBatchDebugRect->draw();
		}

		if ( mTextureFboRayColor[ 1 ] ) {
			mTextureFboRayColor[ 1 ]->unbind();
		}
	} else {
		{
			//////////////////////////////////////////////////////////////////////////////////////////////
			/* COMPOSITE
			 * 
			 * This first pass begins post-processing. That is, we actually start working on our final 
			 * image in screen space here. If we have AO enabled, it is applied to the L-buffer result.
			 * Otherwise, we'll just make a copy of the L-buffer and move on.
			 */
			
			const gl::ScopedFramebuffer scopedFrameBuffer( mFboPingPong );
			const gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboPingPong->getSize() );
			const gl::ScopedMatrices scopedMatrices;
			gl::setMatricesWindow( mFboPingPong->getSize() );
			gl::translate( mFboPingPong->getSize() / 2 );
			gl::scale( mFboPingPong->getSize() );
			gl::disableDepthRead();
			gl::disableDepthWrite();
			
			{
				gl::drawBuffer( GL_COLOR_ATTACHMENT0 + (GLenum)ping );
				if ( mAo != Ao_None ) {
					
					// Blend L-buffer and AO
					const gl::ScopedTextureBind scopedTextureBind1( mTextureFboPingPong[ pong ],	0 );
					const gl::ScopedTextureBind scopedTextureBind0( mTextureFboAo[ 0 ],				1 );
					mBatchAoCompositeRect->draw();
				} else {
					
					// Draw L-buffer without AO
					const gl::ScopedTextureBind scopedTextureBind( mTextureFboPingPong[ pong ], 0 );
					mBatchStockTextureRect->draw();
				}
				
				ping = pong;
				pong = ( ping + 1 ) % 2;
			}
			
			//////////////////////////////////////////////////////////////////////////////////////////////
			/* FOG
			 *
			 * To simulate fog, all we really have to do represent the depth buffer as color and mix
			 * it into our image.
			 */
			
			if ( mEnabledFog ) {
				gl::drawBuffer( GL_COLOR_ATTACHMENT0 + (GLenum)ping );
				const gl::ScopedTextureBind scopedTextureBind0( mFboGBuffer->getDepthTexture(),	0 );
				const gl::ScopedTextureBind scopedTextureBind1( mTextureFboPingPong[ pong ],	1 );
				mBatchFogRect->draw();
				
				ping = pong;
				pong = ( ping + 1 ) % 2;
			}
			
			//////////////////////////////////////////////////////////////////////////////////////////////
			/* DEPTH OF FIELD
			 * 
			 * Depth of field simulates a lens effect by performing a shaped blur (bokeh) on our
			 * image; based on its depth and distance from the camera. It helps to "unflatten" a 3D 
			 * image's appearance on a 2D screen. This is a fairly expensive operation. It really only 
			 * benefits a scene which has objects close to the camera that should be unfocused. 
			 * If this doesn't describe your scene, do not enable this pass.
			 */
			
			if ( mEnabledDoF ) {
				gl::drawBuffer( GL_COLOR_ATTACHMENT0 + (GLenum)ping );
				
				const float d = glm::length( mCamera.getEyePoint() );
				const gl::ScopedTextureBind scopedTextureBind0( mFboGBuffer->getDepthTexture(), 0 );
				const gl::ScopedTextureBind scopedTextureBind1( mTextureFboPingPong[ pong ],	1 );
				mBatchDofRect->getGlslProg()->uniform( "uFocalDepth",	d );
				mBatchDofRect->getGlslProg()->uniform( "uNear",			n );
				mBatchDofRect->draw();
				
				ping = pong;
				pong = ( ping + 1 ) % 2;
			}
			
			//////////////////////////////////////////////////////////////////////////////////////////////
			/* COLOR
			 * 
			 * This pass applies chromatic aberration, brightness, saturation, contrast, and intensity
			 * filtering. You may modify these settings in post/color.frag.
			 */
			
			if ( mEnabledColor ) {
				gl::drawBuffer( GL_COLOR_ATTACHMENT0 + (GLenum)ping );
				const gl::ScopedTextureBind scopedTextureBind( mTextureFboPingPong[ pong ], 0 );
				mBatchColorRect->draw();
				
				ping = pong;
				pong = ( ping + 1 ) % 2;
			}
		}
		
		//////////////////////////////////////////////////////////////////////////////////////////////
		/* FINAL RENDER
		 * 
		 * This pass prepares our image to be rendered to the screen. Light accumulation is painted
		 * onto the image. If we are in full screen AO mode, we'll prepare that view instead.
		 */
		
		const gl::ScopedFramebuffer scopedFramebuffer( mFboPingPong );
		gl::drawBuffer( GL_COLOR_ATTACHMENT0 + (GLenum)ping );
		const gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboPingPong->getSize() );
		const gl::ScopedMatrices scopedMatrices;
		gl::setMatricesWindow( mFboPingPong->getSize() );
		gl::translate( mFboPingPong->getSize() / 2 );
		gl::scale( mFboPingPong->getSize() );
		gl::disableDepthRead();
		gl::disableDepthWrite();
		
		// Fill screen with AO in AO view mode
		if ( mDrawAo ) {
			const gl::ScopedTextureBind scopedTextureBind( mTextureFboAo[ 0 ], 4 );
			mBatchDebugRect->getGlslProg()->uniform( "uMode", 11 );
			mBatchDebugRect->draw();
		} else {
			
			// Composite light rays into image
			if ( mEnabledRay ) {
				const gl::ScopedTextureBind scopedTextureBind0( mTextureFboPingPong[ pong ],	0 );
				const gl::ScopedTextureBind scopedTextureBind1( mTextureFboRayColor[ 1 ],		1 );
				mBatchRayCompositeRect->draw();

				ping = pong;
				pong = ( ping + 1 ) % 2;
			}

			// Composite light accumulation / bloom into our final image
			gl::drawBuffer( GL_COLOR_ATTACHMENT0 + (GLenum)ping );
			{
				const gl::ScopedTextureBind scopedTextureBind0( mTextureFboPingPong[ pong ],				0 );
				const gl::ScopedTextureBind scopedTextureBind1( mTextureFboAccum[ mEnabledBloom ? 2 : 0 ],	1 );
				mBatchBloomCompositeRect->draw();
			}
			
			// Draw light volumes for debugging
			if ( mDrawLightVolume ) {
				const gl::ScopedBlendAlpha scopedBlendAlpha;
				const gl::ScopedPolygonMode scopedPolygonMode( GL_LINE );
				const gl::ScopedMatrices scopedMatrices;
				gl::setMatrices( mCamera );
				
				for ( const Light& light : mLights ) {
					const gl::ScopedModelMatrix scopedModelMatrix;
					const gl::ScopedColor scopedColor( light.getColorDiffuse() * ColorAf( Colorf::white(), 0.08f ) );
					gl::translate( light.getPosition() );
					gl::scale( vec3( light.getVolume() ) );
					mBatchStockColorSphere->draw();
				}
			}
		}
	}
	
	ping = pong;
	pong = ( ping + 1 ) % 2;
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	// BLIT
	
	// Render our final image to the screen
	const gl::ScopedViewport scopedViewport( ivec2( 0 ), toPixels( getWindowSize() ) );
	const gl::ScopedMatrices scopedMatrices;
	gl::setMatricesWindow( toPixels( getWindowSize() ) );
	gl::translate( toPixels( getWindowSize() / 2 )  );
	gl::scale( toPixels( getWindowSize() ) );
	gl::disableDepthRead();
	gl::disableDepthWrite();
	const gl::ScopedTextureBind scopedTextureBind( mTextureFboPingPong[ pong ], 0 );
	if ( mEnabledFxaa ) {
		
		// To keep bandwidth in check, we aren't using any hardware 
		// anti-aliasing (MSAA). Instead, we use FXAA as a post-process 
		// to clean up our image.
		mBatchFxaaRect->draw();
	} else {
		
		// Draw to screen without FXAA
		mBatchStockTextureRect->draw();
	}
}

void DeferredShadingAdvancedApp::resize()
{
	// FBOs are created in the resize event handler so they always match the 
	// window's aspect ratio. For this reason, you must call resize() manually 
	// in your initialization to get things rolling.
	
	mCamera.setAspectRatio( getWindowAspectRatio() );
	mCamera.setFov( mAo != Ao_None ? 70.0f : 60.0f ); // Rough compensation for AO guard band
	
	// Choose window size based on selected quality
	ivec2 winSize	= toPixels( getWindowSize() );
	int32_t h		= winSize.y;
	int32_t w		= winSize.x;
	if ( !mHighQuality ) {
		h			/= 2;
		w			/= 2;
	}
	
	// If we are performing ambient occlusion, the G-buffer will need
	// to add 10% of the screen size to increase the sampling area.
	mOffset = mAo != Ao_None ? vec2( w, h ) * vec2( 0.1f ) : vec2( 0.0f );
	
	// Texture formats for color buffers
	GLuint colorInternalFormat = GL_RGB10_A2;
	gl::Texture2d::Format colorTextureFormatLinear = gl::Texture2d::Format()
		.internalFormat( colorInternalFormat )
		.magFilter( GL_LINEAR )
		.minFilter( GL_LINEAR )
		.wrap( GL_CLAMP_TO_EDGE )
		.dataType( GL_FLOAT );
	gl::Texture2d::Format colorTextureFormatNearest = gl::Texture2d::Format()
		.internalFormat( colorInternalFormat )
		.magFilter( GL_NEAREST )
		.minFilter( GL_NEAREST )
		.wrap( GL_CLAMP_TO_EDGE )
		.dataType( GL_FLOAT );
	
	// Texture format for depth buffers
	gl::Texture2d::Format depthTextureFormat = gl::Texture2d::Format()
		.internalFormat( GL_DEPTH_COMPONENT32F )
		.magFilter( GL_LINEAR )
		.minFilter( GL_LINEAR )
		.wrap( GL_CLAMP_TO_EDGE )
		.dataType( GL_FLOAT );
	
	// Light accumulation frame buffer
	// 0 GL_COLOR_ATTACHMENT0 Light accumulation
	// 1 GL_COLOR_ATTACHMENT1 Bloom ping
	// 2 GL_COLOR_ATTACHMENT2 Bloom pong
	{
		ivec2 sz = ivec2( w, h ) / 2;
		gl::Fbo::Format fboFormat;
		fboFormat.disableDepth();
		for ( size_t i = 0; i < 3; ++i ) {
			mTextureFboAccum[ i ] = gl::Texture2d::create( sz.x, sz.y, colorTextureFormatLinear );
			fboFormat.attachment( GL_COLOR_ATTACHMENT0 + (GLenum)i, mTextureFboAccum[ i ] );
		}
		mFboAccum = gl::Fbo::create( sz.x, sz.y, fboFormat );
		const gl::ScopedFramebuffer scopedFramebuffer( mFboAccum );
		const gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboAccum->getSize() );
		gl::clear();
	}
	
	// Set up the G-buffer
	// 0 GL_COLOR_ATTACHMENT0	Albedo
	// 1 GL_COLOR_ATTACHMENT1	Material ID
	// 2 GL_COLOR_ATTACHMENT2	Encoded normals
	{
		const ivec2 sz = ivec2( vec2( w, h ) + mOffset * 2.0f );
		for ( size_t i = 0; i < 3; ++i ) {
			mTextureFboGBuffer[ i ] = gl::Texture2d::create( sz.x, sz.y );
		}
		mTextureFboGBuffer[ 0 ] = gl::Texture2d::create( sz.x, sz.y, colorTextureFormatNearest );
		mTextureFboGBuffer[ 1 ] = gl::Texture2d::create( sz.x, sz.y,
														gl::Texture2d::Format()
														.internalFormat( GL_R8I )
														.magFilter( GL_NEAREST )
														.minFilter( GL_NEAREST )
														.wrap( GL_CLAMP_TO_EDGE )
														.dataType( GL_BYTE ) );
		mTextureFboGBuffer[ 2 ] = gl::Texture2d::create( sz.x, sz.y,
														gl::Texture2d::Format()
														.internalFormat( GL_RG16F )
														.magFilter( GL_NEAREST )
														.minFilter( GL_NEAREST )
														.wrap( GL_CLAMP_TO_EDGE )
														.dataType( GL_BYTE ) );
		gl::Fbo::Format fboFormat;
		fboFormat.depthTexture( depthTextureFormat );
		for ( size_t i = 0; i < 3; ++i ) {
			fboFormat.attachment( GL_COLOR_ATTACHMENT0 + (GLenum)i, mTextureFboGBuffer[ i ] );
		}
		mFboGBuffer = gl::Fbo::create( sz.x, sz.y, fboFormat );
		const gl::ScopedFramebuffer scopedFramebuffer( mFboGBuffer );
		const gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboGBuffer->getSize() );
		gl::clear();
	}
	
	// Set up the ambient occlusion frame buffer with two attachments to ping-pong.
	if ( mAo != Ao_None ) {
		ivec2 sz = mFboGBuffer->getSize() / 2;
		gl::Fbo::Format fboFormat;
		fboFormat.disableDepth();
		for ( size_t i = 0; i < 2; ++i ) {
			mTextureFboAo[ i ] = gl::Texture2d::create( sz.x, sz.y, gl::Texture2d::Format()
													   .internalFormat( GL_RG32F )
													   .magFilter( GL_LINEAR )
													   .minFilter( GL_LINEAR )
													   .wrap( GL_CLAMP_TO_EDGE )
													   .dataType( GL_FLOAT ) );
			fboFormat.attachment( GL_COLOR_ATTACHMENT0 + (GLenum)i, mTextureFboAo[ i ] );
		}
		mFboAo = gl::Fbo::create( sz.x, sz.y, fboFormat );
		{
			const gl::ScopedFramebuffer scopedFramebuffer( mFboAo );
			const gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboAo->getSize() );
			gl::clear();
		}
		
		// Set up the SAO mip-map (clip-space Z) buffer
		if ( mAo == Ao_Sao ) {
			gl::Texture2d::Format cszTextureFormat = gl::Texture2d::Format()
				.internalFormat( GL_R32F )
				.mipmap()
				.magFilter( GL_NEAREST_MIPMAP_NEAREST )
				.minFilter( GL_NEAREST_MIPMAP_NEAREST )
				.wrap( GL_CLAMP_TO_EDGE )
				.dataType( GL_FLOAT );
			cszTextureFormat.setMaxMipmapLevel( mMipmapLevels );
			mFboCsz = gl::Fbo::create( mFboGBuffer->getWidth(), mFboGBuffer->getHeight(),
									  gl::Fbo::Format().disableDepth().colorTexture( cszTextureFormat ) );
			const gl::ScopedFramebuffer scopedFramebuffer( mFboCsz );
			const gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboCsz->getSize() );
			gl::clear();
		}
	}
	
	// Set up the ping pong frame buffer. We'll use this FBO to render
	// the scene and perform post-processing passes.
	{
		gl::Fbo::Format fboFormat;
		fboFormat.disableDepth();
		for ( size_t i = 0; i < 2; ++i ) {
			mTextureFboPingPong[ i ] = gl::Texture2d::create( w, h, colorTextureFormatNearest );
			fboFormat.attachment( GL_COLOR_ATTACHMENT0 + (GLenum)i, mTextureFboPingPong[ i ] );
		}
		mFboPingPong = gl::Fbo::create( w, h, fboFormat );
		const gl::ScopedFramebuffer scopedFramebuffer( mFboPingPong );
		const gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboPingPong->getSize() );
		gl::clear();
	}

	// Create FBOs for light rays (volumetric light scattering)
	if ( mEnabledRay ) {
		{
			gl::Fbo::Format fboFormat;
			fboFormat.disableDepth();
			ivec2 sz( w / 2, h / 2 );
			for ( size_t i = 0; i < 2; ++i ) {
				mTextureFboRayColor[ i ] = gl::Texture2d::create( sz.x, sz.y, colorTextureFormatLinear );
				fboFormat.attachment( GL_COLOR_ATTACHMENT0 + (GLenum)i, mTextureFboRayColor[ i ] );
			}
			mFboRayColor = gl::Fbo::create( sz.x, sz.y, fboFormat );
			const gl::ScopedFramebuffer scopedFramebuffer( mFboRayColor );
			const gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboRayColor->getSize() );
			gl::clear();
		}
		{
			ivec2 sz		= mFboGBuffer->getSize() / 2;
			mFboRayDepth	= gl::Fbo::create( sz.x, sz.y, 
											  gl::Fbo::Format().disableColor().depthTexture( depthTextureFormat ) );
			const gl::ScopedFramebuffer scopedFramebuffer( mFboRayDepth );
			const gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboRayDepth->getSize() );
			gl::clear();
		}
	}
	
	// Create shadow map buffer
	{
		int32_t sz = (int32_t)toPixels( mHighQuality ? 2048.0f : 1024.0f );
		mFboShadowMap = gl::Fbo::create( sz, sz,
										gl::Fbo::Format().depthTexture( depthTextureFormat ) );
		mFboShadowMap->getDepthTexture()->setCompareMode( GL_COMPARE_REF_TO_TEXTURE );
		const gl::ScopedFramebuffer scopedFramebuffer( mFboShadowMap );
		const gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboShadowMap->getSize() );
		gl::clear();
	}
	
	// Set up shadow camera
	mShadowCamera.setPerspective( 120.0f, mFboShadowMap->getAspectRatio(),
								 mCamera.getNearClip(),
								 mCamera.getFarClip() );
	mShadowCamera.lookAt( vec3( 0.0f, 7.0f, 0.0f ), vec3( 0.0f ) );
	
	// Update uniforms
	setUniforms();
}

void DeferredShadingAdvancedApp::screenShot()
{
	writeImage( getAppPath() / fs::path( "frame" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
}

void DeferredShadingAdvancedApp::setUniforms()
{
	// Set sampler bindings
	mBatchAoCompositeRect->getGlslProg()->uniform(		"uSampler",				0 );
	mBatchAoCompositeRect->getGlslProg()->uniform(		"uSamplerAo",			1 );
	mBatchBloomBlurRect->getGlslProg()->uniform(		"uSampler",				0 );
	mBatchBloomCompositeRect->getGlslProg()->uniform(	"uSamplerColor",		0 );
	mBatchBloomCompositeRect->getGlslProg()->uniform(	"uSamplerBloom",		1 );
	mBatchBloomHighpassRect->getGlslProg()->uniform(	"uSampler",				0 );
	mBatchColorRect->getGlslProg()->uniform(			"uSampler",				0 );
	mBatchDebugRect->getGlslProg()->uniform(			"uSamplerAlbedo",		0 );
	mBatchDebugRect->getGlslProg()->uniform(			"uSamplerMaterial",		1 );
	mBatchDebugRect->getGlslProg()->uniform(			"uSamplerNormal",		2 );
	mBatchDebugRect->getGlslProg()->uniform(			"uSamplerDepth",		3 );
	mBatchDebugRect->getGlslProg()->uniform(			"uSamplerAo",			4 );
	mBatchDebugRect->getGlslProg()->uniform(			"uSamplerAccum",		5 );
	mBatchDebugRect->getGlslProg()->uniform(			"uSamplerRay",			6 );
	mBatchDofRect->getGlslProg()->uniform(				"uSamplerDepth",		0 );
	mBatchDofRect->getGlslProg()->uniform(				"uSamplerColor",		1 );
	mBatchFogRect->getGlslProg()->uniform(				"uSamplerDepth",		0 );
	mBatchFogRect->getGlslProg()->uniform(				"uSamplerColor",		1 );
	mBatchEmissiveRect->getGlslProg()->uniform(			"uSamplerAlbedo",		0 );
	mBatchEmissiveRect->getGlslProg()->uniform(			"uSamplerMaterial",		1 );
	mBatchFxaaRect->getGlslProg()->uniform(				"uSampler",				0 );
	mBatchHbaoAoRect->getGlslProg()->uniform(			"uSamplerDepth",		0 );
	mBatchHbaoAoRect->getGlslProg()->uniform(			"uSamplerNormal",		1 );
	mBatchHbaoBlurRect->getGlslProg()->uniform(			"uSampler",				0 );
	mBatchLBufferLightCube->getGlslProg()->uniform(		"uSamplerAlbedo",		0 );
	mBatchLBufferLightCube->getGlslProg()->uniform(		"uSamplerMaterial",		1 );
	mBatchLBufferLightCube->getGlslProg()->uniform(		"uSamplerNormal",		2 );
	mBatchLBufferLightCube->getGlslProg()->uniform(		"uSamplerDepth",		3 );
	mBatchLBufferShadowRect->getGlslProg()->uniform(	"uSampler",				0 );
	mBatchLBufferShadowRect->getGlslProg()->uniform(	"uSamplerDepth",		1 );
	mBatchRayCompositeRect->getGlslProg()->uniform(		"uSamplerColor",		0 );
	mBatchRayCompositeRect->getGlslProg()->uniform(		"uSamplerRay",			1 );
	mBatchRayOccludeRect->getGlslProg()->uniform(		"uSamplerDepth",		0 );
	mBatchRayOccludeRect->getGlslProg()->uniform(		"uSamplerLightDepth",	1 );
	mBatchRayScatterRect->getGlslProg()->uniform(		"uSampler",				0 );
	mBatchSaoAoRect->getGlslProg()->uniform(			"uSampler",				0 );
	mBatchSaoBlurRect->getGlslProg()->uniform(			"uSampler",				0 );
	mBatchSaoCszRect->getGlslProg()->uniform(			"uSamplerDepth",		0 );
	
	// Bind uniform buffer blocks to shaders
	mBatchDebugRect->getGlslProg()->uniformBlock(					"Materials",	1 );
	mBatchEmissiveRect->getGlslProg()->uniformBlock(				"Materials",	1 );
	mBatchGBufferLightSourceSphere->getGlslProg()->uniformBlock(	"Lights",		0 );
	mBatchLBufferLightCube->getGlslProg()->uniformBlock(			"Lights",		0 );
	mBatchLBufferLightCube->getGlslProg()->uniformBlock(			"Materials",	1 );
	
	// Set uniforms which need to know about screen dimensions
	const vec2 szGBuffer	= mFboGBuffer	? mFboGBuffer->getSize()	: toPixels( getWindowSize() );
	const vec2 szPingPong	= mFboPingPong	? mFboPingPong->getSize()	: toPixels( getWindowSize() );
	const vec2 szRay		= mFboRayColor	? mFboRayColor->getSize()	: toPixels( getWindowSize() / 2 );
	mBatchAoCompositeRect->getGlslProg()->uniform(		"uOffset",		mOffset );
	mBatchAoCompositeRect->getGlslProg()->uniform(		"uWindowSize",	szGBuffer );
	mBatchBloomCompositeRect->getGlslProg()->uniform(	"uPixel",		vec2( 1.0f ) / vec2( szPingPong ) );
	mBatchDofRect->getGlslProg()->uniform(				"uAspect",		getWindowAspectRatio() );
	mBatchDofRect->getGlslProg()->uniform(				"uOffset",		mOffset );
	mBatchDofRect->getGlslProg()->uniform(				"uWindowSize",	szGBuffer );
	mBatchFogRect->getGlslProg()->uniform(				"uOffset",		mOffset );
	mBatchFogRect->getGlslProg()->uniform(				"uWindowSize",	szGBuffer );
	mBatchEmissiveRect->getGlslProg()->uniform(			"uOffset",		mOffset );
	mBatchEmissiveRect->getGlslProg()->uniform(			"uWindowSize",	szGBuffer );
	mBatchFxaaRect->getGlslProg()->uniform(				"uPixel",		1.0f / vec2( szPingPong ) );
	mBatchLBufferLightCube->getGlslProg()->uniform(		"uOffset",		mOffset );
	mBatchLBufferLightCube->getGlslProg()->uniform(		"uWindowSize",	szGBuffer );
	mBatchLBufferShadowRect->getGlslProg()->uniform(	"uOffset",		mOffset );
	mBatchLBufferShadowRect->getGlslProg()->uniform(	"uWindowSize",	szGBuffer );
	mBatchRayCompositeRect->getGlslProg()->uniform(		"uPixel",		vec2( 1.0f ) / vec2( szRay ) );
	mBatchRayScatterRect->getGlslProg()->uniform(		"uOffset",		mOffset * (  szRay / szGBuffer ) );
	mBatchRayScatterRect->getGlslProg()->uniform(		"uWindowSize",	szRay );
}


void DeferredShadingAdvancedApp::updateGui()
{
	static vector<string> ao = { "None", "HBAO", "SAO" };
	ImGui::Begin( "Params" );
	ImGui::Text( "Frame rate %f", mFrameRate );
	ImGui::Checkbox( "Fullscreen", &mFullScreen );
	if( ImGui::Button( "Load shaders" ) ) createBatches();
	ImGui::SameLine();
	if( ImGui::Button( "Screen shot" ) ) screenShot();
	ImGui::SameLine();
	if( ImGui::Button( "Quit" ) ) quit();
	ImGui::Separator();
	if( ImGui::CollapsingHeader( "Draw", ImGuiTreeNodeFlags_DefaultOpen ) ) {
		ImGui::Checkbox( "AO view", &mDrawAo );
		ImGui::Checkbox( "Debug view", &mDrawDebug );
		ImGui::Checkbox( "High quality", &mHighQuality );
		ImGui::Checkbox( "Light volume", &mDrawLightVolume );
		ImGui::Checkbox( "Pause", &mPaused );
	}
	ImGui::Separator();
	if( ImGui::CollapsingHeader( "Pass", ImGuiTreeNodeFlags_DefaultOpen ) ) {
		ImGui::Combo( "AO", (int*)&mAo, ao );
		ImGui::Checkbox( "AO blur", &mEnabledAoBlur );
		ImGui::Checkbox( "Bloom", &mEnabledBloom );
		ImGui::Checkbox( "Color", &mEnabledColor );
		ImGui::Checkbox( "Depth of field", &mEnabledDoF );
		ImGui::Checkbox( "Fog", &mEnabledFog );
		ImGui::Checkbox( "FXAA", &mEnabledFxaa );
		ImGui::Checkbox( "Rays", &mEnabledRay );
		ImGui::Checkbox( "Shadows", &mEnabledShadow );
	}
	ImGui::End();
}

void DeferredShadingAdvancedApp::update()
{
	float e		= (float)getElapsedSeconds();
	mFrameRate	= getAverageFps();

	updateGui();

	if ( mFullScreen != isFullScreen() ) {
		setFullScreen( mFullScreen );
	}
	
	// Call resize to rebuild buffers when render quality
	// or AO method changes
	if ( mAoPrev			!= mAo			||
		mEnabledRayPrev		!= mEnabledRay	||
		mHighQualityPrev	!= mHighQuality ) {
		resize();
		mAoPrev				= mAo;
		mEnabledRayPrev		= mEnabledRay;
		mHighQualityPrev	= mHighQuality;
	}

	// Update icosahedrons
	{
		Model* icosahedron		= (Model*)mVboInstancedIcosahedrons->map( GL_READ_WRITE );
		float t					= e * 0.165f;
		const float d			= ( (float)M_PI * 2.0f ) / (float)mNumIcosahedrons;
		const float r			= 4.5f;
		for ( int32_t i = 0; i < mNumIcosahedrons; ++i, t += d ) {
			mat4 m( 1.0f );
			if ( mPaused ) {
				m				= icosahedron->getModelMatrix();
			} else {
				const float x	= glm::cos( t );
				const float z	= glm::sin( t );
				vec3 p			= vec3( x, 0.0f, z ) * r;
				p.y				= 0.625f;
				m				= glm::translate( m, p );
				m				= glm::rotate( m, e + t, vec3( 1.0f ) );
				m				= glm::scale( m, vec3( 0.5f ) );
				icosahedron->setModelMatrix( m );
			}
			icosahedron->setNormalMatrix( glm::inverseTranspose( mat3( mCamera.getViewMatrix() * m ) ) );
			++icosahedron;
		}
		mVboInstancedIcosahedrons->unmap();
	}
	
	// Update spheres
	const float scale			= 0.85f;
	if ( !mPaused ) {
		const float ground		= scale;
		const float dampen		= 0.092f;
		mSpherePosition.y		+= mSphereVelocity;
		if ( mSphereVelocity > 0.0f ) {
			mSphereVelocity		*= ( 1.0f - dampen );
		} else if ( mSphereVelocity < 0.0f ) {
			mSphereVelocity		*= ( 1.0f + dampen );
		}
		if ( mSpherePosition.y < ground ) {
			mSpherePosition.y	= ground;
			mSphereVelocity		= -mSphereVelocity;
		} else if ( mSphereVelocity > 0.0f && mSphereVelocity < 0.02f ) {
			mSphereVelocity		= -0.01f;
		}
	}
	{
		Model* sphere = (Model*)mVboInstancedSpheres->map( GL_READ_WRITE );
		float t					= e * -0.21f;
		const float d			= ( (float)M_PI * 2.0f ) / (float)mNumSpheres;
		const float r			= 1.75f;
		for ( int32_t i = 0; i < mNumSpheres; ++i, t += d ) {
			mat4 m( 1.0f );
			if ( mPaused ) {
				m				= sphere->getModelMatrix();
			} else {
				const float x	= glm::cos( t );
				const float z	= glm::sin( t );
				vec3 p			= vec3( x, 0.0f, z ) * r + mSpherePosition;
				m				= glm::translate( m, p );
				m				= glm::rotate( m, e + t, vec3( 1.0f ) );
				m				= glm::scale( m, vec3( scale ) );
				sphere->setModelMatrix( m );
			}
			sphere->setNormalMatrix( glm::inverseTranspose( mat3( mCamera.getViewMatrix() * m ) ) );
			++sphere;
		}
		mVboInstancedSpheres->unmap();
	}
	
	// Update light positions
	if ( !mPaused ) {
		float t				= e;
		const float d		= ( (float)M_PI * 2.0f ) / (float)mNumBouncingLights;
		const float r		= 3.5f;
		for ( int32_t i = 0; i < mNumBouncingLights; ++i, t += d ) {
			const float y	= 0.1f;
			const float x	= glm::cos( t );
			const float z	= glm::sin( t );
			vec3 p			= vec3( x, 0.0f, z ) * r;
			p.y				= y + glm::sin( t + e * (float)M_PI ) * 2.0f;
			if ( p.y < y ) {
				p.y			+= ( y - p.y ) * 2.0f;
			}
			mLights.at( i ).setPosition( p );
		}
		t					= e * 0.333f;
		vec3 p				= vec3( glm::sin( t ), 0.0f, glm::cos( t ) ) * 3.0f ;
		p.y					= 7.5f;
		mLights.back().setPosition( p );
		
		// Update light positions in UBO
		Light* lights = (Light*)mUboLight->mapWriteOnly();
		for ( const Light& light : mLights ) {
			lights->setPosition( light.getPosition() );
			++lights;
		}
		mUboLight->unmap();
	
		// Update shadow camera
		mShadowCamera.setEyePoint( mLights.back().getPosition() + vec3( 0.0f, 3.0f, 0.0f ) );
	}
}

CINDER_APP( DeferredShadingAdvancedApp, RendererGl( RendererGl::Options().version( 3, 3 ) ),
		   []( App::Settings* settings )
{
	settings->disableFrameRate();
	settings->setHighDensityDisplayEnabled( true );
	settings->setWindowSize( 1280, 720 );
} )
