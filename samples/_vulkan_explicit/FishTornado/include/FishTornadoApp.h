/*
 Copyright (c) 2014, Robert Hodgin, All rights reserved.
 Copyright (c) 2016, The Cinder Project, All rights reserved.

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

 Copyright 2016 Google Inc.
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

#include "cinder/app/App.h"
#include "cinder/app/RendererVk.h"
#include "cinder/vk/vk.h"
#include "cinder/Camera.h"
#include "cinder/CameraUi.h"
#include "Globals.h"

class GpuFlocker;
class Light;
class Ocean;
class Shark;
using GpuFlockerRef = std::shared_ptr<GpuFlocker>;
using LightRef = std::shared_ptr<Light>;
using OceanRef = std::shared_ptr<Ocean>;
using SharkRef = std::shared_ptr<Shark>;

class FishTornadoApp : public ci::app::App {
public:

	FishTornadoApp();
	virtual ~FishTornadoApp();

	virtual void			resize() override;
	virtual void			setup() override;
	virtual void			mouseMove( ci::app::MouseEvent event ) override;
	virtual void			mouseDown( ci::app::MouseEvent event ) override;
	virtual void			mouseDrag( ci::app::MouseEvent event ) override;
	virtual void			mouseUp( ci::app::MouseEvent event ) override;
	virtual void			keyDown( ci::app::KeyEvent event ) override;
	virtual void			updateTime();
	virtual void			update() override;
	virtual void			draw() override;
	

	const Light*			getLight() const { return mCachedLightPtr; }
	const Ocean*			getOcean() const { return mCachedOceanPtr; }

private:
	void					loadShaders();
	void					drawToMainFbo( const ci::vk::CommandBufferRef& cmdBuf );
	void					drawToDepthFbo( const ci::vk::CommandBufferRef& cmdBuf );

	void					generateCommandBuffer( const ci::vk::CommandBufferRef& cmdBuf );
	
	// Time
	float					mTime;
	float					mTimeElapsed;
	float					mTimeMulti;
	float					mTimeDelta;
	
	// Camera
	ci::CameraPersp			mCamera;
	ci::CameraUi			mCamUi;
	
	// Light source
	LightRef				mLight;
	ci::vk::GlslProgRef		mShadowsGlsl;
	
	ci::vk::GlslProgRef		mPositionShader;
	ci::vk::GlslProgRef		mVelocityShader;
	ci::vk::GlslProgRef		mRenderShader;
	ci::vk::GlslProgRef		mDepthShader;
	
#if defined( THREADED_LOAD )
	std::shared_ptr<std::thread>	mLightLoadThread;
	std::shared_ptr<std::thread>	mOceanLoadThread;
	std::shared_ptr<std::thread>	mSharkLoadThread;
	std::shared_ptr<std::thread>	mFishLoadThread;
	std::atomic<bool>				mLightLoaded	= false;
	std::atomic<bool>				mOceanLoaded	= false;
	std::atomic<bool>				mSharkLoaded	= false;
	std::atomic<bool>				mFishLoaded		= false;
	bool							mCanAnimate		= false;
#else 
	std::atomic<bool>		mLightLoaded = true;
	std::atomic<bool>		mOceanLoaded = true;
	std::atomic<bool>		mSharkLoaded = true;
	std::atomic<bool>		mFishLoaded = true;
#endif

	Ocean*					mCachedOceanPtr		= nullptr;
	Light*					mCachedLightPtr		= nullptr;

	OceanRef				mOcean;
	SharkRef				mShark;
	GpuFlockerRef			mGpuFlocker;
	ci::vk::BatchRef		mHiResFishBatch;
	ci::vk::BatchRef		mLoResFishBatch;
	
	ci::vk::TextureRef		mMainColorTex;
	ci::vk::TextureRef		mMainDepthTex;
	ci::vk::RenderPassRef	mMainRenderPass;
	ci::vk::FramebufferRef	mMainFbo;
	ci::vk::BatchRef		mMainBatch;
	ci::vk::GlslProgRef		mMainShader;
	ci::vk::TextureRef		mNoiseNormalsTex;
	float					mContrast;
	float					mExposure;
	float					mBrightness;
	float					mBlend;
	float					mDistortion;
		
	bool					mDrawParams;
	bool					mDrawShark;
	bool					mDrawOcean;
};