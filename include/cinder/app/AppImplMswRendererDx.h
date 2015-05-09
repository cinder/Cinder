// The copyright in this software is being made available under the BSD License, included below. 
// This software may be subject to other third party and contributor rights, including patent rights, 
// and no such rights are granted under this license.
//
// Copyright (c) 2013, Microsoft Open Technologies, Inc. 
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright notice, 
//   this list of conditions and the following disclaimer in the documentation 
//   and/or other materials provided with the distribution.
// - Neither the name of Microsoft Open Technologies, Inc. nor the names of its contributors 
//   may be used to endorse or promote products derived from this software 
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, 
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#pragma once

#include "cinder/app/App.h"
#include "cinder/app/AppImplMswRenderer.h"
#include "cinder/app/RendererDx.h"
#include "cinder/dx/dx.h"
#include <d3dcompiler.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <dxgi.h>
#include <dxgi1_2.h>

namespace cinder { namespace dx {
class Texture;
}}

namespace cinder { namespace app {

class AppImplMswRendererDx : public AppImplMswRenderer {
 public:
 	struct LightData
	{
		LightData() : specular(Vec4f(0, 0, 0, 1)), constantAtt(1), linearAtt(1), quadAtt(1), type(0), spotExponent(1), spotCutoff(1) {}
		Vec4f diffuse;
		Vec4f specular;
		Vec4f ambient;
		float constantAtt;
		float linearAtt;
		float quadAtt;
		int type;
		Vec4f pos;
		Vec4f direction;
		float spotExponent;
		float spotCutoff;
		float padding[2];
	};

	struct FixedVertex
	{
		FixedVertex(Vec4f POS = Vec4f(0, 0, 0, 1), Vec3f NORM = Vec3f(), Vec2f UV = Vec2f(), Vec4f COLOR = Vec4f()) : pos(POS), norm(NORM), uv(UV), color(COLOR) {}
		FixedVertex(Vec3f POS, Vec3f NORM = Vec3f(), Vec2f UV = Vec2f(), Vec4f COLOR = Vec4f()) : pos(Vec4f(POS, 1)), norm(NORM), uv(UV), color(COLOR) {}
		FixedVertex(Vec2f POS, Vec3f NORM = Vec3f(), Vec2f UV = Vec2f(), Vec4f COLOR = Vec4f()) : pos(Vec4f(POS.x, POS.y, 0, 1)), norm(NORM), uv(UV), color(COLOR) {}
		Vec4f pos;
		Vec3f norm;
		Vec2f uv;
		Vec4f color;
	};

	enum RenderStateFlag
	{
		BATCH_TEXTURE = 1,
		CUSTOM_SHADER_ACTIVE = 2
	};

	AppImplMswRendererDx( App *aApp, RendererDx *aRenderer );
	
#if defined( CINDER_MSW )
	virtual bool	initialize( HWND wnd, HDC dc,  RendererRef sharedRenderer );
#elif defined( CINDER_WINRT)
	virtual bool	initialize( DX_WINDOW_TYPE wnd );
#endif

	virtual void	prepareToggleFullScreen();
	virtual void	finishToggleFullScreen();
	virtual void	kill();
	virtual void	defaultResize() const;
	virtual void	swapBuffers() const;
	virtual void	makeCurrentContext();

	void enableVsync(bool enable = true) { mVsyncEnable = enable; }
	bool vsyncEnabled() const { return mVsyncEnable; }
	void setViewport(int x, int y, int width, int height) const;
	void enableDepthTesting(bool enable = true);
	void enableAlphaBlending(bool premultiplied = false);
	void disableAlphaBlending();
	void enableAdditiveBlending();
	void enableDepthWriting(bool enable = true);

	bool getRenderFlag(RenderStateFlag flag) { return (mStateFlags & flag) == flag; }
	void setRenderFlag(RenderStateFlag flag) { mStateFlags |= flag; }
	void clearRenderFlag(RenderStateFlag flag) { mStateFlags &= ~(flag); }

	//used for texture batch drawing
	const dx::Texture *mCurrentBatchTexture;
	std::vector<std::pair<const dx::Texture*, std::vector<FixedVertex>>> mBatchedTextures;
	
	// matrix stacks
	MatrixStack mModelView;
	MatrixStack mProjection;

	Vec4f mCurrentColor;
	Vec3f mCurrentNormal;
	Vec2f mCurrentUV;
	std::vector<FixedVertex> mImmediateModeVerts;
	GLenum mImmediateModePrimitive;
	bool mLightingEnabled;
	LightData mLights[8];

	RendererDx	*mRenderer;
	D3D_FEATURE_LEVEL mFeatureLevel;
  #if defined( CINDER_WINRT ) || ( _WIN32_WINNT >= 0x0602 )
	ID3D11DeviceContext1 *mDeviceContext;
	IDXGISwapChain1 *mSwapChain;
	ID3D11Device1 *md3dDevice;
  #else
  	ID3D11DeviceContext *mDeviceContext;
	IDXGISwapChain *mSwapChain;
	ID3D11Device *md3dDevice;
  #endif
	ID3D11RenderTargetView *mMainFramebuffer;
	ID3D11Texture2D *mDepthStencilTexture;
	ID3D11DepthStencilView *mDepthStencilView;
	ID3D11Buffer *mCBMatrices;
	ID3D11Buffer *mCBLights;
	ID3D11Buffer *mCBFixedParameters;
	ID3D11BlendState *mBlendState;
	D3D11_BLEND_DESC mBlendDesc;

	ID3D11VertexShader *mFixedColorVertexShader;
	ID3D11PixelShader *mFixedColorPixelShader;
	ID3D11VertexShader *mFixedColorLightVertexShader;
	ID3D11PixelShader *mFixedColorLightPixelShader;
	ID3D11VertexShader *mFixedTextureVertexShader;
	ID3D11PixelShader *mFixedTexturePixelShader;
	ID3D11VertexShader *mFixedTextureLightVertexShader;
	ID3D11PixelShader *mFixedTextureLightPixelShader;

	ID3D11VertexShader *mVboPositionVS;
	ID3D11VertexShader *mVboPositionNormalVS;
	ID3D11VertexShader *mVboPositionTextureVS;
	ID3D11VertexShader *mVboPositionColorVS;
	ID3D11VertexShader *mVboPositionNormalTextureVS;
	ID3D11VertexShader *mVboPositionNormalColorVS;
	ID3D11VertexShader *mVboPositionColorTextureVS;
	ID3D11VertexShader *mVboPositionNormalColorTextureVS;

	ID3D11VertexShader *mVboPositionLightVS;
	ID3D11VertexShader *mVboPositionNormalLightVS;
	ID3D11VertexShader *mVboPositionTextureLightVS;
	ID3D11VertexShader *mVboPositionColorLightVS;
	ID3D11VertexShader *mVboPositionNormalTextureLightVS;
	ID3D11VertexShader *mVboPositionNormalColorLightVS;
	ID3D11VertexShader *mVboPositionColorTextureLightVS;
	ID3D11VertexShader *mVboPositionNormalColorTextureLightVS;

	ID3D11InputLayout *mFixedLayout;
	ID3D11Buffer *mVertexBuffer;
	ID3D11Buffer *mIndexBuffer;

	// no culling, solid fill mode, forward-facing triangles are CCW, no depth bias, no depth bias clamp, anti-aliased lines
	ID3D11RasterizerState *mDefaultRenderState;

	// no depth testing or stencil testing
	ID3D11DepthStencilState *mDepthStencilState;
	D3D11_DEPTH_STENCIL_DESC mDepthStencilDesc;

 protected:
	bool	initializeInternal( DX_WINDOW_TYPE wnd );
	int		initMultisample( int requestedLevelIdx );
	bool	createDevice(UINT createDeviceFlags);
	bool	createDeviceResources();
	bool	createFramebufferResources();

	bool	createShadersFeatureLevel_9_1();
	bool	createShadersFeatureLevel_9_3();
	bool	createShadersFeatureLevel_10_1();
	bool	createShadersFeatureLevel_11_0();
	bool	createShadersFeatureLevel_11_1();

	void	handleLostDevice();
	void	getPlatformWindowDimensions(DX_WINDOW_TYPE wnd, float* width, float* height) const;
	void	releaseNonDeviceResources();
	
	int mStateFlags;
	bool mFullScreen;
	bool mVsyncEnable;
};

} } // namespace cinder::app
