/*
 Copyright (c) 2013, Paul Houx - All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

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
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Light.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Vbo.h"
#include "cinder/params/Params.h"
#include "cinder/Camera.h"
#include "cinder/ImageIo.h"
#include "cinder/MayaCamUI.h"
#include "cinder/Perlin.h"
#include "cinder/Timer.h"
#include "cinder/TriMesh.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class NormalMappingReduxApp : public AppNative {
public:
	void	prepareSettings( Settings* settings );

	void	setup();
	void	shutdown();

	void	update();
	void	draw();
	void	resize();

	void	mouseDown( MouseEvent event );	
	void	mouseDrag( MouseEvent event );

	bool	isInitialized() const { return (mDiffuseMap && mSpecularMap && mNormalMap 
												&& mShader && mLightLantern && mLightAmbient && mMesh); }

private:
	void			delayedSetup();
	TriMesh			createMesh(const fs::path& mshFile);
	gl::VboMeshRef	createDebugMesh(const TriMesh& mesh);

private:
	Matrix44f			mMeshTransform;
	AxisAlignedBox3f	mMeshBounds;

	CameraPersp			mCamera;
	MayaCamUI			mMayaCamera;

	gl::Light*			mLightLantern;
	gl::Light*			mLightAmbient;

	gl::TextureRef		mLoadingMap;
	gl::TextureRef		mDiffuseMap;
	gl::TextureRef		mSpecularMap;
	gl::TextureRef		mNormalMap;

	gl::GlslProg		mShader;
	gl::VboMeshRef		mMesh;
	gl::VboMeshRef		mMeshDebug;

	bool				bAutoRotate;
	float				fAutoRotateAngle;

	bool				bAnimateLantern;
	Perlin				mPerlin;

	bool				bEnableDiffuseMap;
	bool				bEnableSpecularMap;
	bool				bEnableNormalMap;

	bool				bShowNormalsAndTangents;
	bool				bShowNormals;

	float				fTime;

	params::InterfaceGlRef	mParams;
};

void NormalMappingReduxApp::prepareSettings(Settings* settings)
{
	settings->setWindowSize( 1024, 768 );
	settings->setTitle( "Normal Mapping Demo" );

	// make sure our pointers are invalid
	mLightLantern = NULL;
	mLightAmbient = NULL;
}

void NormalMappingReduxApp::setup()
{	
	// load our "loading" message
	mLoadingMap  = gl::Texture::create( loadImage( loadAsset("loading.png") ) );

	// create a parameter window, so we can toggle stuff
	mParams = params::InterfaceGl::create( getWindow(), "Normal Mapping Demo", Vec2i(300, 200) );
	mParams->addParam( "Rotate Model", &bAutoRotate );
	mParams->addParam( "Animate Light", &bAnimateLantern );
	mParams->addSeparator();
	mParams->addParam( "Show Normals & Tangents", &bShowNormalsAndTangents );
	mParams->addParam( "Show Normals", &bShowNormals );
	mParams->addSeparator();
	mParams->addParam( "Enable Diffuse Map", &bEnableDiffuseMap );
	mParams->addParam( "Enable Specular Map", &bEnableSpecularMap );
	mParams->addParam( "Enable Normal Map", &bEnableNormalMap );

	// setup camera and lights
	mCamera.setEyePoint( Vec3f( 0.2f, 0.4f, 1.8f ) );
	mCamera.setCenterOfInterestPoint( Vec3f(0.0f, 0.5f, 0.0f) );
	mCamera.setNearClip( 0.01f );
	mCamera.setFarClip( 100.0f );

	mLightLantern = new gl::Light(gl::Light::DIRECTIONAL, 0);
	mLightLantern->setAmbient( Color(0.0f, 0.0f, 0.1f) );
	mLightLantern->setDiffuse( Color(0.9f, 0.6f, 0.3f) );
	mLightLantern->setSpecular( Color(0.9f, 0.6f, 0.3f) );

	mLightAmbient = new gl::Light(gl::Light::DIRECTIONAL, 1);
	mLightAmbient->setAmbient( Color(0.0f, 0.0f, 0.0f) );
	mLightAmbient->setDiffuse( Color(0.2f, 0.6f, 1.0f) );
	mLightAmbient->setSpecular( Color(0.2f, 0.2f, 0.2f) );

	mPerlin = Perlin(4, 65535);

	// default settings
	mMeshBounds = AxisAlignedBox3f( Vec3f::zero(), Vec3f::one() );

	bAutoRotate = true;
	fAutoRotateAngle = 0.0f;

	bAnimateLantern = true;

	bEnableDiffuseMap = true;
	bEnableSpecularMap = true;
	bEnableNormalMap = true;

	bShowNormalsAndTangents = false;
	bShowNormals = false;

	// keep track of time
	fTime = (float) getElapsedSeconds();
}

void NormalMappingReduxApp::delayedSetup()
{
	if( isInitialized() ) return;

	// load textures and shaders
	try {
		mDiffuseMap = gl::Texture::create( loadImage( loadAsset("leprechaun_diffuse.jpg") ) );
		mSpecularMap = gl::Texture::create( loadImage( loadAsset("leprechaun_specular.jpg") ) );
		mNormalMap = gl::Texture::create( loadImage( loadAsset("leprechaun_normal.jpg") ) );

		mShader = gl::GlslProg( loadAsset("normal_mapping_vert.glsl"), loadAsset("normal_mapping_frag.glsl") );
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

	// reset time after everything is setup
	fTime = (float) getElapsedSeconds();
}

void NormalMappingReduxApp::shutdown()
{
	if(mLightAmbient) delete mLightAmbient;
	if(mLightLantern) delete mLightLantern;

	mLightAmbient = mLightLantern = NULL;
}

void NormalMappingReduxApp::update()
{
	// keep track of time
	float fElapsed = (float) getElapsedSeconds() - fTime;
	fTime += fElapsed;
	
	// because loading the model and shaders might take a while,
	// we make sure our window is visible and cleared before calling 'delayedSetup()'
	if( !isInitialized() && getElapsedFrames() > 5 )
		delayedSetup();
	
	// rotate the mesh
	if(bAutoRotate) {
		fAutoRotateAngle += (fElapsed * 0.2f);

		mMeshTransform.setToIdentity();
		mMeshTransform.rotate( Vec3f::yAxis(), fAutoRotateAngle );
		mMeshTransform.scale( Vec3f::one() / mMeshBounds.getSize().y );
	}
}

void NormalMappingReduxApp::draw()
{
	gl::clear(); 
	gl::color( Color::white() );

	if(!isInitialized())
	{
		// render our loading message while loading is in progress
		Area centered = Area::proportionalFit( mLoadingMap->getBounds(), getWindowBounds(), true, false );
		gl::draw( mLoadingMap, mLoadingMap->getBounds(), centered );
	}
	else
	{
		// get ready to draw in 3D
		gl::pushMatrices();
		gl::setMatrices( mCamera );

		gl::enableDepthRead();
		gl::enableDepthWrite();

		// bind textures
		mDiffuseMap->enableAndBind();
		mSpecularMap->bind(1);
		mNormalMap->bind(2);

		// bind our normal mapping shader
		mShader.bind();
		mShader.uniform( "uDiffuseMap", 0 );
		mShader.uniform( "uSpecularMap", 1 );
		mShader.uniform( "uNormalMap", 2 );
		mShader.uniform( "bShowNormals", bShowNormals );
		mShader.uniform( "bUseDiffuseMap", bEnableDiffuseMap );
		mShader.uniform( "bUseSpecularMap", bEnableSpecularMap );
		mShader.uniform( "bUseNormalMap", bEnableNormalMap );

		// enable our lights
		mLightLantern->enable();
		mLightAmbient->enable();

		Vec3f offset = bAnimateLantern ? mPerlin.dfBm( Vec3f( 0.0f, 0.0f, fTime ) ) * 5.0f : Vec3f::zero();
		Vec3f lanternPositionOS = Vec3f(12.5f, 30.0f, 12.5f) + offset;
		Vec3f lanternPositionWS = mMeshTransform.transformPointAffine( lanternPositionOS );
		mLightLantern->lookAt( lanternPositionWS, Vec3f(0.0f, 0.5f, 0.0f) );
		
		mLightAmbient->lookAt( mCamera.getEyePoint(), mCamera.getCenterOfInterestPoint() );
	
		// render our model
		gl::pushModelView();
		gl::multModelView( mMeshTransform );
		gl::draw( mMesh );
		gl::popModelView();

		// disable our lights
		mLightAmbient->disable();
		mLightLantern->disable();

		// disable our shader
		mShader.unbind();

		// unbind textures
		gl::disable( mDiffuseMap->getTarget() );
	
		// render normals, tangents and bitangents if necessary
		if(bShowNormalsAndTangents) {
			gl::pushModelView();
			gl::multModelView( mMeshTransform );
			gl::draw( mMeshDebug );
			gl::popModelView();
		}

		// get ready to render in 2D again
		gl::disableDepthWrite();
		gl::disableDepthRead();

		gl::popMatrices();

		// render our parameter window
		if(mParams)
			mParams->draw();
	}
}

void NormalMappingReduxApp::resize()
{
	mCamera.setAspectRatio( getWindowAspectRatio() );
}

void NormalMappingReduxApp::mouseDown( MouseEvent event )
{
	mMayaCamera.setCurrentCam( mCamera );
	mMayaCamera.mouseDown( event.getPos() );
}

void NormalMappingReduxApp::mouseDrag( MouseEvent event )
{
	mMayaCamera.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
	mCamera = mMayaCamera.getCamera();
}

TriMesh NormalMappingReduxApp::createMesh(const fs::path& mshFile)
{	
	TriMesh mesh;
	Timer	timer;

	// try to load the msh file
	if(fs::exists(mshFile)) {
		timer.start();
		mesh.read( loadFile(mshFile) ); 
		console() << "Loading the mesh took " << timer.getSeconds() << " seconds." << std::endl;
	}
	else {
		std::string msg = "Could not locate the file ("+mshFile.string()+").";
		throw std::exception(msg.c_str());
	}

	// if the mesh does not have normals, calculate them on-the-fly
	if(!mesh.hasNormals()) {
		timer.start();
		mesh.recalculateNormals();
		console() << "Calculating " << mesh.getNumVertices() << " normals took " << timer.getSeconds() << " seconds." << std::endl;
	}

	// if the mesh does not have tangents, calculate them on-the-fly
	//  (note: your model needs to have normals and texture coordinates for this to work)
	if(!mesh.hasTangents()) {
		timer.start();
		mesh.recalculateTangents();
		console() << "Calculating " << mesh.getNumVertices() << " tangents took " << timer.getSeconds() << " seconds." << std::endl;
	}

	return mesh;
}

gl::VboMeshRef NormalMappingReduxApp::createDebugMesh(const TriMesh& mesh)
{
	// create a debug mesh, showing normals, tangents and bitangents
	size_t numVertices = mesh.getNumVertices();

	std::vector<Vec3f>		vertices;	vertices.reserve( numVertices * 4 );
	std::vector<Color>		colors;		colors.reserve( numVertices * 4 );
	std::vector<uint32_t>	indices;	indices.reserve( numVertices * 6 );

	for(size_t i=0;i<numVertices;++i) {
		uint32_t idx = vertices.size();

		vertices.push_back( mesh.getVertices()[i] );
		vertices.push_back( mesh.getVertices()[i] + mesh.getTangents()[i] );
		vertices.push_back( mesh.getVertices()[i] + mesh.getNormals()[i].cross(mesh.getTangents()[i]) );
		vertices.push_back( mesh.getVertices()[i] + mesh.getNormals()[i] );

		colors.push_back( Color(0, 0, 0) );	// base vertices black
		colors.push_back( Color(1, 0, 0) );	// tangents (along u-coordinate) red
		colors.push_back( Color(0, 1, 0) ); // bitangents (along v-coordinate) green
		colors.push_back( Color(0, 0, 1) ); // normals blue

		indices.push_back( idx );
		indices.push_back( idx + 1 );
		indices.push_back( idx );
		indices.push_back( idx + 2 );
		indices.push_back( idx );
		indices.push_back( idx + 3 );
	}

	gl::VboMesh::Layout layout;
	layout.setStaticPositions();
	layout.setStaticColorsRGB();
	layout.setStaticIndices();
	
	gl::VboMeshRef result = gl::VboMesh::create( numVertices * 4, numVertices * 6, layout, GL_LINES );
	result->bufferPositions( vertices );
	result->bufferColorsRGB( colors );
	result->bufferIndices( indices );

	return result;
}

CINDER_APP_NATIVE( NormalMappingReduxApp, RendererGl )
