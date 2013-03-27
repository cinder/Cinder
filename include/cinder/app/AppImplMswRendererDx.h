/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

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

#pragma once

#include "cinder/app/App.h"
#include "cinder/app/AppImplMswRenderer.h"
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
		FixedVertex(Vec3f POS = Vec3f(), Vec3f NORM = Vec3f(), Vec2f UV = Vec2f(), Vec4f COLOR = Vec4f()) : pos(POS), norm(NORM), uv(UV), color(COLOR) {}
		Vec3f pos;
		Vec3f norm;
		Vec2f uv;
		Vec4f color;
	};

	enum RenderStateFlag
	{
		BATCH_TEXTURE = 1
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
	ID3D11Device1 *md3dDevice;
	ID3D11DeviceContext1 *mDeviceContext;
	IDXGISwapChain1 *mSwapChain;
	ID3D11RenderTargetView *mMainFramebuffer;
	ID3D11Texture2D *mDepthStencilTexture;
	ID3D11DepthStencilView *mDepthStencilView;
	ID3D11Buffer *mCBMatrices;
	ID3D11Buffer *mCBLights;
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
	bool	createDeviceResources();
	bool	createFramebufferResources();
	void	handleLostDevice();
	void	getPlatformWindowDimensions(DX_WINDOW_TYPE wnd, float* width, float* height) const;
	void	releaseNonDeviceResources();
	
	int mStateFlags;
	bool mFullScreen;
	bool mVsyncEnable;
};

} } // namespace cinder::app
