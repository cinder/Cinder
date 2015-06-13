//----------------------------------------------------------------------------------
// File:        ComputeParticles/ParticleSystem.cpp
// SDK Version: v1.2 
// Email:       gameworks@nvidia.com
// Site:        http://developer.nvidia.com/
//
// Copyright (c) 2014, NVIDIA CORPORATION. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------------

#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Camera.h"
#include "cinder/MayaCamUI.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/Rand.h"
#include "cinder/params/Params.h"
#include "cinder/gl/Context.h"
#include "Ssbo.h"
#include "ScopedBufferBase.h"

using namespace ci;
using namespace ci::app;


#define LIGHT_RADIUS	1.5f // Must be at least 1

float sfrand()
{
	//return ( ( randFloat() * 2.0f ) - 1.0f );
	return randPosNegFloat( -1.0f, 1.0f );
}

struct ShaderParams
{
	vec4 attractor;
	uint32_t numParticles;
	float spriteSize;
	float damping;
	float noiseFreq;
	float noiseStrength;

	ShaderParams() :
		spriteSize( 0.015f ),
		attractor( 0.0f, 0.0f, 0.0f, 0.0f ),
		damping( 0.95f ),
		noiseFreq( 10.0f ),
		noiseStrength( 0.001f )
	{
	}
};

class NVidiaComputeParticlesApp : public AppNative {
public:
	void	setup() override;
	void	resize() override;
	void	update() override;
	void	draw() override;
	void	mouseDrag( MouseEvent event ) override;
	void	mouseDown( MouseEvent event ) override;
	void	prepareSettings( Settings *settings ) override;

	void renderScene( gl::GlslProgRef effect );
	void setupShaders();
	void setupTextures();
	void setupFbos();
	void setupBuffers();
	void resetParticleSystem( float size );
	void updateParticleSystem();
	void setupNoiseTexture3D();

	enum { WORK_GROUP_SIZE = 128, NUM_PARTICLES = 1 << 18 };

	gl::VboRef mVBO;
	gl::VboMeshRef teapot;
	gl::GlslProgRef mRenderProg;
	gl::GlslProgRef mUpdateProg;

	SsboT<vec4>::Ref mPos;
	SsboT<vec4>::Ref mVel;
	gl::VboRef mIndicesVbo;

	gl::Texture3dRef mNoiseTex;
	params::InterfaceGlRef mParams;
	MayaCamUI mMayaCam;
	ShaderParams mShaderParams;
	int mNoiseSize;
	bool mEnableAttractor;
	bool mAnimate;
	bool mReset;
	float mTime;
	float mPrevElapsedSeconds;
};


void NVidiaComputeParticlesApp::setup()
{
	mEnableAttractor = false;
	mAnimate = true;
	mReset = false;
	mTime = 0.0f;
	mPrevElapsedSeconds = 0.0f;
	mNoiseSize = 16;

	setupNoiseTexture3D();
	setupShaders();
	setupBuffers();

	resetParticleSystem( 0.5f );

	CameraPersp cam( mMayaCam.getCamera() );
	cam.lookAt( vec3( 0.0f, 0.0f, -3.0f ), vec3( 0 ) );
	cam.setCenterOfInterestPoint( vec3( 0 ) );
	cam.setNearClip( 0.10f );
	cam.setFarClip( 10.0f );
	mMayaCam.setCurrentCam( cam );

	mParams = params::InterfaceGl::create( "Settings", toPixels( ivec2( 225, 180 ) ) );
	mParams->addSeparator();
	mParams->addParam( "Animate", &mAnimate );
	mParams->addParam( "Enable attractor", &mEnableAttractor );
	mParams->addSeparator();
	mParams->addParam( "Sprite size", &( mShaderParams.spriteSize ) );// Range: 0.0f, 0.04f );
	mParams->addParam( "Noise strength", &( mShaderParams.noiseStrength ) );// Range: 0.0f, 0.01f );
	mParams->addParam( "Noise frequency", &( mShaderParams.noiseFreq ) );// Range: 0.0f, 20.0f );
	mParams->addSeparator();
	mParams->addParam( "Reset", &mReset );
}

void NVidiaComputeParticlesApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 1280, 720 );
}

void NVidiaComputeParticlesApp::setupShaders()
{
	try {
		mRenderProg = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "render.vs.glsl" ) )
			.fragment( loadAsset( "render.fs.glsl" ) ) );
	}
	catch( gl::GlslProgCompileExc e ) {
		ci::app::console() << e.what() << std::endl;
		shutdown();
	}

	try {
		mUpdateProg = gl::GlslProg::create( gl::GlslProg::Format().shaderStage( loadAsset( "particles.cs.glsl" ), GL_COMPUTE_SHADER ) );
	}
	catch( gl::GlslProgCompileExc e ) {
		ci::app::console() << e.what() << std::endl;
		shutdown();
	}

	mUpdateProg->uniform( "invNoiseSize", 1.0f / mNoiseSize );
	mUpdateProg->uniform( "noiseTex3D", 0 );
}

void NVidiaComputeParticlesApp::setupBuffers()
{
	mPos = SsboT<vec4>::create( NUM_PARTICLES, GL_STATIC_DRAW );
	mVel = SsboT<vec4>::create( NUM_PARTICLES, GL_STATIC_DRAW );

	std::vector<uint32_t> indices( NUM_PARTICLES * 6 );
	// the index buffer is a classic "two-tri quad" array.
	// This may seem odd, given that the compute buffer contains a single
	// vector for each particle.  However, the shader takes care of this
	// by indexing the compute shader buffer with a /4.  The value mod 4
	// is used to compute the offset from the vertex site, and each of the
	// four indices in a given quad references the same center point
	//uint32_t *indices = mIndices->mapT( GL_WRITE_ONLY );
	for( size_t i = 0, j = 0; i < NUM_PARTICLES; ++i ) {
		size_t index = i << 2;
		indices[j++] = index;
		indices[j++] = index + 1;
		indices[j++] = index + 2;
		indices[j++] = index;
		indices[j++] = index + 2;
		indices[j++] = index + 3;
	}

	mIndicesVbo = gl::Vbo::create<uint32_t>( GL_ELEMENT_ARRAY_BUFFER, indices, GL_STATIC_DRAW );

}

void NVidiaComputeParticlesApp::resize()
{
	CameraPersp cam( mMayaCam.getCamera() );
	float vfov = 45.0f;
	cam.setPerspective( vfov, getWindowAspectRatio(), 0.1f, 10.0f );
	mMayaCam.setCurrentCam( cam );
}

void NVidiaComputeParticlesApp::update()
{
	if( mAnimate ) {
		updateParticleSystem();
	}
}

void NVidiaComputeParticlesApp::draw()
{
	CI_CHECK_GL();
	gl::clear( ColorA( 0.25f, 0.25f, 0.25f, 1.0f ) );
	gl::clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	if( mReset ) {
		mReset = false;
		resetParticleSystem( 0.5f );
	}

	gl::setMatrices( mMayaCam.getCamera() );

	// draw particles
	gl::ScopedGlslProg scopedRenderProg( mRenderProg );
	mRenderProg->uniform( "spriteSize", mShaderParams.spriteSize );

	gl::context()->setDefaultShaderVars();

	gl::enableAdditiveBlending();

	gl::disable( GL_DEPTH_TEST );
	gl::disable( GL_CULL_FACE );

	ScopedBufferBase scopedPosBuffer( mPos, 1 );

	{
		gl::ScopedBuffer scopedIndicex( mIndicesVbo );
		gl::drawElements( GL_TRIANGLES, NUM_PARTICLES * 6, GL_UNSIGNED_INT, 0 );
	}

	CI_CHECK_GL();
	gl::disableAlphaBlending();

	mParams->draw();
}

void NVidiaComputeParticlesApp::mouseDown( MouseEvent event )
{
	mMayaCam.mouseDown( event.getPos() );
}

void NVidiaComputeParticlesApp::mouseDrag( MouseEvent event )
{
	mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
}

void NVidiaComputeParticlesApp::resetParticleSystem( float size )
{
	vec4 *pos = mPos->mapT( GL_WRITE_ONLY );
	for( size_t i = 0; i < NUM_PARTICLES; ++i ) {
		pos[i] = vec4( sfrand() * size, sfrand() * size, sfrand() * size, 1.0f );
	}
	mPos->unmap();

	vec4 *vel = mVel->mapT( GL_WRITE_ONLY );
	for( size_t i = 0; i < NUM_PARTICLES; ++i ) {
		vel[i] = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	}
	mVel->unmap();
}

void NVidiaComputeParticlesApp::updateParticleSystem()
{
	mShaderParams.numParticles = NUM_PARTICLES;
	if( mEnableAttractor ) {
		// move attractor
		const float speed = 0.2f;
		mShaderParams.attractor.x = math<float>::sin( mTime * speed );
		mShaderParams.attractor.y = math<float>::sin( mTime * speed * 1.3f );
		mShaderParams.attractor.z = math<float>::cos( mTime * speed );
		float elapsedSeconds = static_cast<float>( ci::app::getElapsedSeconds() );
		mTime += elapsedSeconds - mPrevElapsedSeconds;
		mPrevElapsedSeconds = elapsedSeconds;

		mShaderParams.attractor.w = 0.0002f;
	}
	else {
		mShaderParams.attractor.w = 0.0f;
	}
	// Invoke the compute shader to integrate the particles
	gl::ScopedGlslProg prog( mUpdateProg );
	mUpdateProg->uniform( "attractor", mShaderParams.attractor );
	//mUpdateProg->uniform( "numParticles", static_cast<int>( mShaderParams.numParticles ) ); //fix this???
	mUpdateProg->uniform( "numParticles", static_cast<float>( mShaderParams.numParticles ) ); //fix this???
	mUpdateProg->uniform( "spriteSize", mShaderParams.spriteSize );
	mUpdateProg->uniform( "damping", mShaderParams.damping );
	mUpdateProg->uniform( "noiseFreq", mShaderParams.noiseFreq );
	mUpdateProg->uniform( "noiseStrength", mShaderParams.noiseStrength );

	gl::ScopedTextureBind scoped3dTex( mNoiseTex );

	ScopedBufferBase scopedPosBuffer( mPos, 1 );
	ScopedBufferBase scopedVelBuffer( mVel, 2 );

	glDispatchCompute( NUM_PARTICLES / WORK_GROUP_SIZE, 1, 1 );
	// We need to block here on compute completion to ensure that the
	// computation is done before we render
	glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
	CI_CHECK_GL();
}

void NVidiaComputeParticlesApp::setupNoiseTexture3D()
{
	gl::Texture3d::Format tex3dFmt;
	tex3dFmt.setWrapR( GL_REPEAT );
	tex3dFmt.setWrapS( GL_REPEAT );
	tex3dFmt.setWrapT( GL_REPEAT );
	tex3dFmt.setMagFilter( GL_LINEAR );
	tex3dFmt.setMinFilter( GL_LINEAR );
	tex3dFmt.setPixelDataFormat( GL_RGBA );
	tex3dFmt.setPixelDataType( GL_FLOAT );
	tex3dFmt.setInternalFormat( GL_RGBA8_SNORM );

	int width = mNoiseSize;
	int height = mNoiseSize;
	int depth = mNoiseSize;

	std::vector<float> data( width * height * depth * 4 );
	int i = 0;
	for( int z = 0; z < depth; z++ ) {
		for( int y = 0; y < height; y++ ) {
			for( int x = 0; x < width; x++ ) {
				data[i++] = sfrand();
				data[i++] = sfrand(); 
				data[i++] = sfrand(); 
				data[i++] = sfrand(); 
			}
		}
	}
	mNoiseTex = gl::Texture3d::create( mNoiseSize, mNoiseSize, mNoiseSize, tex3dFmt );
	gl::ScopedTextureBind scoped3dTex( mNoiseTex );

	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glTexImage3D( mNoiseTex->getTarget(),
					0,
					mNoiseTex->getInternalFormat(),
					mNoiseTex->getWidth(),
					mNoiseTex->getHeight(),
					mNoiseTex->getDepth(),
					0.0f,
					tex3dFmt.getPixelDataFormat(),
					GL_FLOAT,//tex3dFmt.getPixelDataType(), //!!! Possibly copy and paste bug!!!!
					&( data[0] ) );
}

CINDER_APP_NATIVE( NVidiaComputeParticlesApp, RendererGl )
