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

#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/params/Params.h"
#include "cinder/Camera.h"
#include "cinder/ImageIo.h"
#include "cinder/MayaCamUI.h"
#include "cinder/Perlin.h"
#include "cinder/Timeline.h"
#include "cinder/Timer.h"
#include "cinder/TriMesh.h"

#include "DebugMesh.h"

using namespace ci;
using namespace ci::app;
using namespace std;

struct LightSource
{
	Vec4f position;
	ColorA diffuse;
	ColorA specular;
};

class NormalMappingApp : public AppNative {

#if ! defined( CINDER_GL_ES )
	typedef enum { Default, Glossy, Normals, Lighting, Mesh } ViewMode;
#else
	typedef enum { Default, Glossy, Normals, Lighting } ViewMode;
#endif

public:
	void	prepareSettings( Settings* settings );

	void	setup();
	void	shutdown();

	void	update();
	void	draw();
	void	resize();

	void	mouseDown( MouseEvent event );
	void	mouseDrag( MouseEvent event );

	void	keyDown( KeyEvent event );

	bool	isInitialized() const {
		return (mDiffuseMap && mSpecularMap && mNormalMap && mCopyrightMap && mShaderNormalMapping && mMesh);
	}

private:
	/* Load the mesh and calculate normals and tangents if necessary. */
	TriMesh			createMesh(const fs::path& mshFile);
	/* Construct a mesh to visualize normals (blue), tangents (red) and bitangents (green). */
	gl::VboMeshRef	createDebugMesh(const TriMesh& mesh);

private:
	ViewMode			mViewMode;

	Matrix44f			mMeshTransform;
	AxisAlignedBox3f	mMeshBounds;

	CameraPersp			mCamera;
	MayaCamUI			mMayaCamera;

	LightSource			mLightLantern;
	LightSource			mLightAmbient;

	gl::TextureRef		mCopyrightMap;
	gl::TextureRef		mDiffuseMap;
	gl::TextureRef		mSpecularMap;
	gl::TextureRef		mNormalMap;
	gl::TextureRef		mEmmisiveMap;

	gl::GlslProgRef		mShaderNormalMapping;
	gl::GlslProgRef		mShaderWireframe;

	gl::VboMeshRef		mMesh;
	gl::VboMeshRef		mMeshDebug;

	bool				bAutoRotate;
	float				fAutoRotateAngle;

	bool				bAnimateLantern;
	Perlin				mPerlin;

	bool				bEnableNormalMap;
	bool				bShowNormalsAndTangents;

	float				fTime;

#if ! defined( CINDER_GL_ES )
	params::InterfaceGlRef	mParams;
#endif
};

void NormalMappingApp::prepareSettings(Settings* settings)
{
	settings->disableFrameRate();

	settings->setWindowSize( 1024, 768 );
	settings->setTitle( "OpenGL Core Profile - Normal Mapping Demo" );
}

void NormalMappingApp::setup()
{
	// setup camera and lights
	mCamera.setEyePoint( Vec3f( 0.2f, 0.4f, 1.0f ) );
	mCamera.setCenterOfInterestPoint( Vec3f(0.0f, 0.425f, 0.0f) );
	mCamera.setNearClip( 0.01f );
	mCamera.setFarClip( 100.0f );

	mLightLantern.diffuse = Color(0.9f, 0.6f, 0.3f);
	mLightLantern.specular = Color(0.9f, 0.6f, 0.3f);

	mLightAmbient.diffuse = Color(0.2f, 0.6f, 1.0f);
	mLightAmbient.specular = Color(0.05f, 0.15f, 0.25f);

	// setup perlin noise to easily animate our lantern light source
	mPerlin = Perlin(4, 65535);

	// default settings
	mMeshBounds = AxisAlignedBox3f( Vec3f::zero(), Vec3f::one() );

	bAutoRotate = true;
	fAutoRotateAngle = 0.0f;

	bAnimateLantern = true;

	bEnableNormalMap = true;
	bShowNormalsAndTangents = false;

	mViewMode = ViewMode::Default;

	// load assets
	try {
		// load our copyright message
		mCopyrightMap  = gl::Texture::create( loadImage( loadAsset("copyright.png") ) );

		// load textures
		mDiffuseMap = gl::Texture::create( loadImage( loadAsset("leprechaun_diffuse.jpg") ) );
		mSpecularMap = gl::Texture::create( loadImage( loadAsset("leprechaun_specular.jpg") ) );
		mNormalMap = gl::Texture::create( loadImage( loadAsset("leprechaun_normal.jpg") ) );
		mEmmisiveMap = gl::Texture::create( loadImage( loadAsset("leprechaun_emmisive.png") ) );

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
	catch( const std::exception& e ) {
		console() << "Error loading asset: " << e.what() << std::endl;
		quit();
	}

	// load mesh file and create missing data (normals, tangents) if necessary
	try {
		fs::path mshFile = getAssetPath("") / "leprechaun.msh";
		TriMesh mesh = createMesh( mshFile );

		mMesh = gl::VboMesh::create( mesh );
		mMeshBounds = mesh.calcBoundingBox();

		mMeshDebug = createDebugMesh(mesh);
	}
	catch( const std::exception& e ) {
		console() << "Error loading asset: " << e.what() << std::endl;
		quit();
	}

	// create a parameter window, so we can toggle stuff
	std::vector<std::string> viewmodes;
	viewmodes.push_back("Final");
	viewmodes.push_back("Glossy");
	viewmodes.push_back("Normals");
	viewmodes.push_back("Lighting");
	viewmodes.push_back("Mesh");

#if ! defined( CINDER_GL_ES )
	mParams = params::InterfaceGl::create( getWindow(), "Normal Mapping Demo", Vec2i(340, 150) );
	mParams->setOptions( "", "valueswidth=100" );

	mParams->addParam( "Enable Normal Mapping", &bEnableNormalMap );
	mParams->addParam( "Viewing Mode", viewmodes, (int*) &mViewMode );

	mParams->addSeparator();

	mParams->addParam( "Rotate Model", &bAutoRotate );
	mParams->addParam( "Animate Light", &bAnimateLantern );
	mParams->addParam( "Show Normals & Tangents", &bShowNormalsAndTangents );
#endif

	// keep track of time
	fTime = (float) getElapsedSeconds();
}

void NormalMappingApp::shutdown()
{
}

void NormalMappingApp::update()
{
	// keep track of time
	float fElapsed = (float) getElapsedSeconds() - fTime;
	fTime += fElapsed;
	
	// rotate the mesh
	if(bAutoRotate) 
		fAutoRotateAngle += (fElapsed * 0.2f);

	mMeshTransform.setToIdentity();
	mMeshTransform.rotate( Vec3f::yAxis(), fAutoRotateAngle );
	mMeshTransform.scale( Vec3f::one() / mMeshBounds.getSize().y );

	// position our lights (in eye space)
	Vec3f lanternPositionOS = Vec3f(12.5f, 30.0f, 12.5f);
	if(bAnimateLantern) 
		lanternPositionOS += mPerlin.dfBm( Vec3f( 0.0f, 0.0f, fTime ) ) * 5.0f;
	Vec3f lanternPositionWS = mMeshTransform.transformPointAffine( lanternPositionOS );
	mLightLantern.position = mCamera.getViewMatrix().transformPointAffine( lanternPositionWS );
	mLightAmbient.position = Vec4f::zero();

	// set the varying shader uniforms
	mShaderNormalMapping->uniform( "bShowNormals", mViewMode == ViewMode::Normals );
	mShaderNormalMapping->uniform( "bUseDiffuseMap", (mViewMode == ViewMode::Default || mViewMode == ViewMode::Glossy) );
	mShaderNormalMapping->uniform( "bUseSpecularMap", (mViewMode == ViewMode::Default || mViewMode == ViewMode::Lighting) );
	mShaderNormalMapping->uniform( "bUseEmmisiveMap", (mViewMode == ViewMode::Default || mViewMode == ViewMode::Glossy) );
	mShaderNormalMapping->uniform( "bUseNormalMap", bEnableNormalMap );

	mShaderNormalMapping->uniform( "uLights[0].position", mLightLantern.position );
	mShaderNormalMapping->uniform( "uLights[1].position", mLightAmbient.position );

#if ! defined( CINDER_GL_ES )
	if(mShaderWireframe)
		mShaderWireframe->uniform( "uViewportSize", Vec2f( getWindowSize() ) );
#endif
}

void NormalMappingApp::draw()
{
	// clear the window
	gl::clear(); 
	gl::color( Color::white() );

	if(isInitialized())
	{
		// get ready to draw in 3D
		gl::pushMatrices();
		gl::setMatrices( mCamera );

		gl::enableDepthRead();
		gl::enableDepthWrite();

		// bind textures
		mDiffuseMap->bind(0);
		mSpecularMap->bind(1);
		mNormalMap->bind(2);
		mEmmisiveMap->bind(3);

		// render our model
#if ! defined( CINDER_GL_ES )
		if(mViewMode == ViewMode::Mesh && mShaderWireframe)
		{
			// use our wireframe shader for this scope
			gl::ScopedGlslProg GlslProgScope( mShaderWireframe );
	
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
		if(bShowNormalsAndTangents) {
			// use a default shader for this scope
			gl::ScopedGlslProg GlslProgScope( gl::getStockShader( gl::ShaderDef().color() ) );

			gl::pushModelMatrix();
			gl::multModelMatrix( mMeshTransform );
			gl::draw( mMeshDebug );
			gl::popModelMatrix();
		}

		// get ready to render in 2D again
		gl::disableDepthWrite();
		gl::disableDepthRead();

		gl::popMatrices();

		// render our parameter window
#if ! defined( CINDER_GL_ES )
		if( mParams )
			mParams->draw();
#endif

		// render the copyright message
		Area centered = Area::proportionalFit( mCopyrightMap->getBounds(), getWindowBounds(), true, false );
		centered.offset( Vec2i(0, (getWindowHeight() - centered.y2) - 20) );

		gl::enableAlphaBlending();
		gl::draw( mCopyrightMap, centered );
		gl::disableAlphaBlending();
	}
}

void NormalMappingApp::resize()
{
	mCamera.setAspectRatio( getWindowAspectRatio() );
}

void NormalMappingApp::mouseDown( MouseEvent event )
{
	mMayaCamera.setCurrentCam( mCamera );
	mMayaCamera.mouseDown( event.getPos() );
}

void NormalMappingApp::mouseDrag( MouseEvent event )
{
	mMayaCamera.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
	mCamera = mMayaCamera.getCamera();
}

void NormalMappingApp::keyDown( KeyEvent event )
{
	switch( event.getCode() ) {
		case KeyEvent::KEY_ESCAPE:
			quit();
		break;
		case KeyEvent::KEY_v:
			gl::enableVerticalSync( !gl::isVerticalSyncEnabled() );
		break;
	}
}

TriMesh NormalMappingApp::createMesh(const fs::path& mshFile)
{	
	TriMesh mesh( TriMesh::Format().positions(3).texCoords(2).normals() );
	Timer	timer;

	// try to load the msh file
	if( fs::exists(mshFile) ) {
		timer.start();
		mesh.read( loadFile(mshFile) ); 
		console() << "Loading the mesh took " << timer.getSeconds() << " seconds." << std::endl;
	}
	else {
		std::string msg = "Could not locate the file ("+mshFile.string()+").";
		throw std::runtime_error( msg );
	}

	// if the mesh does not have normals, calculate them on-the-fly
	if( ! mesh.hasNormals() ) {
		timer.start();
		mesh.recalculateNormals();
		console() << "Calculating " << mesh.getNumVertices() << " normals took " << timer.getSeconds() << " seconds." << std::endl;
	}

	// if the mesh does not have tangents, calculate them on-the-fly
	//  (note: your model needs to have normals and texture coordinates for this to work)
	if( ! mesh.hasTangents() ) {
		timer.start();
		mesh.recalculateTangents();
		console() << "Calculating " << mesh.getNumVertices() << " tangents took " << timer.getSeconds() << " seconds." << std::endl;
	}

	return mesh;
}

gl::VboMeshRef NormalMappingApp::createDebugMesh(const TriMesh& mesh)
{
	// create a debug mesh, showing normals, tangents and bitangents
	DebugMesh source(mesh);

	// create a VBO from the mesh
	gl::VboMeshRef result = gl::VboMesh::create( source );

	return result;
}

CINDER_APP_NATIVE( NormalMappingApp, RendererGl )
