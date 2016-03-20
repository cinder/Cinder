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

#pragma once

#include "cinder/vk/vk.h"

class FishTornadoApp;
class GpuFlocker;
using GpuFlockerRef = std::shared_ptr<GpuFlocker>;

class GpuFlocker {
public:	
	GpuFlocker( FishTornadoApp *app );

	static GpuFlockerRef		create( FishTornadoApp *app );

	void						setFboPositions( const ci::vk::TextureRef& curTex, const ci::vk::TextureRef& prevTex, ci::Surface32f& velSurf );
	void						setFboVelocities( const ci::vk::TextureRef& curTex, const ci::vk::TextureRef& prevTex, ci::Surface32f& outSurf );
	void						update( float time, float dt, const ci::vec3& camPos, const ci::vec3& predPos );
	void						updateForDepthFbo();
	void						updateForMainFbo();
	void						drawIntoVelocityFbo();
	void						drawIntoPositionFbo();
	void						drawToDepthFbo();
	void						drawToMainFbo();
	
	void						swapFbos();	
	void						beginSimRender( const ci::vk::CommandBufferRef& cmdBuf );
	void						nextSimPass();
	void						endSimRender();

	void						processSimulation( const ci::vk::CommandBufferRef& cmdBuf );

	int							getNumFlockers();
	
	// parameters
	float						mMinThresh, mMaxThresh;
	float						mZoneRadius;
	float						mMinSpeed, mMaxSpeed;
	
	float						mTime, mTimeDelta;

	int							mThisFbo, mPrevFbo;
	ci::vk::TextureRef			mPositionTextures[2];
	ci::vk::TextureRef			mVelocityTextures[2];
	ci::vk::RenderPassRef		mRenderPasses[2];
	ci::vk::FramebufferRef		mFbos[2];

private:
	FishTornadoApp				*mApp = nullptr;

	ci::vec2					mFboSize;
	ci::Area					mFboBounds;
	
	ci::vk::TextureRef			mDiffuseTex;
	ci::vk::TextureRef			mNormalsTex;
	ci::vk::TextureRef			mSpecularTex;

	ci::vk::GlslProgRef			mPositionShader;
	ci::vk::GlslProgRef			mVelocityShader;
	ci::vk::GlslProgRef			mRenderShader;
	ci::vk::GlslProgRef			mDepthShader;
	
	ci::vk::BatchRef			mHiResFishBatch;
	ci::vk::BatchRef			mLoResFishBatch;
	bool						mCanDrawFishDepth = false;
	bool						mCanDrawFishMain = false;

	ci::vk::BatchRef			mPositionRects[2];
	ci::vk::BatchRef			mVelocityRects[2];
	bool						mCanDrawPosition[2];
	bool						mCanDrawVelocity[2];

	ci::vk::CommandBufferRef	mCommandBuffer;
};

