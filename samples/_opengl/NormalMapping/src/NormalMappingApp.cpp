/*
 Copyright (c) 2014, Paul Houx - All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Leprechaun 3D model courtesy of Fabiano Di Liso aka Nazedo
(c) Fabiano Di Liso - All rights reserved - Used with permission.
http://www.cgtrader.com/3d-models/character-people/fantasy/the-leprechaun-the-goblin

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CinderImGui.h"
#include "cinder/Camera.h"
#include "cinder/ImageIo.h"
#include "cinder/CameraUi.h"
#include "cinder/Perlin.h"
#include "cinder/Timeline.h"
#include "cinder/Timer.h"
#include "cinder/TriMesh.h"
#include "cinder/Log.h"

#if defined( CINDER_ANDROID )
  #include "cinder/android/CinderAndroid.h"
  #include "cinder/app/android/AssetFileSystem.h"
#endif



#include "DebugMesh.h"

using namespace ci;
using namespace ci::app;
using namespace std;

struct LightSource
{
	vec4 position;
	ColorA diffuse;
	ColorA specular;
};

static void prepareSettings( App::Settings *settings )
{
	settings->disableFrameRate();

	settings->setWindowSize( 1024, 768 );
	settings->setTitle( "OpenGL Core Profile - Normal Mapping Demo" );
}

class NormalMappingApp : public App {

#if ! defined( CINDER_GL_ES )
	typedef enum { Default, Glossy, Normals, Lighting, Mesh } ViewMode;
#else
	typedef enum { Default, Glossy, Normals, Lighting } ViewMode;
#endif

public:
	void	setup();

	void	update();
	void	draw();

	void	keyDown( KeyEvent event );

	bool	isInitialized() const {
		return ( mDiffuseMap && mSpecularMap && mNormalMap && mCopyrightMap && mShaderNormalMapping && mMesh );
	}

private:
	/* Load the mesh and calculate normals and tangents if necessary. */
	TriMesh			createMesh( const fs::path& mshFile );
	/* Construct a mesh to visualize normals (blue), tangents (red) and bitangents (green). */
	gl::VboMeshRef	createDebugMesh( const TriMesh& mesh );

private:
	ViewMode			mViewMode;

	mat4				mMeshTransform;
	AxisAlignedBox	mMeshBounds;

	CameraPersp			mCamera;
	CameraUi			mCamUi;

	LightSource			mLightLantern;
	LightSource			mLightAmbient;

	gl::TextureRef		mCopyrightMap;
	gl::TextureRef		mDiffuseMap;
	gl::TextureRef		mSpecularMap;
	gl::TextureRef		mNormalMap;
	gl::TextureRef		mEmissiveMap;

	gl::GlslProgRef		mShaderNormalMapping;
	gl::GlslProgRef		mShaderWireframe;

	gl::VboMeshRef		mMesh;
	gl::VboMeshRef		mMeshDebug;

	bool				mAutoRotate;
	float				mAutoRotateAngle;

	bool				mAnimateLantern;
	Perlin				mPerlin;

	bool				mEnableNormalMap;
	bool				mShowNormalsAndTangents;

	float				mTime;
};

void NormalMappingApp::setup()
{
	// setup camera and lights
	mCamera.lookAt( vec3( 0.2f, 0.4f, 1.0f ), vec3(0.0f, 0.425f, 0.0f) );
	mCamera.setNearClip( 0.01f );
	mCamera.setFarClip( 100.0f );

	mLightLantern.diffuse = Color(0.9f, 0.6f, 0.3f);
	mLightLantern.specular = Color(0.9f, 0.6f, 0.3f);

	mLightAmbient.diffuse = Color(0.2f, 0.6f, 1.0f);
	mLightAmbient.specular = Color(0.05f, 0.15f, 0.25f);

	// setup perlin noise to easily animate our lantern light source
	mPerlin = Perlin(4, 65535);

	// default settings
	mMeshBounds = AxisAlignedBox( vec3( 0 ), vec3( 1 ) );

	mAutoRotate = true;
	mAutoRotateAngle = 0.0f;

	mAnimateLantern = true;

	mEnableNormalMap = true;
	mShowNormalsAndTangents = false;

	mViewMode = ViewMode::Default;

	// load assets
	try {
		// load our copyright message
		mCopyrightMap  = gl::Texture::create( loadImage( loadAsset("copyright.png") ) );

		// load textures
		auto texFormat = gl::Texture2d::Format().loadTopDown();
		mDiffuseMap = gl::Texture::create( loadImage( loadAsset("leprechaun_diffuse.jpg") ), texFormat );
		mSpecularMap = gl::Texture::create( loadImage( loadAsset("leprechaun_specular.jpg") ), texFormat );
		mNormalMap = gl::Texture::create( loadImage( loadAsset("leprechaun_normal.jpg") ), texFormat );
		mEmissiveMap = gl::Texture::create( loadImage( loadAsset("leprechaun_emmisive.png") ), texFormat );

		// load our shaders and set the non-varying uniforms
#if ! defined( CINDER_GL_ES )
		mShaderNormalMapping = gl::GlslProg::create( loadAsset("normal_mapping_vert.glsl"), loadAsset("normal_mapping_frag.glsl") );

		gl::GlslProg::Format fmt;
		fmt.vertex( loadAsset("wireframe_vert.glsl") );
		fmt.geometry( loadAsset("wireframe_geom.glsl") );
		fmt.fragment( loadAsset("wireframe_frag.glsl") );
		mShaderWireframe = gl::GlslProg::create( fmt );
#else
		mShaderNormalMapping = gl::GlslProg::create( loadAsset("normal_mapping_vert_es2.glsl"), loadAsset("normal_mapping_frag_es2.glsl") );
#endif

		mShaderNormalMapping->uniform( "uDiffuseMap", 0 );
		mShaderNormalMapping->uniform( "uSpecularMap", 1 );
		mShaderNormalMapping->uniform( "uNormalMap", 2 );
		mShaderNormalMapping->uniform( "uEmmisiveMap", 3 );
		mShaderNormalMapping->uniform( "uLights[0].diffuse", mLightLantern.diffuse );
		mShaderNormalMapping->uniform( "uLights[0].specular", mLightLantern.specular );
		mShaderNormalMapping->uniform( "uLights[1].diffuse", mLightAmbient.diffuse );
		mShaderNormalMapping->uniform( "uLights[1].specular", mLightAmbient.specular );
		mShaderNormalMapping->uniform( "uNumOfLights", 2 );
	}
	catch( const std::exception &e ) {
		CI_LOG_EXCEPTION( "Error loading asset", e );
		quit();
	}

#if defined( CINDER_ANDROID )
ci::app::android::AssetFileSystem_FILE* asset = ci::app::android::AssetFileSystem_fopen( "leprechaun.msh", 0 );
console() << "Asset size: " << ci::app::android::AssetFileSystem_flength( asset ) << std::endl;
#endif

	// load mesh file and create missing data (normals, tangents) if necessary
	try {
		fs::path mshFile = getAssetPath( "leprechaun.msh" );
		TriMesh mesh = createMesh( mshFile );

		mMesh = gl::VboMesh::create( mesh );
		mMeshBounds = mesh.calcBoundingBox();

		mMeshDebug = createDebugMesh(mesh);
	}
	catch( const std::exception& e ) {
		CI_LOG_EXCEPTION( "Error loading asset", e );
		quit();
	}

#if ! defined( CINDER_GL_ES )
	ImGui::Initialize();
#endif

	mCamUi = CameraUi( &mCamera, getWindow(), -1 );

	// keep track of time
	mTime = (float) getElapsedSeconds();
}

void NormalMappingApp::update()
{
#if ! defined( CINDER_GL_ES )
	static std::vector<std::string> viewmodes;
	viewmodes.push_back( "Final" );
	viewmodes.push_back( "Glossy" );
	viewmodes.push_back( "Normals" );
	viewmodes.push_back( "Lighting" );
	viewmodes.push_back( "Mesh" );
	ImGui::Begin( "Normal Mapping Demo" );
	ImGui::Checkbox( "Enable Normal Mapping", &mEnableNormalMap );
	ImGui::Combo( "Viewing Mode", (int*)&mViewMode, viewmodes );
	ImGui::Separator();
	ImGui::Checkbox( "Rotate Model", &mAutoRotate );
	ImGui::Checkbox( "Animate Light", &mAnimateLantern );
	ImGui::Checkbox( "Show Normals & Tangents", &mShowNormalsAndTangents );
	ImGui::End();
#endif
	// keep track of time
	float fElapsed = (float)getElapsedSeconds() - mTime;
	mTime += fElapsed;
	
	// rotate the mesh
	if( mAutoRotate )
		mAutoRotateAngle += (fElapsed * 0.2f);

	mMeshTransform = mat4();
	mMeshTransform *= rotate( mAutoRotateAngle, vec3( 0, 1, 0 ) );
	mMeshTransform *= scale( vec3( 1 ) / mMeshBounds.getSize().y );

	// position our lights (in eye space)
	vec3 lanternPositionOS = vec3(12.5f, 30.0f, 12.5f);
	if( mAnimateLantern )
		lanternPositionOS += mPerlin.dfBm( vec3( 0.0f, 0.0f, mTime ) ) * 5.0f;

	vec4 lanternPositionWS = mMeshTransform * vec4( lanternPositionOS, 1 );
	mLightLantern.position = mCamera.getViewMatrix() * lanternPositionWS;

	mLightAmbient.position = vec4( 0 );

	// set the varying shader uniforms
	mShaderNormalMapping->uniform( "bShowNormals", mViewMode == ViewMode::Normals );
	mShaderNormalMapping->uniform( "bUseDiffuseMap", ( mViewMode == ViewMode::Default || mViewMode == ViewMode::Glossy ) );
	mShaderNormalMapping->uniform( "bUseSpecularMap", ( mViewMode == ViewMode::Default || mViewMode == ViewMode::Lighting ) );
	mShaderNormalMapping->uniform( "bUseEmmisiveMap", ( mViewMode == ViewMode::Default || mViewMode == ViewMode::Glossy ) );
	mShaderNormalMapping->uniform( "bUseNormalMap", mEnableNormalMap );

	mShaderNormalMapping->uniform( "uLights[0].position", mLightLantern.position );
	mShaderNormalMapping->uniform( "uLights[1].position", mLightAmbient.position );

#if ! defined( CINDER_GL_ES )
	if( mShaderWireframe )
		mShaderWireframe->uniform( "uViewportSize", vec2( getWindowSize() ) );
#endif
}

void NormalMappingApp::draw()
{
	// clear the window
	gl::clear(); 
	gl::color( Color::white() );

	if( isInitialized() ) {
		// get ready to draw in 3D
		gl::pushMatrices();
		gl::setMatrices( mCamera );

		gl::enableDepthRead();
		gl::enableDepthWrite();

		// bind textures
		mDiffuseMap->bind(0);
		mSpecularMap->bind(1);
		mNormalMap->bind(2);
		mEmissiveMap->bind(3);

		// render our model
#if ! defined( CINDER_GL_ES )
		if( mViewMode == ViewMode::Mesh && mShaderWireframe ) {
			// use our wireframe shader for this scope
			gl::ScopedGlslProg glslProgScope( mShaderWireframe );
	
			gl::pushModelMatrix();
			gl::multModelMatrix( mMeshTransform );
			gl::draw( mMesh );
			gl::popModelMatrix();
		}
		else
#endif
		{
			// use our own normal mapping shader for this scope
			gl::ScopedGlslProg GlslProgScope( mShaderNormalMapping );
	
			gl::pushModelMatrix();
			gl::multModelMatrix( mMeshTransform );
			gl::draw( mMesh );
			gl::popModelMatrix();
		}
	
		// render normals, tangents and bitangents if necessary
		if( mShowNormalsAndTangents ) {
			// use a default shader for this scope
			gl::ScopedGlslProg glslProgScope( gl::getStockShader( gl::ShaderDef().color() ) );

			gl::pushModelMatrix();
			gl::multModelMatrix( mMeshTransform );
			gl::draw( mMeshDebug );
			gl::popModelMatrix();
		}

		// get ready to render in 2D again
		gl::disableDepthWrite();
		gl::disableDepthRead();

		gl::popMatrices();

		// render the copyright message
		Area centered = Area::proportionalFit( mCopyrightMap->getBounds(), getWindowBounds(), true, false );
		centered.offset( ivec2( 0, ( getWindowHeight() - centered.y2 ) - 20 ) );

		gl::enableAlphaBlending();
		gl::draw( mCopyrightMap, centered );
		gl::disableAlphaBlending();
	}
}

void NormalMappingApp::keyDown( KeyEvent event )
{
	switch( event.getCode() ) {
		case KeyEvent::KEY_ESCAPE:
			quit();
		break;
		case KeyEvent::KEY_v:
			gl::enableVerticalSync( ! gl::isVerticalSyncEnabled() );
		break;
	}
}

TriMesh NormalMappingApp::createMesh( const fs::path& mshFile )
{	
	TriMesh mesh( TriMesh::Format().positions(3).texCoords(2).normals() );
	Timer	timer;

	// try to load the msh file
#if defined( CINDER_ANDROID )
	if( ci::android::fs::exists( mshFile ) ) {
#else
	if( fs::exists( mshFile ) ) {
#endif
		timer.start();
		mesh.read( loadFile( mshFile ) );
		CI_LOG_I( "Loading the mesh took " << timer.getSeconds() << " seconds." );
	}
	else {
		std::string msg = "File does not exist (" + mshFile.string() + ")";
		throw std::runtime_error( msg );
	}

	// if the mesh does not have normals, calculate them on-the-fly
	if( ! mesh.hasNormals() ) {
		timer.start();
		mesh.recalculateNormals();
		CI_LOG_I( "Calculating " << mesh.getNumVertices() << " normals took " << timer.getSeconds() << " seconds." );
	}

	// if the mesh does not have tangents, calculate them on-the-fly
	//  (note: your model needs to have normals and texture coordinates for this to work)
	if( ! mesh.hasTangents() ) {
		timer.start();
		mesh.recalculateTangents();
		CI_LOG_I( "Calculating " << mesh.getNumVertices() << " tangents took " << timer.getSeconds() << " seconds." );
	}

	return mesh;
}

gl::VboMeshRef NormalMappingApp::createDebugMesh( const TriMesh& mesh )
{
	// create a debug mesh, showing normals, tangents and bitangents
	DebugMesh source(mesh);

	// create a VBO from the mesh
	gl::VboMeshRef result = gl::VboMesh::create( source );

	return result;
}

CINDER_APP( NormalMappingApp, RendererGl( RendererGl::Options().msaa( 16 ) ), prepareSettings )
