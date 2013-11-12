/*
 Copyright (c) 2013, Paul Houx - All rights reserved.
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
#include "cinder/Timeline.h"
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

	bool	isInitialized() const { return (mDiffuseMap && mSpecularMap && mNormalMap && mCopyrightMap
												&& mShader && mLightLantern && mLightAmbient && mMesh); }

private:
	TriMesh			createMesh(const fs::path& mshFile);
	gl::VboMeshRef	createDebugMesh(const TriMesh& mesh);

private:
	Matrix44f			mMeshTransform;
	AxisAlignedBox3f	mMeshBounds;

	CameraPersp			mCamera;
	MayaCamUI			mMayaCamera;

	gl::Light*			mLightLantern;
	gl::Light*			mLightAmbient;

	gl::TextureRef		mCopyrightMap;
	gl::TextureRef		mDiffuseMap;
	gl::TextureRef		mSpecularMap;
	gl::TextureRef		mNormalMap;
	gl::TextureRef		mEmmisiveMap;

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
	bool				bEnableEmmisiveMap;

	bool				bShowNormalsAndTangents;
	bool				bShowNormals;

	float				fTime;
	Anim<float>			fOpacity;

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
	mParams->addParam( "Enable Emmisive Map", &bEnableEmmisiveMap );
	mParams->setOptions( "", "valueswidth=fit" );

	// setup camera and lights
	mCamera.setEyePoint( Vec3f( 0.2f, 0.4f, 1.0f ) );
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

	// setup perlin noise to easily animate our lantern light source
	mPerlin = Perlin(4, 65535);

	// default settings
	mMeshBounds = AxisAlignedBox3f( Vec3f::zero(), Vec3f::one() );

	bAutoRotate = true;
	fAutoRotateAngle = 0.0f;

	bAnimateLantern = true;

	bEnableDiffuseMap = true;
	bEnableSpecularMap = true;
	bEnableNormalMap = true;
	bEnableEmmisiveMap = true;

	bShowNormalsAndTangents = false;
	bShowNormals = false;

	// load assets
	try {
		// load our copyright message
		mCopyrightMap  = gl::Texture::create( loadImage( loadAsset("copyright.png") ) );

		// load textures
		mDiffuseMap = gl::Texture::create( loadImage( loadAsset("leprechaun_diffuse.jpg") ) );
		mSpecularMap = gl::Texture::create( loadImage( loadAsset("leprechaun_specular.jpg") ) );
		mNormalMap = gl::Texture::create( loadImage( loadAsset("leprechaun_normal.jpg") ) );
		mEmmisiveMap = gl::Texture::create( loadImage( loadAsset("leprechaun_emmisive.png") ) );

		// load our shader
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

	// animate copyright message
	fOpacity = 0.0f;
	timeline().apply( &fOpacity, 0.0f, 0.0f, 2.0f );
	timeline().appendTo( &fOpacity, 1.0f, 2.5f, EaseInOutCubic() );
	timeline().appendTo( &fOpacity, 1.0f, 30.0f );
	timeline().appendTo( &fOpacity, 0.0f, 2.5f, EaseInOutCubic() );

	// keep track of time
	fTime = (float) getElapsedSeconds();
}

void NormalMappingReduxApp::shutdown()
{
	// safely delete our lights
	if(mLightAmbient) delete mLightAmbient;
	if(mLightLantern) delete mLightLantern;

	mLightAmbient = mLightLantern = NULL;
}

void NormalMappingReduxApp::update()
{
	// keep track of time
	float fElapsed = (float) getElapsedSeconds() - fTime;
	fTime += fElapsed;
	
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
		mDiffuseMap->enableAndBind();
		mSpecularMap->bind(1);
		mNormalMap->bind(2);
		mEmmisiveMap->bind(3);

		// bind our normal mapping shader
		mShader.bind();
		mShader.uniform( "uDiffuseMap", 0 );
		mShader.uniform( "uSpecularMap", 1 );
		mShader.uniform( "uNormalMap", 2 );
		mShader.uniform( "uEmmisiveMap", 3 );
		mShader.uniform( "bShowNormals", bShowNormals );
		mShader.uniform( "bUseDiffuseMap", bEnableDiffuseMap );
		mShader.uniform( "bUseSpecularMap", bEnableSpecularMap );
		mShader.uniform( "bUseNormalMap", bEnableNormalMap );
		mShader.uniform( "bUseEmmisiveMap", bEnableEmmisiveMap );

		// enable and position our lights
		mLightLantern->enable();
		mLightAmbient->enable();

		Vec3f lanternPositionOS = Vec3f(12.5f, 30.0f, 12.5f);
		if(bAnimateLantern) 
			lanternPositionOS += mPerlin.dfBm( Vec3f( 0.0f, 0.0f, fTime ) ) * 5.0f;
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

		// render the copyright message
		Area centered = Area::proportionalFit( mCopyrightMap->getBounds(), getWindowBounds(), true, false );
		centered.offset( Vec2i(0, (getWindowHeight() - centered.y2) - 20) );

		gl::enableAlphaBlending();
		gl::color( ColorA(1, 1, 1, fOpacity.value()) );
		gl::draw( mCopyrightMap, mCopyrightMap->getBounds(), centered );
		gl::disableAlphaBlending();
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
