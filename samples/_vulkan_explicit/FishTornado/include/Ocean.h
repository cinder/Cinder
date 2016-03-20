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

#include "cinder/app/App.h"
#include "cinder/vk/vk.h"
#include "cinder/Matrix44.h"
#include "cinder/ObjLoader.h"
#include "Globals.h"

class FishTornadoApp;
class Ocean;
using OceanRef = std::shared_ptr<Ocean>;

class Ocean {
public:
	Ocean( FishTornadoApp *app );

	static OceanRef			create( FishTornadoApp *app );

	void					initVbos();
	void					update( float time, float dt );
	void					updateForMainFbo();
	void					drawFloor();
	void					drawSurface();
	void					drawBeams();
	
	const ci::vk::TextureRef&	getCausticsTex() const;

private:
	FishTornadoApp			*mApp = nullptr;

	float					mTime;
	
	ci::vk::GlslProgRef		mSurfaceShader;
	ci::vk::TextureRef		mSurfaceHeightsTex;
	ci::vk::TextureRef		mSurfaceNormalsTex;
	ci::vk::BatchRef		mSurfaceBatch;
	
	ci::vk::GlslProgRef		mFloorShader;
	ci::vk::TextureRef		mFloorDiffuseTex;
	ci::vk::TextureRef		mFloorNormalsTex;
	ci::vk::TextureRef		mFloorHeightsTex;
	ci::vk::TextureRef		mFloorHeightNormalsTex;
	ci::vk::BatchRef		mFloorBatch;
	
	ci::vk::GlslProgRef		mBeamShader;
	ci::vk::TextureRef		mBeamTex;
	ci::vk::BatchRef		mBeamsBatch1;
	ci::vk::BatchRef		mBeamsBatch2;
	bool					mCanDraw = false;

	// caustics
	ci::vk::TextureRef		mCausticsTexs[32];
	int						mCausticsIndex;
};