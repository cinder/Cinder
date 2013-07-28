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


#include "cinder/dx/dx.h"
#include "cinder/dx/DxVbo.h"
#include "cinder/CinderMath.h"
#include "cinder/Vector.h"
#include "cinder/Camera.h"
#include "cinder/TriMesh.h"
#include "cinder/Sphere.h"
#include "cinder/dx/DxTexture.h"
#include "cinder/Text.h"
#include "cinder/PolyLine.h"
#include "cinder/Path2d.h"
#include "cinder/Shape2d.h"
#include "cinder/Triangulate.h"
#include <cmath>
#include <map>
#include "cinder/app/AppBasic.h"
#include "cinder/app/AppImplMswRendererDx.h"
#include "cinder/dx/DxLight.h"
#include "cinder/MatrixStack.h"

#define COLOR_VERTEX (getDxRenderer()->mLightingEnabled) ? getDxRenderer()->mFixedColorLightVertexShader : getDxRenderer()->mFixedColorVertexShader
#define COLOR_PIXEL (getDxRenderer()->mLightingEnabled) ? getDxRenderer()->mFixedColorLightPixelShader : getDxRenderer()->mFixedColorPixelShader
#define TEXTURE_VERTEX (getDxRenderer()->mLightingEnabled) ? getDxRenderer()->mFixedTextureLightVertexShader : getDxRenderer()->mFixedTextureVertexShader
#define TEXTURE_PIXEL (getDxRenderer()->mLightingEnabled) ? getDxRenderer()->mFixedTextureLightPixelShader : getDxRenderer()->mFixedTexturePixelShader

namespace cinder { namespace dx {

// need to have our own model view matrix stack not managed by OpenGL
//MatrixStack getDxRenderer()->mModelView, getDxRenderer()->mProjection;

//struct FixedVertex
//{
//	BasicTriangleVertex(Vec3f POS = Vec3f(), Vec3f NORM = Vec3f(), Vec2f UV = Vec2f(), Vec4f COLOR = Vec4f()) : pos(POS), norm(NORM), uv(UV), color(COLOR) {}
//	Vec3f pos;
//	Vec3f norm;
//	Vec2f uv;
//	Vec4f color;
//};

typedef app::AppImplMswRendererDx::FixedVertex FixedVertex;
typedef app::AppImplMswRendererDx::LightData LightData;

//static Vec4f getDxRenderer()->mCurrentColor(1, 1, 1, 1);
//static Vec3f getDxRenderer()->mCurrentNormal(0, 0, 1);
//static Vec2f getDxRenderer()->mCurrentUV(0, 0);
//static std::vector<FixedVertex> getDxRenderer()->mImmediateModeVerts;
//static GLenum getDxRenderer()->mImmediateModePrimitive;
//static bool getDxRenderer()->mLightingEnabled = false;
//static LightData getDxRenderer()->mLights[8];

app::AppImplMswRendererDx *getDxRenderer()
{
	return ((app::RendererDx*)(&*app::App::get()->getRenderer()))->mImpl;
}

static bool anyShadersActive()
{
	auto dx = getDxRenderer();
	ID3D11VertexShader *vs;
	ID3D11GeometryShader *gs;
	ID3D11PixelShader *ps;
	dx->mDeviceContext->VSGetShader(&vs, NULL, NULL);
	if(vs)
	{
		vs->Release();
		return true;
	}
	dx->mDeviceContext->GSGetShader(&gs, NULL, NULL);
	if(gs)
	{
		gs->Release();
		return true;
	}
	dx->mDeviceContext->PSGetShader(&ps, NULL, NULL);
	if(ps)
	{
		ps->Release();
		return true;
	}
	return false;
}

static void drawQuads()
{
	auto dx = getDxRenderer();

	size_t vertexCount = dx->mImmediateModeVerts.size();
    if(vertexCount < 4) {
		return;
	}

	ID3D11ShaderResourceView *view;
	dx->mDeviceContext->PSGetShaderResources(0, 1, &view);
	ID3D11VertexShader *vs = (view) ? TEXTURE_VERTEX : COLOR_VERTEX;
	ID3D11PixelShader *ps = (view) ? TEXTURE_PIXEL : COLOR_PIXEL;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	dx->mDeviceContext->Map(dx->mCBMatrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	reinterpret_cast<Matrix44f*>(mappedResource.pData)[0] = dx->mProjection.top();
	reinterpret_cast<Matrix44f*>(mappedResource.pData)[1] = dx->mModelView.top();
	dx->mDeviceContext->Unmap(dx->mCBMatrices, 0);
	if(dx->mLightingEnabled)
		dx->mDeviceContext->VSSetConstantBuffers(1, 1, &dx->mCBLights);
	dx->mDeviceContext->VSSetShader(vs, NULL, 0);
	dx->mDeviceContext->VSSetConstantBuffers(0, 1, &dx->mCBMatrices);
	dx->mDeviceContext->PSSetShader(ps, NULL, 0);
	dx->mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	size_t size =  sizeof(FixedVertex);
	FixedVertex *vertices = dx->mImmediateModeVerts.data();

	while(vertexCount > 0) 
	{
		size_t index = 0;
		size_t count = 0;
		dx->mDeviceContext->Map(dx->mVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		FixedVertex *pData = static_cast<FixedVertex *>(mappedResource.pData);
		size_t end = (vertexCount >= D3D_FL9_1_IA_PRIMITIVE_MAX_COUNT) ? D3D_FL9_1_IA_PRIMITIVE_MAX_COUNT : vertexCount;
		while(count < end)
		{
			memcpy(pData++, &vertices[index+0], sizeof(FixedVertex));
			memcpy(pData++, &vertices[index+1], sizeof(FixedVertex));
			memcpy(pData++, &vertices[index+2], sizeof(FixedVertex));
			memcpy(pData++, &vertices[index+0], sizeof(FixedVertex));
			memcpy(pData++, &vertices[index+2], sizeof(FixedVertex));
			memcpy(pData++, &vertices[index+3], sizeof(FixedVertex));
			vertexCount -= 4;
			count += 6;
			index += 4;
		}
		dx->mDeviceContext->Unmap(dx->mVertexBuffer, 0);

		if(count > 0) 
		{
			UINT stride = sizeof(FixedVertex);
			UINT offset = 0;
			dx->mDeviceContext->IASetVertexBuffers(0, 1, &dx->mVertexBuffer, &stride, &offset);
			dx->mDeviceContext->IASetInputLayout(dx->mFixedLayout);
			dx->mDeviceContext->Draw(count, 0);
		}
	}
}




static void applyDxFixedPipeline(const FixedVertex *verts, UINT elements, ID3D11VertexShader *vs, ID3D11PixelShader *ps, D3D_PRIMITIVE_TOPOLOGY topology)
{
	auto dx = getDxRenderer();
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	dx->mDeviceContext->Map(dx->mCBMatrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	reinterpret_cast<Matrix44f*>(mappedResource.pData)[0] = dx->mProjection.top();
	reinterpret_cast<Matrix44f*>(mappedResource.pData)[1] = dx->mModelView.top();
	dx->mDeviceContext->Unmap(dx->mCBMatrices, 0);
	if(dx->mLightingEnabled) {
		dx->mDeviceContext->VSSetConstantBuffers(1, 1, &dx->mCBLights);
	}
	if( ! dx->getRenderFlag( app::AppImplMswRendererDx::CUSTOM_SHADER_ACTIVE ) ) {
		dx->mDeviceContext->VSSetShader(vs, NULL, 0);
		dx->mDeviceContext->PSSetShader(ps, NULL, 0);
	}
	dx->mDeviceContext->VSSetConstantBuffers(0, 1, &dx->mCBMatrices);
	dx->mDeviceContext->IASetPrimitiveTopology(topology);
	dx->mDeviceContext->Map( dx->mVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
	memcpy( mappedResource.pData, verts, sizeof(FixedVertex) * elements );
	dx->mDeviceContext->Unmap( dx->mVertexBuffer, 0 );
	UINT stride = sizeof(FixedVertex);
	UINT offset = 0;
	dx->mDeviceContext->IASetVertexBuffers( 0, 1, &dx->mVertexBuffer, &stride, &offset );
	dx->mDeviceContext->IASetInputLayout( dx->mFixedLayout );
}

void clear( const ColorA &color, bool clearDepthBuffer )
{
	//if(app::AppBasic::get()->getRenderer()->GetRendererType() == app::Renderer::RENDERER_GL)
	//{
	//	glClearColor( color.r, color.g, color.b, color.a );
	//	if( clearDepthBuffer ) {
	//		glDepthMask( GL_TRUE );
	//		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//	}
	//	else
	//		glClear( GL_COLOR_BUFFER_BIT );
	//}
	//else
	{
		auto dx = getDxRenderer();
		dx->mDeviceContext->ClearRenderTargetView(dx->mMainFramebuffer, &color.r);
		if(clearDepthBuffer)
			dx->mDeviceContext->ClearDepthStencilView(dx->mDepthStencilView, D3D11_CLEAR_DEPTH, 1, 0);
	}
}

void enableVerticalSync( bool enable )
{
//	if(app::App::get()->getRenderer()->GetRendererType() == app::Renderer::RENDERER_GL)
//	{
//		GLint sync = ( enable ) ? 1 : 0;
//#if defined( CINDER_MAC )
//		::CGLSetParameter( ::CGLGetCurrentContext(), kCGLCPSwapInterval, &sync );
//#elif defined( CINDER_MSW )
//		if( WGL_EXT_swap_control )
//			::wglSwapIntervalEXT( sync );
//	}
//	else
	{
		getDxRenderer()->enableVsync(enable);
	}
//#else
	//}
//#endif
}

bool isVerticalSyncEnabled()
{
//	if(app::App::get()->getRenderer()->GetRendererType() == app::Renderer::RENDERER_GL)
//	{
//#if defined( CINDER_MAC )
//		GLint enabled;
//		::CGLGetParameter( ::CGLGetCurrentContext(), kCGLCPSwapInterval, &enabled );
//		return enabled > 0;
//#elif defined( CINDER_MSW )
//		if( WGL_EXT_swap_control )
//			return ::wglGetSwapIntervalEXT() > 0;
//		else
//			return true;
//	}
//	else
	{
		return getDxRenderer()->vsyncEnabled();
	}
//#else
//		return true;
//	}
//#endif
}

void setModelView( const Camera &cam )
{
	//if(usingGL())
	//{
	//	glMatrixMode( GL_MODELVIEW );
	//	glLoadMatrixf( cam.getModelViewMatrix().m );
	//}
	getDxRenderer()->mModelView.top() = cam.getModelViewMatrix();
}

void setModelView( const Matrix44f &m )
{
	//if(usingGL())
	//{
	//	glMatrixMode( GL_MODELVIEW );
	//	glLoadMatrixf( cam.getModelViewMatrix().m );
	//}
	getDxRenderer()->mModelView.top() = m;
}

void setProjection( const Camera &cam )
{
	auto dx = getDxRenderer();

	dx->mProjection.top() = cam.getProjectionMatrix();
	//if(usingGL())
	//{
	//	glMatrixMode( GL_PROJECTION );
	//	glLoadMatrixf( cam.getProjectionMatrix().m );
	//}
	//else
	{
		// DX's NDC space goes from [0, 1] whereas OpenGL maps from [-1, 1] so DX's near plane will be too far out
		// need to modify the projection matrix a bit
		float invFarMinusNear = 1.0f / (cam.getFarClip() - cam.getNearClip());
		if(cam.isPersp())
		{
			dx->mProjection.top().m22 -= cam.getNearClip() * invFarMinusNear;
			dx->mProjection.top().m23 *= 0.5f;
		}
		else
		{
			dx->mProjection.top().m22 = invFarMinusNear;
			dx->mProjection.top().m23 = -cam.getNearClip() * invFarMinusNear;
		}
	}
}

void setMatrices( const Camera &cam )
{
	setProjection( cam );
	setModelView( cam );
}

void pushModelView()
{
	//if(app::AppBasic::get()->getRenderer()->GetRendererType() == app::Renderer::RENDERER_GL)
	//{
	//	glMatrixMode( GL_MODELVIEW );
	//	glPushMatrix();
	//}
	getDxRenderer()->mModelView.push();
}

void popModelView()
{
	//if(app::AppBasic::get()->getRenderer()->GetRendererType() == app::Renderer::RENDERER_GL)
	//{
	//	glMatrixMode( GL_MODELVIEW );
	//	glPopMatrix();
	//}
	getDxRenderer()->mModelView.pop();
}

void pushModelView( const Camera &cam )
{
	//if(app::AppBasic::get()->getRenderer()->GetRendererType() == app::Renderer::RENDERER_GL)
	//{
	//	glMatrixMode( GL_MODELVIEW );
	//	glPushMatrix();
	//	glLoadMatrixf( cam.getModelViewMatrix().m );
	//}
	getDxRenderer()->mModelView.push();
	getDxRenderer()->mModelView.top() = cam.getModelViewMatrix();
}

void pushProjection( const Camera &cam )
{
	//if(app::AppBasic::get()->getRenderer()->GetRendererType() == app::Renderer::RENDERER_GL)
	//{
	//	glMatrixMode( GL_PROJECTION );
	//	glPushMatrix();
	//	glLoadMatrixf( cam.getProjectionMatrix().m );
	//}
	getDxRenderer()->mProjection.top() = cam.getProjectionMatrix();
}

void pushMatrices()
{
	//if(app::AppBasic::get()->getRenderer()->GetRendererType() == app::Renderer::RENDERER_GL)
	//{
	//	glMatrixMode( GL_PROJECTION );
	//	glPushMatrix();
	//	glMatrixMode( GL_MODELVIEW );
	//	glPushMatrix();	
	//}
	getDxRenderer()->mModelView.push();
	getDxRenderer()->mProjection.push();
}

void popMatrices()
{
	//if(app::AppBasic::get()->getRenderer()->GetRendererType() == app::Renderer::RENDERER_GL)
	//{
	//	glMatrixMode( GL_PROJECTION );
	//	glPopMatrix();
	//	glMatrixMode( GL_MODELVIEW );
	//	glPopMatrix();
	//}
	getDxRenderer()->mModelView.pop();
	getDxRenderer()->mProjection.pop();
}

void multModelView( const Matrix44f &mtx )
{
	//if(app::AppBasic::get()->getRenderer()->GetRendererType() == app::Renderer::RENDERER_GL)
	//{
	//	glMatrixMode( GL_MODELVIEW );
	//	glMultMatrixf( mtx );
	//}
	getDxRenderer()->mModelView.top() *= mtx;
}

void multProjection( const Matrix44f &mtx )
{
	//if(app::AppBasic::get()->getRenderer()->GetRendererType() == app::Renderer::RENDERER_GL)
	//{
	//	glMatrixMode( GL_PROJECTION );
	//	glMultMatrixf( mtx );
	//}
	getDxRenderer()->mProjection.top() *= mtx;
}

Matrix44f getModelView()
{
	//if(app::AppBasic::get()->getRenderer()->GetRendererType() == app::Renderer::RENDERER_GL)
	//{
	//	Matrix44f result;
	//	glGetFloatv( GL_MODELVIEW_MATRIX, reinterpret_cast<GLfloat*>( &(result.m) ) );
	//	return result;
	//}
	return getDxRenderer()->mModelView.top();
}

Matrix44f getProjection()
{
	//if(app::AppBasic::get()->getRenderer()->GetRendererType() == app::Renderer::RENDERER_GL)
	//{
	//	Matrix44f result;
	//	glGetFloatv( GL_PROJECTION_MATRIX, reinterpret_cast<GLfloat*>( &(result.m) ) );
	//	return result;
	//}
	return getDxRenderer()->mProjection.top();
}

void setMatricesWindowPersp( int screenWidth, int screenHeight, float fovDegrees, float nearPlane, float farPlane, bool originUpperLeft )
{
	CameraPersp cam( screenWidth, screenHeight, fovDegrees, nearPlane, farPlane );
	auto dx = getDxRenderer();
	dx->mProjection.top() = cam.getProjectionMatrix();
	float invFarMinusNear = 1.0f / (cam.getFarClip() - cam.getNearClip());
	dx->mProjection.top().m22 -= cam.getNearClip() * invFarMinusNear;
	dx->mProjection.top().m23 *= 0.5f;
	dx->mModelView.top() = cam.getModelViewMatrix();
	if(originUpperLeft)
	{
		Matrix44f scale = Matrix44f::createScale(Vec3f(1, -1, 1));
		Matrix44f translate = Matrix44f::createTranslation(Vec3f(0, (float)-screenHeight, 0));
		dx->mModelView.top() *= scale * translate;
	}
//	if(usingGL())
//	{
//		glMatrixMode( GL_PROJECTION );
//		glLoadMatrixf( cam.getProjectionMatrix().m );
//
//		glMatrixMode( GL_MODELVIEW );
//		glLoadMatrixf( cam.getModelViewMatrix().m );
//		if( originUpperLeft ) {
//			glScalef( 1.0f, -1.0f, 1.0f );           // invert Y axis so increasing Y goes down.
//			glTranslatef( 0.0f, (float)-screenHeight, 0.0f );       // shift origin up to upper-left corner.
//			glViewport( 0, 0, screenWidth, screenHeight );
//		}
//	}
//#if defined( CINDER_MSW )
//	else
	{
		if(originUpperLeft)
			getDxRenderer()->setViewport(0, 0, screenWidth, screenHeight);
	}
//#endif
}

void setMatricesWindow( int screenWidth, int screenHeight, bool originUpperLeft )
{
//	if(app::AppBasic::get()->getRenderer()->GetRendererType() == app::Renderer::RENDERER_GL)
//	{
//		glMatrixMode( GL_PROJECTION );
//		glLoadIdentity();
//#if defined( CINDER_GLES )
//		if( originUpperLeft )
//			glOrthof( 0, screenWidth, screenHeight, 0, -1.0f, 1.0f );
//		else
//			glOrthof( 0, screenWidth, 0, screenHeight, -1.0f, 1.0f );
//#else	
//		if( originUpperLeft )
//			glOrtho( 0, screenWidth, screenHeight, 0, -1.0f, 1.0f );
//		else
//			glOrtho( 0, screenWidth, 0, screenHeight, -1.0f, 1.0f );
//#endif
//		glMatrixMode( GL_MODELVIEW );
//		glLoadIdentity();
//		glViewport( 0, 0, screenWidth, screenHeight );
//	}
	auto dx = getDxRenderer();
	if(originUpperLeft)
	{
		CameraOrtho ortho(0, (float)screenWidth, (float)screenHeight, 0, -1, 1);
		dx->mProjection.top() = ortho.getProjectionMatrix();
		float invFarMinusNear = 1.0f / (ortho.getFarClip() - ortho.getNearClip());
		dx->mProjection.top().m22 = invFarMinusNear;
		dx->mProjection.top().m23 = -ortho.getNearClip() * invFarMinusNear;
	}
	else
	{
		CameraOrtho ortho(0, (float)screenWidth, 0, (float)screenHeight, -1, 1);
		dx->mProjection.top() = ortho.getProjectionMatrix();
		float invFarMinusNear = 1.0f / (ortho.getFarClip() - ortho.getNearClip());
		dx->mProjection.top().m22 = invFarMinusNear;
		dx->mProjection.top().m23 = -ortho.getNearClip() * invFarMinusNear;
	}
	dx->mModelView.top().setToIdentity();

	dx->setViewport(0, 0, screenWidth, screenHeight);
}

Area getViewport()
{
//	if(usingGL())
//	{
//		GLint params[4];
//		glGetIntegerv( GL_VIEWPORT, params );
//		Area result;
//		return Area( params[0], params[1], params[0] + params[2], params[1] + params[3] );
//	}
//#if defined ( CINDER_MSW )
//	else
	{
		D3D11_VIEWPORT viewport;
		UINT viewport_count = 1;
		getDxRenderer()->mDeviceContext->RSGetViewports(&viewport_count, &viewport);
		return Area ( (int32_t)viewport.TopLeftX, (int32_t)viewport.TopLeftY, (int32_t)(viewport.TopLeftX + viewport.Width), (int32_t)(viewport.TopLeftY + viewport.Height) );
	}
//#endif
}

void setViewport( const Area &area )
{
//	if(usingGL())
//		glViewport( area.x1, area.y1, ( area.x2 - area.x1 ), ( area.y2 - area.y1 ) );
//#if defined ( CINDER_MSW )
//	else
		getDxRenderer()->setViewport(area.x1, area.y1, area.x2 - area.x1, area.y2 - area.y1);
//#endif
}

void translate( const Vec2f &pos )
{
	//if(usingGL())
	//	glTranslatef( pos.x, pos.y, 0 );
	multModelView(Matrix44f::createTranslation(Vec3f(pos, 0)));
}

void translate( const Vec3f &pos )
{
	//if(usingGL())
	//	glTranslatef( pos.x, pos.y, pos.z );
	multModelView(Matrix44f::createTranslation(pos));
}

void scale( const Vec3f &scale )
{
	//if(usingGL())
	//	glScalef( scale.x, scale.y, scale.z );
	multModelView(Matrix44f::createScale(scale));
}

void rotate( const Vec3f &xyz )
{
	//if(usingGL())
	//{
	//	glRotatef( xyz.x, 1.0f, 0.0f, 0.0f );
	//	glRotatef( xyz.y, 0.0f, 1.0f, 0.0f );
	//	glRotatef( xyz.z, 0.0f, 0.0f, 1.0f );
	//}
	Vec3f radians(toRadians(xyz.x), toRadians(xyz.y), toRadians(xyz.z));
	multModelView(Matrix44f::createRotation(radians));
}

void rotate( const Quatf &quat )
{
	Vec3f axis;
	float angle;
	quat.getAxisAngle( &axis, &angle );
	if( math<float>::abs( angle ) > EPSILON_VALUE )
	{
		//if(usingGL())
		//	glRotatef( toDegrees( angle ), axis.x, axis.y, axis.z );
		multModelView(quat.toMatrix44());
	}
}

void begin( GLenum mode )
{
	//if(usingGL())
	//	glBegin(mode);
	getDxRenderer()->mImmediateModeVerts.clear();
	getDxRenderer()->mImmediateModePrimitive = mode;
}

void end()
{
//	if(usingGL())
//		glEnd();
//#if defined( CINDER_MSW )
//	else
	{
		auto dx = getDxRenderer();
		size_t vertexCount = dx->mImmediateModeVerts.size();
		if(vertexCount == 0)
			return;

		std::vector<FixedVertex> *vertexBuffer = &dx->mImmediateModeVerts;
		std::vector<FixedVertex> newVerts;

		dx->mProjection.push();
		dx->mProjection.top() = Matrix44f::identity();
		dx->mModelView.push();
		dx->mModelView.top() = Matrix44f::identity();
		if(dx->mImmediateModePrimitive == GL_QUADS) {
			// optimized draw routine for GL_QUADS
			drawQuads();
		}
		else
		{
			ID3D11ShaderResourceView *view;
			dx->mDeviceContext->PSGetShaderResources(0, 1, &view);
			ID3D11VertexShader *vs = (view) ? TEXTURE_VERTEX : COLOR_VERTEX;
			ID3D11PixelShader *ps = (view) ? TEXTURE_PIXEL : COLOR_PIXEL;
			D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

			switch(dx->mImmediateModePrimitive)
			{
				case GL_POINTS:
					topology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
					break;

				case GL_LINES:
					topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
					break;
					
				case GL_LINE_STRIP:
					topology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
					break;

				case GL_LINE_LOOP:
					dx->mImmediateModeVerts.push_back(dx->mImmediateModeVerts[0]);
					topology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
					break;

				case GL_TRIANGLES:
					topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
					break;

				case GL_TRIANGLE_STRIP:
					topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
					break;
					
				//TODO: need to optimize this
				case GL_POLYGON: //have no flipping clue how to do this but it seems to act like triangle fan
				case GL_TRIANGLE_FAN:
				{
					if(dx->mImmediateModeVerts.size() < 3)
						break;
					for(unsigned i = 2; i < dx->mImmediateModeVerts.size(); ++i)
					{
						newVerts.push_back(dx->mImmediateModeVerts[0]);
						newVerts.push_back(dx->mImmediateModeVerts[i-1]);
						newVerts.push_back(dx->mImmediateModeVerts[i]);
					}
					vertexBuffer = &newVerts;
					topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
					break;
				}

#if 0
				case GL_QUADS:
					{
						if(dx->mImmediateModeVerts.size() < 4)
							break;


						newVerts.reserve(dx->mImmediateModeVerts.size() * 2);
						for(unsigned i = 0; i < dx->mImmediateModeVerts.size(); i += 4)
						{
							newVerts.push_back(dx->mImmediateModeVerts[i+0]);
							newVerts.push_back(dx->mImmediateModeVerts[i+1]);
							newVerts.push_back(dx->mImmediateModeVerts[i+2]);
							newVerts.push_back(dx->mImmediateModeVerts[i+0]);
							newVerts.push_back(dx->mImmediateModeVerts[i+2]);
							newVerts.push_back(dx->mImmediateModeVerts[i+3]);
						}
						vertexBuffer = &newVerts;
						topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
						break;
					}  
#endif // 0

				//TODO: need to optimize this
				case GL_QUAD_STRIP:
				{
					if(dx->mImmediateModeVerts.size() < 4)
						break;
					newVerts.reserve(dx->mImmediateModeVerts.size() * 2);

					newVerts.push_back(dx->mImmediateModeVerts[0]);
					newVerts.push_back(dx->mImmediateModeVerts[1]);
					newVerts.push_back(dx->mImmediateModeVerts[3]);
					newVerts.push_back(dx->mImmediateModeVerts[0]);
					newVerts.push_back(dx->mImmediateModeVerts[3]);
					newVerts.push_back(dx->mImmediateModeVerts[2]);
					
					//ignore the odd vertex
					if(dx->mImmediateModeVerts.size() % 2)
						dx->mImmediateModeVerts.pop_back();
					for(unsigned i = 4; i < dx->mImmediateModeVerts.size(); i += 2)
					{
						newVerts.push_back(dx->mImmediateModeVerts[i-2]);
						newVerts.push_back(dx->mImmediateModeVerts[i-1]);
						newVerts.push_back(dx->mImmediateModeVerts[i+1]);
						newVerts.push_back(dx->mImmediateModeVerts[i-2]);
						newVerts.push_back(dx->mImmediateModeVerts[i+1]);
						newVerts.push_back(dx->mImmediateModeVerts[i-0]);
					}
					vertexBuffer = &newVerts;
					topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
					break;
				}
			}
			vertexCount = vertexBuffer->size();
			size_t verticesProcessed = 0;
			FixedVertex* fv = vertexBuffer->data();

			while(vertexCount >= D3D_FL9_1_IA_PRIMITIVE_MAX_COUNT) 
			{
				applyDxFixedPipeline(&fv[verticesProcessed], D3D_FL9_1_IA_PRIMITIVE_MAX_COUNT , vs, ps, topology);
				dx->mDeviceContext->Draw(D3D_FL9_1_IA_PRIMITIVE_MAX_COUNT, 0);
				verticesProcessed += D3D_FL9_1_IA_PRIMITIVE_MAX_COUNT;
				vertexCount -= D3D_FL9_1_IA_PRIMITIVE_MAX_COUNT;
			}

			if(vertexCount > 0) 
			{
				applyDxFixedPipeline(&fv[verticesProcessed], vertexCount, vs, ps, topology);
				dx->mDeviceContext->Draw(vertexCount, 0);
			}
		}
		dx->mProjection.pop();
		dx->mModelView.pop();
	}
//#endif
}

void vertex( const Vec2f &v )
{
	//if(usingGL())
	//	glVertex2fv(&v.x);
	auto dx = getDxRenderer();

	dx->mImmediateModeVerts.push_back(FixedVertex(dx->mProjection.top() * dx->mModelView.top() * Vec4f(v.x, v.y, 0.0f, 1.0f), dx->mCurrentNormal, dx->mCurrentUV, dx->mCurrentColor));
}

void vertex( float x, float y )
{
	//if(usingGL())
	//	glVertex2f(x, y);
	auto dx = getDxRenderer();
	dx->mImmediateModeVerts.push_back(FixedVertex(dx->mProjection.top() * dx->mModelView.top() * Vec4f(x, y, 0.0f, 1.0f), dx->mCurrentNormal, dx->mCurrentUV, dx->mCurrentColor));
}

void vertex( const Vec3f &v )
{
	//if(usingGL())
	//	glVertex3fv(&v.x);
	auto dx = getDxRenderer();
	dx->mImmediateModeVerts.push_back(FixedVertex(dx->mProjection.top() * dx->mModelView.top() * Vec4f(v, 1), dx->mCurrentNormal, dx->mCurrentUV, dx->mCurrentColor));
}

void vertex( float x, float y, float z )
{
	//if(usingGL())
	//	glVertex3f(x, y, z);
	auto dx = getDxRenderer();
	dx->mImmediateModeVerts.push_back(FixedVertex(dx->mProjection.top() * dx->mModelView.top() * Vec4f(x, y, z, 1), dx->mCurrentNormal, dx->mCurrentUV, dx->mCurrentColor));
}

void texCoord( float x, float y )
{
	//if(usingGL())
	//	glTexCoord2f(x, y);
	getDxRenderer()->mCurrentUV = Vec2f(x, y);
}

void texCoord( const Vec2f &v )
{
	//if(usingGL())
	//	glTexCoord2fv(&v.x);
	getDxRenderer()->mCurrentUV = v;
}

void texCoord( float x, float y, float z )
{
	//if(usingGL())
	//	glTexCoord3f(x, y, z);
	getDxRenderer()->mCurrentUV = Vec2f(x, y);
}

void texCoord( const Vec3f &v )
{
	//if(usingGL())
	//	glTexCoord3fv(&v.x);
	getDxRenderer()->mCurrentUV = v.xy();
}

void color( float r, float g, float b )
{
	//if(usingGL())
	//	glColor3f(r, g, b);
	getDxRenderer()->mCurrentColor = Vec4f(r, g, b, 1);
}

void color( float r, float g, float b, float a )
{
	//if(usingGL())
	//	glColor4f(r, g, b, a);
	getDxRenderer()->mCurrentColor = Vec4f(r, g, b, a);
}

void color( const Color8u &c )
{
	//if(usingGL())
	//	glColor3ub(c.r, c.g, c.b);
	getDxRenderer()->mCurrentColor = Vec4f(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, 1);
}

void color( const ColorA8u &c )
{
	//if(usingGL())
	//	glColor4ub(c.r, c.g, c.b, c.a);
	getDxRenderer()->mCurrentColor = Vec4f(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, c.a / 255.0f);
}

void color( const Color &c )
{
	getDxRenderer()->mCurrentColor = Vec4f(c.r, c.g, c.b, 1);
}

void color( const ColorA &c )
{
	getDxRenderer()->mCurrentColor = Vec4f(c.r, c.g, c.b, c.a);
}

void normal( float x, float y, float z )
{
	getDxRenderer()->mCurrentNormal = Vec3f(x, y, z);
}

void normal( const Vec3f &v )
{
	getDxRenderer()->mCurrentNormal = v;
}

void enableAlphaBlending( bool premultiplied )
{
//	if(usingGL())
//	{
//		glEnable( GL_BLEND );
//		if( ! premultiplied )
//			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
//		else
//			glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
//	}
//#if defined( CINDER_MSW )
//	else
	{
		getDxRenderer()->enableAlphaBlending(premultiplied);
	}
//#endif
}

void disableAlphaBlending()
{
//	if(usingGL())
//		glDisable( GL_BLEND );
//#if defined( CINDER_MSW )
//	else
	{
		getDxRenderer()->disableAlphaBlending();
	}
//#endif
}

void enableAdditiveBlending()
{
//	if(usingGL())
//	{
//		glEnable( GL_BLEND );
//		glBlendFunc( GL_SRC_ALPHA, GL_ONE );
//	}
//#if defined( CINDER_MSW )
//	else
	{
		getDxRenderer()->enableAdditiveBlending();
	}
//#endif
}

void enableAlphaTest( float value, int func )
{
	throw (std::string(__FUNCTION__) + " not implemented yet").c_str();
	//seems as though only OpenGL can do this, it's even deprecated in OpenGL 3.3+
	//if(usingGL())
	//{
	//	glEnable( GL_ALPHA_TEST );
	//	glAlphaFunc( func, value );
	//}
}

void disableAlphaTest()
{
	throw (std::string(__FUNCTION__) + " not implemented yet").c_str();
	//seems as though only OpenGL can do this, it's even deprecated in OpenGL 3.3+
	//if(usingGL())
	//	glDisable( GL_ALPHA_TEST );
}

#if ! defined( CINDER_GLES )
void enableWireframe()
{
	auto dx = getDxRenderer();
	if(dx->mDefaultRenderState) dx->mDefaultRenderState->Release();
	D3D11_RASTERIZER_DESC rd;
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_NONE;
	rd.FrontCounterClockwise = TRUE;
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0;
	rd.SlopeScaledDepthBias = 0;
	rd.DepthClipEnable = TRUE;
	rd.ScissorEnable = FALSE;
	rd.MultisampleEnable = FALSE;
	rd.AntialiasedLineEnable = FALSE;
	dx->md3dDevice->CreateRasterizerState( &rd, &dx->mDefaultRenderState );
	dx->mDeviceContext->RSSetState( dx->mDefaultRenderState );
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
}

void disableWireframe()
{
	auto dx = getDxRenderer();
	if(dx->mDefaultRenderState) dx->mDefaultRenderState->Release();
	D3D11_RASTERIZER_DESC rd;
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_NONE;
	rd.FrontCounterClockwise = TRUE;
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0;
	rd.SlopeScaledDepthBias = 0;
	rd.DepthClipEnable = TRUE;
	rd.ScissorEnable = FALSE;
	rd.MultisampleEnable = FALSE;
	rd.AntialiasedLineEnable = FALSE;
	dx->md3dDevice->CreateRasterizerState( &rd, &dx->mDefaultRenderState );
	dx->mDeviceContext->RSSetState( dx->mDefaultRenderState );
	//glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}
#endif

void disableDepthRead()
{
//	if(usingGL())
//		glDisable( GL_DEPTH_TEST );
//#if defined( CINDER_MSW )
//	else
		getDxRenderer()->enableDepthTesting(false);
//#endif
}

void enableDepthRead( bool enable )
{
//	if(usingGL())
//	{
//		if( enable )
//			glEnable( GL_DEPTH_TEST );
//		else
//			glDisable( GL_DEPTH_TEST );
//	}
//#if defined( CINDER_MSW )
//	else
		getDxRenderer()->enableDepthTesting(enable);
//#endif
}

void enableDepthWrite( bool enable )
{
//	if(usingGL())
//		glDepthMask( (enable) ? GL_TRUE : GL_FALSE );
//#if defined( CINDER_MSW )
//	else
		getDxRenderer()->enableDepthWriting(enable);
//#endif
}

void disableDepthWrite()
{
//	if(usingGL())
//		glDepthMask( GL_FALSE );
//#if defined( CINDER_MSW )
//	else
		getDxRenderer()->enableDepthWriting(false);
//#endif
}

void enableLighting()
{
	getDxRenderer()->mLightingEnabled = true;
}

void disableLighting()
{
	getDxRenderer()->mLightingEnabled = false;
}

void setLight(const Light &light)
{
	auto dx = getDxRenderer();
	LightData &data = dx->mLights[light.getId()];
	data.type = light.getType();
	if(light.enabled())
	{
		data.diffuse = Vec4f(light.getDiffuse().r, light.getDiffuse().g, light.getDiffuse().b, 0);
		data.specular = Vec4f(light.getSpecular().r, light.getSpecular().g, light.getSpecular().b, 1);
		data.ambient = Vec4f(light.getAmbient().r, light.getAmbient().g, light.getAmbient().b, 0);
	}
	else
	{
		data.diffuse = Vec4f();
		data.specular = Vec4f(0, 0, 0, 1);
		data.ambient = Vec4f();
	}
	data.pos = Vec4f(light.getRelativePosition(), 1);
	data.direction = Vec4f(light.getRelativeDirection(), 0);
	data.constantAtt = light.getConstantAtt();
	data.linearAtt = light.getLinearAtt();
	data.quadAtt = light.getQuadraticAtt();
	data.spotExponent = light.getSpotExponent();
	data.spotCutoff = toRadians(light.getSpotCutoff());
	D3D11_MAPPED_SUBRESOURCE subResource;
	dx->mDeviceContext->Map(dx->mCBLights, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	memcpy(subResource.pData, dx->mLights, sizeof(dx->mLights));
	dx->mDeviceContext->Unmap(dx->mCBLights, 0);

}

void drawLine( const Vec2f &start, const Vec2f &end )
{
//	if(usingGL())
//	{
//		float lineVerts[2*2];
//		glEnableClientState( GL_VERTEX_ARRAY );
//		glVertexPointer( 2, GL_FLOAT, 0, lineVerts );
//		lineVerts[0] = start.x; lineVerts[1] = start.y;
//		lineVerts[2] = end.x; lineVerts[3] = end.y;
//		glDrawArrays( GL_LINES, 0, 2 );
//		glDisableClientState( GL_VERTEX_ARRAY );
//	}
//#if defined( CINDER_MSW )
//	else
	{
		auto dx = getDxRenderer();
		FixedVertex verts[2];
		verts[0] = FixedVertex(Vec3f(start, 0), dx->mCurrentNormal, dx->mCurrentUV, dx->mCurrentColor);
		verts[1] = FixedVertex(Vec3f(end, 0), dx->mCurrentNormal, dx->mCurrentUV, dx->mCurrentColor);
		applyDxFixedPipeline(verts, 2, COLOR_VERTEX, COLOR_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_LINELIST );
		dx->mDeviceContext->Draw(2, 0);
	}
//#endif
}

void drawLine( const Vec3f &start, const Vec3f &end )
{
//	if(usingGL())
//	{
//		float lineVerts[3*2];
//		glEnableClientState( GL_VERTEX_ARRAY );
//		glVertexPointer( 3, GL_FLOAT, 0, lineVerts );
//		lineVerts[0] = start.x; lineVerts[1] = start.y; lineVerts[2] = start.z;
//		lineVerts[3] = end.x; lineVerts[4] = end.y; lineVerts[5] = end.z; 
//		glDrawArrays( GL_LINES, 0, 2 );
//		glDisableClientState( GL_VERTEX_ARRAY );
//	}
//#if defined( CINDER_MSW )
//	else
	{
		auto dx = getDxRenderer();
		dx->mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		FixedVertex verts[2];
		verts[0] = FixedVertex(start, dx->mCurrentNormal, dx->mCurrentUV, dx->mCurrentColor);
		verts[1] = FixedVertex(end, dx->mCurrentNormal, dx->mCurrentUV, dx->mCurrentColor);
		applyDxFixedPipeline(verts, 2, COLOR_VERTEX, COLOR_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_LINELIST );
		dx->mDeviceContext->Draw(2, 0);
	}
//#endif
}

namespace {
void drawCubeImpl( const Vec3f &c, const Vec3f &size, bool drawColors )
{
	GLfloat sx = size.x * 0.5f;
	GLfloat sy = size.y * 0.5f;
	GLfloat sz = size.z * 0.5f;
	GLfloat vertices[24*3]={c.x+1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,		// +X
							c.x+1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,		// +Y
							c.x+1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,		// +Z
							c.x+-1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	// -X
							c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	// -Y
							c.x+1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,	c.x+1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz};	// -Z


	static GLfloat normals[24*3]={ 1,0,0,	1,0,0,	1,0,0,	1,0,0,
								  0,1,0,	0,1,0,	0,1,0,	0,1,0,
									0,0,1,	0,0,1,	0,0,1,	0,0,1,
								  -1,0,0,	-1,0,0,	-1,0,0,	-1,0,0,
								  0,-1,0,	0,-1,0,  0,-1,0,0,-1,0,
								  0,0,-1,	0,0,-1,	0,0,-1,	0,0,-1};
	 
	static GLubyte colors[24*4]={	255,0,0,255,	255,0,0,255,	255,0,0,255,	255,0,0,255,	// +X = red
									0,255,0,255,	0,255,0,255,	0,255,0,255,	0,255,0,255,	// +Y = green
									0,0,255,255,	0,0,255,255,	0,0,255,255,	0,0,255,255,	// +Z = blue
									0,255,255,255,	0,255,255,255,	0,255,255,255,	0,255,255,255,	// -X = cyan
									255,0,255,255,	255,0,255,255,	255,0,255,255,	255,0,255,255,	// -Y = purple
									255,255,0,255,	255,255,0,255,	255,255,0,255,	255,255,0,255 };// -Z = yellow

	static GLfloat texs[24*2]={	0,1,	1,1,	1,0,	0,0,
								1,1,	1,0,	0,0,	0,1,
								0,1,	1,1,	1,0,	0,0,							
								1,1,	1,0,	0,0,	0,1,
								1,0,	0,0,	0,1,	1,1,
								1,0,	0,0,	0,1,	1,1 };
	
	static GLubyte elements[6*6] ={	0, 1, 2, 0, 2, 3,
									4, 5, 6, 4, 6, 7,
									8, 9,10, 8, 10,11,
									12,13,14,12,14,15,
									16,17,18,16,18,19,
									20,21,22,20,22,23 };

//	if(usingGL())
//	{
//		glEnableClientState( GL_NORMAL_ARRAY );
//		glNormalPointer( GL_FLOAT, 0, normals );
//
//		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
//		glTexCoordPointer( 2, GL_FLOAT, 0, texs );
//
//		if( drawColors ) {
//			glEnableClientState( GL_COLOR_ARRAY );	
//			glColorPointer( 4, GL_UNSIGNED_BYTE, 0, colors );		
//		}
//
//		glEnableClientState( GL_VERTEX_ARRAY );	 
//		glVertexPointer( 3, GL_FLOAT, 0, vertices );
//
//		glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, elements );
//
//		glDisableClientState( GL_VERTEX_ARRAY );
//		glDisableClientState( GL_TEXTURE_COORD_ARRAY );	 
//		glDisableClientState( GL_NORMAL_ARRAY );
//		if( drawColors )
//			glDisableClientState( GL_COLOR_ARRAY );
//	}
//#if defined( CINDER_MSW )
//	else
	{
		auto dx = getDxRenderer();
		FixedVertex verts[24];
		for(int i = 0; i < 24; ++i)
		{
			int index30 = i * 3;
			int index31 = index30 + 1;
			int index32 = index30 + 2;
			int index20 = i * 2;
			int index21 = index20 + 1;
			int index40 = i * 4;
			int index41 = index40 + 1;
			int index42 = index40 + 2;
			int index43 = index40 + 3;
			verts[i].pos = Vec3f(vertices[index30], vertices[index31], vertices[index32]);
			verts[i].norm = Vec3f(normals[index30], normals[index31], normals[index32]);
			verts[i].uv = Vec2f(texs[index20], texs[index21]);
			if(drawColors)
				verts[i].color = Vec4f(colors[index40] / 255.0f, colors[index41] / 255.0f, colors[index42] / 255.0f, colors[index43] / 255.0f);
			else
				verts[i].color = dx->mCurrentColor;
		}

		// DX11 doesn't support 8-bit indices :(
		GLushort indices16[] = {	0, 1, 2, 0, 2, 3,
									4, 5, 6, 4, 6, 7,
									8, 9,10, 8, 10,11,
									12,13,14,12,14,15,
									16,17,18,16,18,19,
									20,21,22,20,22,23 };
		//D3D11_BOX box = { 0, 0, 0, sizeof( indices16 ), 1, 1 };
		//dx->mDeviceContext->UpdateSubresource(dx->mIndexBuffer, 0, &box, indices16, 0, 0);
		applyDxFixedPipeline( verts, sizeof(verts) / sizeof(verts[0]), COLOR_VERTEX, COLOR_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		dx->mDeviceContext->IASetIndexBuffer(dx->mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
		dx->mDeviceContext->DrawIndexed(36, 0, 0);
	} 
//#endif
}
} // anonymous namespace

void drawCube( const Vec3f &center, const Vec3f &size )
{
	drawCubeImpl( center, size, false );
}

void drawColorCube( const Vec3f &center, const Vec3f &size )
{
	drawCubeImpl( center, size, true );
}

void drawStrokedCube( const Vec3f &center, const Vec3f &size )
{
	Vec3f min = center - size * 0.5f;
	Vec3f max = center + size * 0.5f;

	dx::drawLine( Vec3f(min.x, min.y, min.z), Vec3f(max.x, min.y, min.z) );
	dx::drawLine( Vec3f(max.x, min.y, min.z), Vec3f(max.x, max.y, min.z) );
	dx::drawLine( Vec3f(max.x, max.y, min.z), Vec3f(min.x, max.y, min.z) );
	dx::drawLine( Vec3f(min.x, max.y, min.z), Vec3f(min.x, min.y, min.z) );
	
	dx::drawLine( Vec3f(min.x, min.y, max.z), Vec3f(max.x, min.y, max.z) );
	dx::drawLine( Vec3f(max.x, min.y, max.z), Vec3f(max.x, max.y, max.z) );
	dx::drawLine( Vec3f(max.x, max.y, max.z), Vec3f(min.x, max.y, max.z) );
	dx::drawLine( Vec3f(min.x, max.y, max.z), Vec3f(min.x, min.y, max.z) );
	
	dx::drawLine( Vec3f(min.x, min.y, min.z), Vec3f(min.x, min.y, max.z) );
	dx::drawLine( Vec3f(min.x, max.y, min.z), Vec3f(min.x, max.y, max.z) );
	dx::drawLine( Vec3f(max.x, max.y, min.z), Vec3f(max.x, max.y, max.z) );
	dx::drawLine( Vec3f(max.x, min.y, min.z), Vec3f(max.x, min.y, max.z) );
}

// http://local.wasp.uwa.edu.au/~pbourke/texture_colour/spheremap/  Paul Bourke's sphere code
// We should weigh an alternative that reduces the batch count by using GL_TRIANGLES instead
void drawSphere( const Vec3f &center, float radius, int segments )
{
	if( segments < 0 )
		return;

	float *verts = new float[(segments+1)*2*3];
	float *normals = new float[(segments+1)*2*3];
	float *texCoords = new float[(segments+1)*2*2];
	FixedVertex *vertices = new FixedVertex[(segments+1)*2];

//	if(usingGL())
//	{
//		glEnableClientState( GL_VERTEX_ARRAY );
//		glVertexPointer( 3, GL_FLOAT, 0, verts );
//		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
//		glTexCoordPointer( 2, GL_FLOAT, 0, texCoords );
//		glEnableClientState( GL_NORMAL_ARRAY );
//		glNormalPointer( GL_FLOAT, 0, normals );
//
//		for( int j = 0; j < segments / 2; j++ ) {
//			float theta1 = j * 2 * 3.14159f / segments - ( 3.14159f / 2.0f );
//			float theta2 = (j + 1) * 2 * 3.14159f / segments - ( 3.14159f / 2.0f );
//
//			for( int i = 0; i <= segments; i++ ) {
//				Vec3f e, p;
//				float theta3 = i * 2 * 3.14159f / segments;
//
//				e.x = math<float>::cos( theta1 ) * math<float>::cos( theta3 );
//				e.y = math<float>::sin( theta1 );
//				e.z = math<float>::cos( theta1 ) * math<float>::sin( theta3 );
//				p = e * radius + center;
//				normals[i*3*2+0] = e.x; normals[i*3*2+1] = e.y; normals[i*3*2+2] = e.z;
//				texCoords[i*2*2+0] = 0.999f - i / (float)segments; texCoords[i*2*2+1] = 0.999f - 2 * j / (float)segments;
//				verts[i*3*2+0] = p.x; verts[i*3*2+1] = p.y; verts[i*3*2+2] = p.z;
//
//				e.x = math<float>::cos( theta2 ) * math<float>::cos( theta3 );
//				e.y = math<float>::sin( theta2 );
//				e.z = math<float>::cos( theta2 ) * math<float>::sin( theta3 );
//				p = e * radius + center;
//				normals[i*3*2+3] = e.x; normals[i*3*2+4] = e.y; normals[i*3*2+5] = e.z;
//				texCoords[i*2*2+2] = 0.999f - i / (float)segments; texCoords[i*2*2+3] = 0.999f - 2 * ( j + 1 ) / (float)segments;
//				verts[i*3*2+3] = p.x; verts[i*3*2+4] = p.y; verts[i*3*2+5] = p.z;
//			}
//			glDrawArrays( GL_TRIANGLE_STRIP, 0, (segments + 1)*2 );
//		}
//
//		glDisableClientState( GL_VERTEX_ARRAY );
//		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
//		glDisableClientState( GL_NORMAL_ARRAY );
//	}
//#if defined( CINDER_MSW )
//	else
	{
		auto dx = getDxRenderer();
		for( int j = 0; j < segments / 2; j++ ) {
			float theta1 = j * 2 * 3.14159f / segments - ( 3.14159f / 2.0f );
			float theta2 = (j + 1) * 2 * 3.14159f / segments - ( 3.14159f / 2.0f );

			for( int i = 0; i <= segments; i++ ) {
				Vec3f e, p;
				float theta3 = i * 2 * 3.14159f / segments;

				e.x = math<float>::cos( theta1 ) * math<float>::cos( theta3 );
				e.y = math<float>::sin( theta1 );
				e.z = math<float>::cos( theta1 ) * math<float>::sin( theta3 );
				p = e * radius + center;
				normals[i*3*2+0] = e.x; normals[i*3*2+1] = e.y; normals[i*3*2+2] = e.z;
				texCoords[i*2*2+0] = 0.999f - i / (float)segments; texCoords[i*2*2+1] = 0.999f - 2 * j / (float)segments;
				verts[i*3*2+0] = p.x; verts[i*3*2+1] = p.y; verts[i*3*2+2] = p.z;
				vertices[i*2].pos = p;
				vertices[i*2].norm = e;
				vertices[i*2].uv.x = texCoords[i*2*2+0]; vertices[i*2].uv.y = texCoords[i*2*2+1];
				vertices[i*2].color = dx->mCurrentColor;

				e.x = math<float>::cos( theta2 ) * math<float>::cos( theta3 );
				e.y = math<float>::sin( theta2 );
				e.z = math<float>::cos( theta2 ) * math<float>::sin( theta3 );
				p = e * radius + center;
				normals[i*3*2+3] = e.x; normals[i*3*2+4] = e.y; normals[i*3*2+5] = e.z;
				texCoords[i*2*2+2] = 0.999f - i / (float)segments; texCoords[i*2*2+3] = 0.999f - 2 * ( j + 1 ) / (float)segments;
				verts[i*3*2+3] = p.x; verts[i*3*2+4] = p.y; verts[i*3*2+5] = p.z;
				vertices[i*2+1].pos = p;
				vertices[i*2+1].norm = e;
				vertices[i*2+1].uv.x = texCoords[i*2*2+4]; vertices[i*2+1].uv.y = texCoords[i*2*2+5];
				vertices[i*2+1].color = dx->mCurrentColor;
			}
			UINT dataSize = sizeof( FixedVertex ) * ( segments + 1 ) * 2;
			applyDxFixedPipeline(vertices, (segments + 1) * 2, COLOR_VERTEX, COLOR_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
			dx->mDeviceContext->Draw((segments + 1) * 2, 0);
		}
	}
//#endif
	delete [] verts;
	delete [] normals;
	delete [] texCoords;
	delete [] vertices;
}

void draw( const class Sphere &sphere, int segments )
{
	drawSphere( sphere.getCenter(), sphere.getRadius(), segments );
}

void drawSolidCircle( const Vec2f &center, float radius, int numSegments )
{
	// automatically determine the number of segments from the circumference
	if( numSegments <= 0 ) {
		numSegments = (int)math<double>::floor( radius * M_PI * 2 );
	}
	if( numSegments < 2 ) numSegments = 2;
	
//	if(usingGL())
//	{
//		GLfloat *verts = new float[(numSegments+2)*2];
//		verts[0] = center.x;
//		verts[1] = center.y;
//		for( int s = 0; s <= numSegments; s++ ) {
//			float t = s / (float)numSegments * 2.0f * 3.14159f;
//			verts[(s+1)*2+0] = center.x + math<float>::cos( t ) * radius;
//			verts[(s+1)*2+1] = center.y + math<float>::sin( t ) * radius;
//		}
//		glEnableClientState( GL_VERTEX_ARRAY );
//		glVertexPointer( 2, GL_FLOAT, 0, verts );
//		glDrawArrays( GL_TRIANGLE_FAN, 0, numSegments + 2 );
//		glDisableClientState( GL_VERTEX_ARRAY );
//		delete [] verts;
//	}
//#if defined( CINDER_MSW )
//	else
	{
		auto dx = getDxRenderer();
		FixedVertex *verts = new FixedVertex[3*(1+numSegments)];
		for( int s = 0; s < numSegments; s++ ) {
			float t1 = s / (float)numSegments * 2.0f * 3.14159f;
			float t2 = (s+1) / (float)numSegments * 2.0f * 3.14159f;
			verts[s*3].pos = Vec4f(center.x, center.y, 0, 1);
			verts[s*3].norm = dx->mCurrentNormal;
			verts[s*3].uv = dx->mCurrentUV;
			verts[s*3].color = dx->mCurrentColor;
			verts[s*3+1] = verts[s*3+2] = verts[s*3];
			verts[s*3+1].pos.x += math<float>::cos( t1 ) * radius;
			verts[s*3+1].pos.y += math<float>::sin( t1 ) * radius;
			verts[s*3+2].pos.x += math<float>::cos( t2 ) * radius;
			verts[s*3+2].pos.y += math<float>::sin( t2 ) * radius;
		}

		applyDxFixedPipeline( verts, 3 * numSegments, COLOR_VERTEX, COLOR_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		dx->mDeviceContext->Draw((1 + numSegments) * 3, 0);
		delete [] verts;
	}
//#endif
}

void drawStrokedCircle( const Vec2f &center, float radius, int numSegments )
{
	// automatically determine the number of segments from the circumference
	if( numSegments <= 0 ) {
		numSegments = (int)math<double>::floor( radius * M_PI * 2 );
	}
	if( numSegments < 2 ) numSegments = 2;
	
//	if(usingGL())
//	{
//		GLfloat *verts = new float[numSegments*2];
//		for( int s = 0; s < numSegments; s++ ) {
//			float t = s / (float)numSegments * 2.0f * 3.14159f;
//			verts[s*2+0] = center.x + math<float>::cos( t ) * radius;
//			verts[s*2+1] = center.y + math<float>::sin( t ) * radius;
//		}
//		glEnableClientState( GL_VERTEX_ARRAY );
//		glVertexPointer( 2, GL_FLOAT, 0, verts );
//		glDrawArrays( GL_LINE_LOOP, 0, numSegments );
//		glDisableClientState( GL_VERTEX_ARRAY );
//		delete [] verts;
//	}
//#if defined( CINDER_MSW )
//	else
	{
		auto dx = getDxRenderer();
		FixedVertex *verts = new FixedVertex[numSegments+1];
		for( int s = 0; s <= numSegments; s++ ) {
			float t = s / (float)numSegments * 2.0f * 3.14159f;
			verts[s].pos.x = center.x + math<float>::cos( t ) * radius;
			verts[s].pos.y = center.y + math<float>::sin( t ) * radius;
			verts[s].norm = dx->mCurrentNormal;
			verts[s].color = dx->mCurrentColor;
			verts[s].uv = dx->mCurrentUV;
		}

		applyDxFixedPipeline( verts, numSegments + 1, COLOR_VERTEX, COLOR_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP );
		dx->mDeviceContext->Draw(numSegments + 1, 0);

		delete [] verts;
	}
//#endif
}

void drawSolidEllipse( const Vec2f &center, float radiusX, float radiusY, int numSegments )
{
	// automatically determine the number of segments from the circumference
	if( numSegments <= 0 ) {
		numSegments = (int)math<double>::floor( std::max(radiusX,radiusY) * M_PI * 2 );
	}
	if( numSegments < 2 ) numSegments = 2;
	
//	if(usingGL())
//	{
//		GLfloat *verts = new float[(numSegments+2)*2];
//		verts[0] = center.x;
//		verts[1] = center.y;
//		for( int s = 0; s <= numSegments; s++ ) {
//			float t = s / (float)numSegments * 2.0f * 3.14159f;
//			verts[(s+1)*2+0] = center.x + math<float>::cos( t ) * radiusX;
//			verts[(s+1)*2+1] = center.y + math<float>::sin( t ) * radiusY;
//		}
//		glEnableClientState( GL_VERTEX_ARRAY );
//		glVertexPointer( 2, GL_FLOAT, 0, verts );
//		glDrawArrays( GL_TRIANGLE_FAN, 0, numSegments + 2 );
//		glDisableClientState( GL_VERTEX_ARRAY );
//		delete [] verts;
//	}
//#if defined( CINDER_MSW )
//	else
	{
		auto dx = getDxRenderer();
		FixedVertex *verts = new FixedVertex[3*(1+numSegments)];
		for( int s = 0; s <= numSegments; s++ ) {
			float t1 = s / (float)numSegments * 2.0f * 3.14159f;
			float t2 = (s+1) / (float)numSegments * 2.0f * 3.14159f;
			verts[s*3].pos = Vec3f(center, 0);
			verts[s*3].norm = dx->mCurrentNormal;
			verts[s*3].uv = dx->mCurrentUV;
			verts[s*3].color = dx->mCurrentColor;
			verts[s*3+1] = verts[s*3+2] = verts[s*3];
			verts[s*3+1].pos.x += math<float>::cos( t1 ) * radiusX;
			verts[s*3+1].pos.y += math<float>::sin( t1 ) * radiusY;
			verts[s*3+2].pos.x += math<float>::cos( t2 ) * radiusX;
			verts[s*3+2].pos.y += math<float>::sin( t2 ) * radiusY;
		}

		UINT dataSize = sizeof( FixedVertex ) * ( (1 + numSegments) * 3);
		applyDxFixedPipeline( verts, (1 + numSegments) * 3, COLOR_VERTEX, COLOR_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		dx->mDeviceContext->Draw((1 + numSegments) * 3, 0);
		delete [] verts;
	}
//#endif
}

void drawStrokedEllipse( const Vec2f &center, float radiusX, float radiusY, int numSegments )
{
	// automatically determine the number of segments from the circumference
	if( numSegments <= 0 ) {
		numSegments = (int)math<double>::floor( std::max(radiusX,radiusY) * M_PI * 2 );
	}
	if( numSegments < 2 ) numSegments = 2;
	
//	if(usingGL())
//	{
//		GLfloat *verts = new float[numSegments*2];
//		for( int s = 0; s < numSegments; s++ ) {
//			float t = s / (float)numSegments * 2.0f * 3.14159f;
//			verts[s*2+0] = center.x + math<float>::cos( t ) * radiusX;
//			verts[s*2+1] = center.y + math<float>::sin( t ) * radiusY;
//		}
//		glEnableClientState( GL_VERTEX_ARRAY );
//		glVertexPointer( 2, GL_FLOAT, 0, verts );
//		glDrawArrays( GL_LINE_LOOP, 0, numSegments );
//		glDisableClientState( GL_VERTEX_ARRAY );
//		delete [] verts;
//	}
//#if defined( CINDER_MSW )
//	else
	{
		auto dx = getDxRenderer();
		FixedVertex *verts = new FixedVertex[numSegments+1];
		for( int s = 0; s <= numSegments; s++ ) {
			float t = s / (float)numSegments * 2.0f * 3.14159f;
			verts[s].pos.x = center.x + math<float>::cos( t ) * radiusX;
			verts[s].pos.y = center.y + math<float>::sin( t ) * radiusY;
			verts[s].color = dx->mCurrentColor;
			verts[s].norm = dx->mCurrentNormal;
			verts[s].uv = dx->mCurrentUV;
		}
		applyDxFixedPipeline( verts, numSegments + 1, COLOR_VERTEX, COLOR_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP );
		dx->mDeviceContext->Draw(numSegments + 1, 0);
		delete [] verts;
	}
//#endif
}

void drawSolidRect( const Rectf &rect, bool textureRectangle )
{
//	if(usingGL())
//	{
//		glEnableClientState( GL_VERTEX_ARRAY );
//		GLfloat verts[8];
//		glVertexPointer( 2, GL_FLOAT, 0, verts );
//		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
//		GLfloat texCoords[8];
//		glTexCoordPointer( 2, GL_FLOAT, 0, texCoords );
//		verts[0*2+0] = rect.getX2(); texCoords[0*2+0] = ( textureRectangle ) ? rect.getX2() : 1;
//		verts[0*2+1] = rect.getY1(); texCoords[0*2+1] = ( textureRectangle ) ? rect.getY1() : 0;
//		verts[1*2+0] = rect.getX1(); texCoords[1*2+0] = ( textureRectangle ) ? rect.getX1() : 0;
//		verts[1*2+1] = rect.getY1(); texCoords[1*2+1] = ( textureRectangle ) ? rect.getY1() : 0;
//		verts[2*2+0] = rect.getX2(); texCoords[2*2+0] = ( textureRectangle ) ? rect.getX2() : 1;
//		verts[2*2+1] = rect.getY2(); texCoords[2*2+1] = ( textureRectangle ) ? rect.getY2() : 1;
//		verts[3*2+0] = rect.getX1(); texCoords[3*2+0] = ( textureRectangle ) ? rect.getX1() : 0;
//		verts[3*2+1] = rect.getY2(); texCoords[3*2+1] = ( textureRectangle ) ? rect.getY2() : 1;
//
//		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
//
//		glDisableClientState( GL_VERTEX_ARRAY );
//		glDisableClientState( GL_TEXTURE_COORD_ARRAY );	
//	}
//#if defined( CINDER_MSW )
//	else
	{
		auto dx = getDxRenderer();
		FixedVertex verts[4] = {
			FixedVertex(Vec3f(rect.getX2(), rect.getY1(), 0), dx->mCurrentNormal, Vec2f((textureRectangle) ? rect.getX2() : 1, (textureRectangle) ? rect.getY1() : 0), dx->mCurrentColor),
			FixedVertex(Vec3f(rect.getX1(), rect.getY1(), 0), dx->mCurrentNormal, Vec2f((textureRectangle) ? rect.getX1() : 1, (textureRectangle) ? rect.getY1() : 0), dx->mCurrentColor),
			FixedVertex(Vec3f(rect.getX2(), rect.getY2(), 0), dx->mCurrentNormal, Vec2f((textureRectangle) ? rect.getX2() : 1, (textureRectangle) ? rect.getY2() : 0), dx->mCurrentColor),
			FixedVertex(Vec3f(rect.getX1(), rect.getY2(), 0), dx->mCurrentNormal, Vec2f((textureRectangle) ? rect.getX1() : 1, (textureRectangle) ? rect.getY2() : 0), dx->mCurrentColor)
		};
		applyDxFixedPipeline( verts, 4, COLOR_VERTEX, COLOR_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		dx->mDeviceContext->Draw( 4, 0 );
	}
//#endif
}

void drawStrokedRect( const Rectf &rect )
{
//	if(usingGL())
//	{
//		GLfloat verts[8];
//		verts[0] = rect.getX1();	verts[1] = rect.getY1();
//		verts[2] = rect.getX2();	verts[3] = rect.getY1();
//		verts[4] = rect.getX2();	verts[5] = rect.getY2();
//		verts[6] = rect.getX1();	verts[7] = rect.getY2();
//		glEnableClientState( GL_VERTEX_ARRAY );
//		glVertexPointer( 2, GL_FLOAT, 0, verts );
//		glDrawArrays( GL_LINE_LOOP, 0, 4 );
//		glDisableClientState( GL_VERTEX_ARRAY );
//	}
//#if defined( CINDER_MSW )
//	else
	{
		auto dx = getDxRenderer();
		FixedVertex verts[5] = {
			FixedVertex(Vec3f(rect.getX1(), rect.getY1(), 0), dx->mCurrentNormal, dx->mCurrentUV, dx->mCurrentColor),
			FixedVertex(Vec3f(rect.getX2(), rect.getY1(), 0), dx->mCurrentNormal, dx->mCurrentUV, dx->mCurrentColor),
			FixedVertex(Vec3f(rect.getX2(), rect.getY2(), 0), dx->mCurrentNormal, dx->mCurrentUV, dx->mCurrentColor),
			FixedVertex(Vec3f(rect.getX1(), rect.getY2(), 0), dx->mCurrentNormal, dx->mCurrentUV, dx->mCurrentColor),
			FixedVertex(Vec3f(rect.getX1(), rect.getY1(), 0), dx->mCurrentNormal, dx->mCurrentUV, dx->mCurrentColor),
		};
		applyDxFixedPipeline(verts, 5, COLOR_VERTEX, COLOR_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		dx->mDeviceContext->Draw(5, 0);
	}
//#endif
}

void drawSolidRoundedRect( const Rectf &r, float cornerRadius, int numSegmentsPerCorner )
{
	// automatically determine the number of segments from the circumference
	if( numSegmentsPerCorner <= 0 ) {
		numSegmentsPerCorner = (int)math<double>::floor( cornerRadius * M_PI * 2 / 4 );
	}
	if( numSegmentsPerCorner < 2 ) numSegmentsPerCorner = 2;

	Vec2f center = r.getCenter();

//	if(usingGL())
//	{
//		GLfloat *verts = new float[(numSegmentsPerCorner+2)*2*4+4];
//		verts[0] = center.x;
//		verts[1] = center.y;
//		size_t tri = 1;
//		const float angleDelta = 1 / (float)numSegmentsPerCorner * (float)M_PI / 2;
//		const float cornerCenterVerts[8] = { r.x2 - cornerRadius, r.y2 - cornerRadius, r.x1 + cornerRadius, r.y2 - cornerRadius,
//				r.x1 + cornerRadius, r.y1 + cornerRadius, r.x2 - cornerRadius, r.y1 + cornerRadius };
//		for( size_t corner = 0; corner < 4; ++corner ) {
//			float angle = corner * (float)M_PI / 2.0f;
//			Vec2f cornerCenter( cornerCenterVerts[corner*2+0], cornerCenterVerts[corner*2+1] );
//			for( int s = 0; s <= numSegmentsPerCorner; s++ ) {
//				Vec2f pt( cornerCenter.x + math<float>::cos( angle ) * cornerRadius, cornerCenter.y + math<float>::sin( angle ) * cornerRadius );
//				verts[tri*2+0] = pt.x;
//				verts[tri*2+1] = pt.y;
//				++tri;
//				angle += angleDelta;
//			}
//		}
//		// close it off
//		verts[tri*2+0] = r.x2;
//		verts[tri*2+1] = r.y2 - cornerRadius;
//		glEnableClientState( GL_VERTEX_ARRAY );
//		glVertexPointer( 2, GL_FLOAT, 0, verts );
//		glDrawArrays( GL_TRIANGLE_FAN, 0, (numSegmentsPerCorner+1) * 4 + 2 );
//		glDisableClientState( GL_VERTEX_ARRAY );
//		delete [] verts;
//	}
//#if defined( CINDER_MSW )
//	else
	{
		auto dx = getDxRenderer();
		FixedVertex *verts = new FixedVertex[(numSegmentsPerCorner * 4 + 4) * 3];
		const float angleDelta = 1 / (float)numSegmentsPerCorner * (float)M_PI / 2;
		const float cornerCenterVerts[8] = { r.x2 - cornerRadius, r.y2 - cornerRadius, r.x1 + cornerRadius, r.y2 - cornerRadius,
				r.x1 + cornerRadius, r.y1 + cornerRadius, r.x2 - cornerRadius, r.y1 + cornerRadius };
		int currentVert = 0;
		for( size_t corner = 0; corner < 4; ++corner ) {
			float angle = corner * (float)M_PI / 2.0f;
			Vec2f cornerCenter( cornerCenterVerts[corner*2+0], cornerCenterVerts[corner*2+1] );
			FixedVertex vert(Vec3f(center, 0), dx->mCurrentNormal, dx->mCurrentUV, dx->mCurrentColor);
			verts[currentVert++] = vert;
			Vec2f pt( cornerCenter.x + math<float>::cos( angle ) * cornerRadius, cornerCenter.y + math<float>::sin( angle ) * cornerRadius );
			vert.pos = Vec3f(pt, 0);
			verts[currentVert++] = vert;
			angle += angleDelta;
			pt = Vec2f( cornerCenter.x + math<float>::cos( angle ) * cornerRadius, cornerCenter.y + math<float>::sin( angle ) * cornerRadius );
			vert.pos = Vec3f(pt, 0);
			verts[currentVert++] = vert;
			angle += angleDelta;
			for( int s = 1; s < numSegmentsPerCorner; s++ ) {
				vert.pos = Vec3f(center, 0);
				verts[currentVert++] = vert;
				verts[currentVert] = verts[currentVert - 2];
				++currentVert;
				pt = Vec2f( cornerCenter.x + math<float>::cos( angle ) * cornerRadius, cornerCenter.y + math<float>::sin( angle ) * cornerRadius );
				vert.pos = Vec3f(pt, 0);
				verts[currentVert++] = vert;
				angle += angleDelta;
			}
			angle = (corner + 1) * (float)M_PI / 2.0f;
			vert.pos = Vec3f(center, 0);
			verts[currentVert++] = vert;
			verts[currentVert] = verts[currentVert - 2];
			++currentVert;
			cornerCenter.x = cornerCenterVerts[(corner*2+2)%8];
			cornerCenter.y = cornerCenterVerts[(corner*2+3)%8];
			pt = Vec2f( cornerCenter.x + math<float>::cos( angle ) * cornerRadius, cornerCenter.y + math<float>::sin( angle ) * cornerRadius );
			vert.pos = Vec3f(pt, 0);
			verts[currentVert++] = vert;
		}
		applyDxFixedPipeline(verts, (numSegmentsPerCorner * 4 + 4) * 3, COLOR_VERTEX, COLOR_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		dx->mDeviceContext->Draw(currentVert, 0);
		delete [] verts;
	}
//#endif
}

void drawStrokedRoundedRect( const Rectf &r, float cornerRadius, int numSegmentsPerCorner )
{
	// automatically determine the number of segments from the circumference
	if( numSegmentsPerCorner <= 0 ) {
		numSegmentsPerCorner = (int)math<double>::floor( cornerRadius * M_PI * 2 / 4 );
	}
	if( numSegmentsPerCorner < 2 ) numSegmentsPerCorner = 2;

//	if(usingGL())
//	{
//		GLfloat *verts = new float[(numSegmentsPerCorner+2)*2*4];
//		size_t tri = 0;
//		const float angleDelta = 1 / (float)numSegmentsPerCorner * (float)M_PI / 2;
//		const float cornerCenterVerts[8] = { r.x2 - cornerRadius, r.y2 - cornerRadius, r.x1 + cornerRadius, r.y2 - cornerRadius,
//				r.x1 + cornerRadius, r.y1 + cornerRadius, r.x2 - cornerRadius, r.y1 + cornerRadius };
//		for( size_t corner = 0; corner < 4; ++corner ) {
//			float angle = corner * (float)M_PI / 2.0f;
//			Vec2f cornerCenter( cornerCenterVerts[corner*2+0], cornerCenterVerts[corner*2+1] );
//			for( int s = 0; s <= numSegmentsPerCorner; s++ ) {
//				Vec2f pt( cornerCenter.x + math<float>::cos( angle ) * cornerRadius, cornerCenter.y + math<float>::sin( angle ) * cornerRadius );
//				verts[tri*2+0] = pt.x;
//				verts[tri*2+1] = pt.y;
//				++tri;
//				angle += angleDelta;
//			}
//		}
//		glEnableClientState( GL_VERTEX_ARRAY );
//		glVertexPointer( 2, GL_FLOAT, 0, verts );
//		glDrawArrays( GL_LINE_LOOP, 0, tri );
//		glDisableClientState( GL_VERTEX_ARRAY );
//		delete [] verts;
//	}
//#if defined( CINDER_MSW )
//	else
	{
		auto dx = getDxRenderer();
		FixedVertex *verts = new FixedVertex[(numSegmentsPerCorner + 1) * 4 + 1];
		const float angleDelta = 1 / (float)numSegmentsPerCorner * (float)M_PI / 2;
		const float cornerCenterVerts[8] = { r.x2 - cornerRadius, r.y2 - cornerRadius, r.x1 + cornerRadius, r.y2 - cornerRadius,
				r.x1 + cornerRadius, r.y1 + cornerRadius, r.x2 - cornerRadius, r.y1 + cornerRadius };
		int tri = 0;
		FixedVertex vert(Vec3f(), dx->mCurrentNormal, dx->mCurrentUV, dx->mCurrentColor);
		for( size_t corner = 0; corner < 4; ++corner ) {
			float angle = corner * (float)M_PI / 2.0f;
			Vec2f cornerCenter( cornerCenterVerts[corner*2+0], cornerCenterVerts[corner*2+1] );
			for( int s = 0; s <= numSegmentsPerCorner; s++ ) {
				Vec2f pt( cornerCenter.x + math<float>::cos( angle ) * cornerRadius, cornerCenter.y + math<float>::sin( angle ) * cornerRadius );
				vert.pos = Vec3f(pt, 0);
				verts[tri++] = vert;
				angle += angleDelta;
			}
		}
		verts[tri++] = verts[0];
		applyDxFixedPipeline(verts, (numSegmentsPerCorner + 1) * 4 + 1, COLOR_VERTEX, COLOR_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		dx->mDeviceContext->Draw(tri, 0);
		delete [] verts;
	}
//#endif
}
	
void drawSolidTriangle( const Vec2f &pt1, const Vec2f &pt2, const Vec2f &pt3 )
{
	Vec2f pts[3] = { pt1, pt2, pt3 };
	drawSolidTriangle( pts );
}

void drawSolidTriangle( const Vec2f pts[3] )
{
//	if(usingGL())
//	{
//		glEnableClientState( GL_VERTEX_ARRAY );
//		glVertexPointer( 2, GL_FLOAT, 0, &pts[0].x );
//		glDrawArrays( GL_TRIANGLES, 0, 3 );
//		glDisableClientState( GL_VERTEX_ARRAY );
//	}
//#if defined( CINDER_MSW )
//	else
	{
		auto dx = getDxRenderer();
		FixedVertex verts[3];
		for(int i = 0; i < 3; ++i)
			verts[i] = FixedVertex(Vec3f(pts[i], 0), dx->mCurrentNormal, dx->mCurrentUV, dx->mCurrentColor);
		applyDxFixedPipeline(verts, 3, COLOR_VERTEX, COLOR_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		dx->mDeviceContext->Draw(3, 0);
	}
//#endif
}
	
void drawSolidTriangle( const Vec2f &pt1, const Vec2f &pt2, const Vec2f &pt3, const Vec2f &texPt1, const Vec2f &texPt2, const Vec2f &texPt3 )
{
	Vec2f pts[3] = { pt1, pt2, pt3 };
	Vec2f texCoords[3] = { texPt1, texPt2, texPt3 };
	drawSolidTriangle( pts, texCoords );
}
	
void drawSolidTriangle( const Vec2f pts[3], const Vec2f texCoord[3] )
{
//	if(usingGL())
//	{
//		glEnableClientState( GL_VERTEX_ARRAY );
//		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
//		glVertexPointer( 2, GL_FLOAT, 0, &pts[0].x );
//		glTexCoordPointer( 2, GL_FLOAT, 0, &texCoord[0].x );	
//		glDrawArrays( GL_TRIANGLES, 0, 3 );
//		glDisableClientState( GL_VERTEX_ARRAY );
//		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
//	}
//#if defined( CINDER_MSW )
//	else
	{
		auto dx = getDxRenderer();
		FixedVertex verts[3];
		for(int i = 0; i < 3; ++i)
			verts[i] = FixedVertex(Vec3f(pts[i], 0), dx->mCurrentNormal, texCoord[i], dx->mCurrentColor);
		applyDxFixedPipeline(verts, 3, COLOR_VERTEX, COLOR_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		dx->mDeviceContext->Draw(3, 0);
	}
//#endif
}

void drawStrokedTriangle( const Vec2f &pt1, const Vec2f &pt2, const Vec2f &pt3 )
{
	Vec2f pts[3] = { pt1, pt2, pt3 };
	drawStrokedTriangle( pts );
}

void drawStrokedTriangle( const Vec2f pts[3] )
{
//	if(usingGL())
//	{
//		glEnableClientState( GL_VERTEX_ARRAY );
//		glVertexPointer( 2, GL_FLOAT, 0, &pts[0].x );
//		glDrawArrays( GL_LINE_LOOP, 0, 3 );
//		glDisableClientState( GL_VERTEX_ARRAY );
//	}
//#if defined( CINDER_MSW )
//	else
	{
		auto dx = getDxRenderer();
		FixedVertex verts[4];
		for(int i = 0; i < 3; ++i)
			verts[i] = FixedVertex(Vec3f(pts[i], 0), dx->mCurrentNormal, dx->mCurrentUV, dx->mCurrentColor);
		verts[3] = verts[0];
		applyDxFixedPipeline(verts, 4, COLOR_VERTEX, COLOR_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		dx->mDeviceContext->Draw(4, 0);
	}
//#endif
}

void drawCoordinateFrame( float axisLength, float headLength, float headRadius )
{
//	if(usingGL())
//	{
//		glColor4ub( 255, 0, 0, 255 );
//		drawVector( Vec3f::zero(), Vec3f::xAxis() * axisLength, headLength, headRadius );
//		glColor4ub( 0, 255, 0, 255 );
//		drawVector( Vec3f::zero(), Vec3f::yAxis() * axisLength, headLength, headRadius );
//		glColor4ub( 0, 0, 255, 255 );
//		drawVector( Vec3f::zero(), Vec3f::zAxis() * axisLength, headLength, headRadius );
//	}
//#if defined( CINDER_MSW )
//	else
	{
		auto dx = getDxRenderer();
		dx->mCurrentColor = Vec4f(1, 0, 0, 1);
		drawVector( Vec3f::zero(), Vec3f::xAxis() * axisLength, headLength, headRadius );
		dx->mCurrentColor = Vec4f(0, 1, 0, 1);
		drawVector( Vec3f::zero(), Vec3f::yAxis() * axisLength, headLength, headRadius );
		dx->mCurrentColor = Vec4f(0, 0, 1, 1);
		drawVector( Vec3f::zero(), Vec3f::zAxis() * axisLength, headLength, headRadius );
	}
//#endif
}

void drawVector( const Vec3f &start, const Vec3f &end, float headLength, float headRadius )
{
	const int NUM_SEGMENTS = 32;
	//float lineVerts[3*2];
	Vec3f coneVerts[NUM_SEGMENTS+2];
//	if(usingGL())
//	{
//		glEnableClientState( GL_VERTEX_ARRAY );
//		glVertexPointer( 3, GL_FLOAT, 0, lineVerts );
//		lineVerts[0] = start.x; lineVerts[1] = start.y; lineVerts[2] = start.z;
//		lineVerts[3] = end.x; lineVerts[4] = end.y; lineVerts[5] = end.z;	
//		glDrawArrays( GL_LINES, 0, 2 );
//	
//		// Draw the cone
//		Vec3f axis = ( end - start ).normalized();
//		Vec3f temp = ( axis.dot( Vec3f::yAxis() ) > 0.999f ) ? axis.cross( Vec3f::xAxis() ) : axis.cross( Vec3f::yAxis() );
//		Vec3f left = axis.cross( temp ).normalized();
//		Vec3f up = axis.cross( left ).normalized();
//
//		glVertexPointer( 3, GL_FLOAT, 0, &coneVerts[0].x );
//		coneVerts[0] = Vec3f( end + axis * headLength );
//		for( int s = 0; s <= NUM_SEGMENTS; ++s ) {
//			float t = s / (float)NUM_SEGMENTS;
//			coneVerts[s+1] = Vec3f( end + left * headRadius * math<float>::cos( t * 2 * 3.14159f )
//				+ up * headRadius * math<float>::sin( t * 2 * 3.14159f ) );
//		}
//		glDrawArrays( GL_TRIANGLE_FAN, 0, NUM_SEGMENTS+2 );
//
//		// draw the cap
//		glVertexPointer( 3, GL_FLOAT, 0, &coneVerts[0].x );
//		coneVerts[0] = end;
//		for( int s = 0; s <= NUM_SEGMENTS; ++s ) {
//			float t = s / (float)NUM_SEGMENTS;
//			coneVerts[s+1] = Vec3f( end - left * headRadius * math<float>::cos( t * 2 * 3.14159f )
//				+ up * headRadius * math<float>::sin( t * 2 * 3.14159f ) );
//		}
//		glDrawArrays( GL_TRIANGLE_FAN, 0, NUM_SEGMENTS+2 );
//
//		glDisableClientState( GL_VERTEX_ARRAY );
//	}
//#if defined( CINDER_MSW )
//	else
	{
		drawLine(start, end);
	
		// Draw the cone
		Vec3f axis = ( end - start ).normalized();
		Vec3f temp = ( axis.dot( Vec3f::yAxis() ) > 0.999f ) ? axis.cross( Vec3f::xAxis() ) : axis.cross( Vec3f::yAxis() );
		Vec3f left = axis.cross( temp ).normalized();
		Vec3f up = axis.cross( left ).normalized();

		coneVerts[0] = Vec3f( end + axis * headLength );
		for( int s = 0; s <= NUM_SEGMENTS; ++s ) {
			float t = s / (float)NUM_SEGMENTS;
			coneVerts[s+1] = Vec3f( end + left * headRadius * math<float>::cos( t * 2 * 3.14159f )
				+ up * headRadius * math<float>::sin( t * 2 * 3.14159f ) );
		}
		FixedVertex fixedConeVerts[NUM_SEGMENTS*3];
		auto dx = getDxRenderer();
		FixedVertex vert(Vec3f(), dx->mCurrentNormal, dx->mCurrentUV, dx->mCurrentColor);
		int coneIndex = 1;
		for(int i = 0; i < NUM_SEGMENTS; ++i)
		{
			vert.pos = coneVerts[0];
			fixedConeVerts[i*3+0] = vert;
			vert.pos = coneVerts[coneIndex++];
			fixedConeVerts[i*3+1] = vert;
			vert.pos = coneVerts[coneIndex];
			fixedConeVerts[i*3+2] = vert;
		}
		applyDxFixedPipeline(fixedConeVerts, NUM_SEGMENTS * 3, COLOR_VERTEX, COLOR_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		dx->mDeviceContext->Draw(NUM_SEGMENTS * 3, 0);

		// draw the cap
		coneVerts[0] = end;
		for( int s = 0; s <= NUM_SEGMENTS; ++s ) {
			float t = s / (float)NUM_SEGMENTS;
			coneVerts[s+1] = Vec3f( end - left * headRadius * math<float>::cos( t * 2 * 3.14159f )
				+ up * headRadius * math<float>::sin( t * 2 * 3.14159f ) );
		}
		coneIndex = 1;
		for(int i = 0; i < NUM_SEGMENTS; ++i)
		{
			vert.pos = coneVerts[0];
			fixedConeVerts[i*3+0] = vert;
			vert.pos = coneVerts[coneIndex++];
			fixedConeVerts[i*3+1] = vert;
			vert.pos = coneVerts[coneIndex];
			fixedConeVerts[i*3+2] = vert;
		}
		applyDxFixedPipeline(fixedConeVerts, NUM_SEGMENTS * 3, COLOR_VERTEX, COLOR_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		dx->mDeviceContext->Draw(NUM_SEGMENTS * 3, 0);
	}
//#endif
}

void drawFrustum( const Camera &cam )
{
	Vec3f vertex[8];
	Vec3f nearTopLeft, nearTopRight, nearBottomLeft, nearBottomRight;
	cam.getNearClipCoordinates( &nearTopLeft, &nearTopRight, &nearBottomLeft, &nearBottomRight );

	Vec3f farTopLeft, farTopRight, farBottomLeft, farBottomRight;
	cam.getFarClipCoordinates( &farTopLeft, &farTopRight, &farBottomLeft, &farBottomRight );

	// extract camera position from modelview matrix, so that it will work with CameraStereo as well	
	//  see: http://www.gamedev.net/topic/397751-how-to-get-camera-position/page__p__3638207#entry3638207
	Matrix44f modelview = cam.getModelViewMatrix();	
	Vec3f eye;
	eye.x = -(modelview.at(0,0) * modelview.at(0,3) + modelview.at(1,0) * modelview.at(1,3) + modelview.at(2,0) * modelview.at(2,3));
	eye.y = -(modelview.at(0,1) * modelview.at(0,3) + modelview.at(1,1) * modelview.at(1,3) + modelview.at(2,1) * modelview.at(2,3));
	eye.z = -(modelview.at(0,2) * modelview.at(0,3) + modelview.at(1,2) * modelview.at(1,3) + modelview.at(2,2) * modelview.at(2,3));
		
//	if(usingGL())
//	{
//		glEnableClientState( GL_VERTEX_ARRAY );
//		glVertexPointer( 3, GL_FLOAT, 0, &vertex[0].x );
//	
//#if ! defined( CINDER_GLES )
//		glEnable( GL_LINE_STIPPLE );
//		glLineStipple( 3, 0xAAAA );
//#endif
//	}
	vertex[0] = eye;
	vertex[1] = nearTopLeft;
	vertex[2] = eye;
	vertex[3] = nearTopRight;
	vertex[4] = eye;
	vertex[5] = nearBottomRight;
	vertex[6] = eye;
	vertex[7] = nearBottomLeft;
//	if(usingGL())
//		glDrawArrays( GL_LINES, 0, 8 );
//#if defined( CINDER_MSW )
//	else
	{
		for(int i = 0; i < 8; i += 2)
			drawLine(vertex[i], vertex[i+1]);
	}
//#endif
//#if ! defined( CINDER_GLES )
//	glDisable( GL_LINE_STIPPLE );
//#endif
	vertex[0] = farTopLeft;
	vertex[1] = nearTopLeft;
	vertex[2] = farTopRight;
	vertex[3] = nearTopRight;
	vertex[4] = farBottomRight;
	vertex[5] = nearBottomRight;
	vertex[6] = farBottomLeft;
	vertex[7] = nearBottomLeft;
//	if(usingGL())
//	{
//		glDrawArrays( GL_LINES, 0, 8 );
//		glLineWidth( 2.0f );
//	}
//#if defined( CINDER_MSW )
//	else
	{
		for(int i = 0; i < 8; i += 2)
			drawLine(vertex[i], vertex[i+1]);
	}
//#endif
	vertex[0] = nearTopLeft;
	vertex[1] = nearTopRight;
	vertex[2] = nearBottomRight;
	vertex[3] = nearBottomLeft;
//	if(usingGL())
//		glDrawArrays( GL_LINE_LOOP, 0, 4 );
//#if defined( CINDER_MSW )
//	else
	{
		drawLine(vertex[0], vertex[3]);
		for(int i = 0; i < 3; ++i)
			drawLine(vertex[i], vertex[i+1]);
	}
//#endif
	vertex[0] = farTopLeft;
	vertex[1] = farTopRight;
	vertex[2] = farBottomRight;
	vertex[3] = farBottomLeft;
//	if(usingGL())
//	{
//		glDrawArrays( GL_LINE_LOOP, 0, 4 );
//	
//		glLineWidth( 1.0f );
//		glDisableClientState( GL_VERTEX_ARRAY );
//	}
//#if defined( CINDER_MSW )
//	else
	{
		drawLine(vertex[0], vertex[3]);
		for(int i = 0; i < 3; ++i)
			drawLine(vertex[i], vertex[i+1]);
	}
//#endif
}

void drawTorus( float outterRadius, float innerRadius, int longitudeSegments, int latitudeSegments )
{
	auto dx = getDxRenderer();
	longitudeSegments = std::min( std::max( 7, longitudeSegments ) + 1, 255 );
	latitudeSegments = std::min( std::max( 7, latitudeSegments ) + 1, 255 );

	int i, j;
	float *normal = new float[longitudeSegments * latitudeSegments * 3];
	float *vertex = new float[longitudeSegments * latitudeSegments * 3];
	float *tex = new float[longitudeSegments * latitudeSegments * 2];
	GLushort *indices = new GLushort[latitudeSegments * 2];
	float ct, st, cp, sp;
	FixedVertex *verts = new FixedVertex[longitudeSegments * latitudeSegments];

	//if(usingGL())
	//{
	//	glEnableClientState( GL_VERTEX_ARRAY );
	//	glVertexPointer( 3, GL_FLOAT, 0, vertex );
	//	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	//	glTexCoordPointer( 2, GL_FLOAT, 0, tex );
	//	glEnableClientState( GL_NORMAL_ARRAY );
	//	glNormalPointer( GL_FLOAT, 0, normal );
	//}

	for( i = 0; i < longitudeSegments; i++ ) {
		ct = cos(2.0f * (float)M_PI * (float)i / (float)(longitudeSegments - 1));
		st = sin(2.0f * (float)M_PI * (float)i / (float)(longitudeSegments - 1));

		for ( j = 0; j < latitudeSegments; j++ ) {
			cp = cos(2.0f * (float)M_PI * (float)j / (float)(latitudeSegments - 1));
			sp = sin(2.0f * (float)M_PI * (float)j / (float)(latitudeSegments - 1));

			normal[3 * (i + longitudeSegments * j)    ] = cp * ct;
			normal[3 * (i + longitudeSegments * j) + 1] = sp * ct;
			normal[3 * (i + longitudeSegments * j) + 2] = st;

			tex[2 * (i + longitudeSegments * j)    ] = 1.0f * (float)i / (float)(longitudeSegments - 1);
			tex[2 * (i + longitudeSegments * j) + 1] = 5.0f * (float)j / (float)(latitudeSegments - 1);

			vertex[3 * (i + longitudeSegments * j)    ] = cp * (outterRadius  + innerRadius * ct);
			vertex[3 * (i + longitudeSegments * j) + 1] = sp * (outterRadius  + innerRadius * ct);
			vertex[3 * (i + longitudeSegments * j) + 2] = innerRadius * st;
			verts[i + longitudeSegments * j].pos = *(Vec3f*)&vertex[3 * (i + longitudeSegments * j)];
			verts[i + longitudeSegments * j].norm = *(Vec3f*)&normal[3 * (i + longitudeSegments * j)];
			verts[i + longitudeSegments * j].uv = *(Vec2f*)&tex[2 * (i + longitudeSegments * j)];
			verts[i + longitudeSegments * j].color = dx->mCurrentColor;
		}
	}
//#if defined( CINDER_MSW )
//		if(!usingGL())
			applyDxFixedPipeline(verts, longitudeSegments * latitudeSegments, COLOR_VERTEX, COLOR_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
//#endif
	for ( i = 0; i < longitudeSegments - 1; i++ ) {
		for ( j = 0; j < latitudeSegments; j++ ) {
			indices[j*2+0] = i + 1 + longitudeSegments * j;
			indices[j*2+1] = i + longitudeSegments * j;
		}
//		if(usingGL())
//			glDrawElements( GL_TRIANGLE_STRIP, (latitudeSegments)*2, GL_UNSIGNED_SHORT, indices );
//#if defined( CINDER_MSW )
//		else
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			dx->mDeviceContext->Map(dx->mIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			memcpy(mappedResource.pData, indices, sizeof(GLushort) * latitudeSegments * 2);
			dx->mDeviceContext->Unmap(dx->mIndexBuffer, 0);
			//D3D11_BOX box = { 0, 0, 0, sizeof( GLushort ) * latitudeSegments * 2, 1, 1 };
			//dx->mDeviceContext->UpdateSubresource(dx->mIndexBuffer, 0, &box, indices, 0, 0);

			dx->mDeviceContext->IASetIndexBuffer(dx->mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
			dx->mDeviceContext->DrawIndexed(latitudeSegments*2, 0, 0);
		}
//#endif
	}

	//if(usingGL())
	//{
	//	glDisableClientState( GL_VERTEX_ARRAY );
	//	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	//	glDisableClientState( GL_NORMAL_ARRAY );
	//}

	delete [] normal;
	delete [] tex;
	delete [] vertex;
	delete [] indices;
	delete [] verts;
}

void drawCylinder( float base, float top, float height, int slices, int stacks )
{
	auto dx = getDxRenderer();
	stacks = math<int>::max(2, stacks + 1);	// minimum of 1 stack
	slices = math<int>::max(4, slices + 1);	// minimum of 3 slices

	int i, j;
	float *normal = new float[stacks * slices * 3];
	float *vertex = new float[stacks * slices * 3];
	float *tex = new float[stacks * slices * 2];
	GLushort *indices = new GLushort[slices * 2];
	FixedVertex *verts = new FixedVertex[stacks * slices];

	//if(usingGL())
	//{
	//	glEnableClientState( GL_VERTEX_ARRAY );
	//	glVertexPointer( 3, GL_FLOAT, 0, vertex );
	//	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	//	glTexCoordPointer( 2, GL_FLOAT, 0, tex );
	//	glEnableClientState( GL_NORMAL_ARRAY );
	//	glNormalPointer( GL_FLOAT, 0, normal );
	//}

	for(i=0;i<slices;i++) {
		float u = (float)i / (float)(slices - 1);
		float ct = cos(2.0f * (float)M_PI * u);
		float st = sin(2.0f * (float)M_PI * u);

		for(j=0;j<stacks;j++) {
			float v = (float)j / (float)(stacks-1);
			float radius = lerp<float>(base, top, v); 

			int index = 3 * (i * stacks + j);

			normal[index    ] = ct;
			normal[index + 1] = 0;
			normal[index + 2] = st;

			tex[2 * (i * stacks + j)    ] = u;
			tex[2 * (i * stacks + j) + 1] = 1.0f - v; // top of texture is top of cylinder

			vertex[index    ] = ct * radius;
			vertex[index + 1] = v * height;
			vertex[index + 2] = st * radius;
			verts[i * stacks + j].pos = *(Vec3f*)&vertex[index];
			verts[i * stacks + j].norm = *(Vec3f*)&normal[index];
			verts[i * stacks + j].uv = *(Vec2f*)&tex[2 * (i * stacks + j)];
			verts[i * stacks + j].color = dx->mCurrentColor;
		}
	}

//#if defined( CINDER_MSW )
//	if(!usingGL())
		applyDxFixedPipeline(verts, stacks * slices, COLOR_VERTEX, COLOR_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
//#endif

	for(i=0;i<(stacks - 1);i++) {
		for(j=0;j<slices;j++) {
			indices[j*2+0] = i + 0 + j * stacks;
			indices[j*2+1] = i + 1 + j * stacks;
		}
//		if(usingGL())
//			glDrawElements( GL_TRIANGLE_STRIP, (slices)*2, GL_UNSIGNED_SHORT, indices );
//#if defined( CINDER_MSW )
//		else
		{
			D3D11_MAPPED_SUBRESOURCE subresource;
			dx->mDeviceContext->Map(dx->mIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
			memcpy(subresource.pData, indices, sizeof(GLushort) * slices * 2);
			dx->mDeviceContext->Unmap(dx->mIndexBuffer, 0);
			//D3D11_BOX box = { 0, 0, 0, sizeof( GLushort ) * slices * 2, 1, 1 };
			//dx->mDeviceContext->UpdateSubresource(dx->mIndexBuffer, 0, &box, indices, 0, 0);
			dx->mDeviceContext->IASetIndexBuffer(dx->mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
			dx->mDeviceContext->DrawIndexed(slices*2, 0, 0);
		}
//#endif
	}

	//if(usingGL())
	//{
	//	glDisableClientState( GL_NORMAL_ARRAY );
	//	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	//	glDisableClientState( GL_VERTEX_ARRAY );
	//}

	delete [] normal;
	delete [] tex;
	delete [] vertex;
	delete [] indices;
	delete [] verts;
}

void draw( const PolyLine<Vec2f> &polyLine )
{
//	if(usingGL())
//	{
//		glEnableClientState( GL_VERTEX_ARRAY );
//		glVertexPointer( 2, GL_FLOAT, 0, &(polyLine.getPoints()[0]) );
//		glDrawArrays( ( polyLine.isClosed() ) ? GL_LINE_LOOP : GL_LINE_STRIP, 0, polyLine.size() );
//		glDisableClientState( GL_VERTEX_ARRAY );
//	}
//#if defined( CINDER_MSW )
//	else
	{
		FixedVertex *verts = new FixedVertex[polyLine.getPoints().size() + 1];
		auto dx = getDxRenderer();

		for(unsigned i = 0; i < polyLine.getPoints().size(); ++i)
			verts[i] = FixedVertex(Vec3f(polyLine.getPoints()[i], 0), dx->mCurrentNormal, dx->mCurrentUV, dx->mCurrentColor);
		verts[polyLine.getPoints().size()] = verts[0];
		applyDxFixedPipeline(verts, polyLine.getPoints().size() + 1, COLOR_VERTEX, COLOR_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		dx->mDeviceContext->Draw((polyLine.isClosed()) ? polyLine.getPoints().size() + 1 : polyLine.getPoints().size(), 0);
		delete [] verts;
	}
//#endif
}

void draw( const PolyLine<Vec3f> &polyLine )
{
//	if(usingGL())
//	{
//		glEnableClientState( GL_VERTEX_ARRAY );
//		glVertexPointer( 3, GL_FLOAT, 0, &(polyLine.getPoints()[0]) );
//		glDrawArrays( ( polyLine.isClosed() ) ? GL_LINE_LOOP : GL_LINE_STRIP, 0, polyLine.size() );
//		glDisableClientState( GL_VERTEX_ARRAY );
//	}
//#if defined( CINDER_MSW )
//	else
	{
		auto dx = getDxRenderer();
		FixedVertex *verts = new FixedVertex[polyLine.getPoints().size() + 1];
		for(unsigned i = 0; i < polyLine.getPoints().size(); ++i)
			verts[i] = FixedVertex(polyLine.getPoints()[i], dx->mCurrentNormal, dx->mCurrentUV, dx->mCurrentColor);
		verts[polyLine.getPoints().size()] = verts[0];
		applyDxFixedPipeline(verts, polyLine.getPoints().size() + 1, COLOR_VERTEX, COLOR_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		dx->mDeviceContext->Draw((polyLine.isClosed()) ? polyLine.getPoints().size() + 1 : polyLine.getPoints().size(), 0);
		delete [] verts;
	}
//#endif
}

void draw( const Path2d &path2d, float approximationScale )
{
	if( path2d.getNumSegments() == 0 )
		return;
	std::vector<Vec2f> points = path2d.subdivide( approximationScale );
//	if(usingGL())
//	{
//		glEnableClientState( GL_VERTEX_ARRAY );
//		glVertexPointer( 2, GL_FLOAT, 0, &(points[0]) );
//		glDrawArrays( GL_LINE_STRIP, 0, points.size() );
//		glDisableClientState( GL_VERTEX_ARRAY );	
//	}
//#if defined( CINDER_MSW )
//	else
	{
		unsigned pointCount = points.size();
		FixedVertex *verts = new FixedVertex[pointCount];
		auto dx = getDxRenderer();
		for(unsigned i = 0; i < pointCount; ++i)
			verts[i] = FixedVertex(Vec3f(points[i], 0), dx->mCurrentNormal, dx->mCurrentUV, dx->mCurrentColor);
		applyDxFixedPipeline(verts, points.size(), COLOR_VERTEX, COLOR_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		dx->mDeviceContext->Draw(points.size(), 0);
		delete [] verts;
	}
//#endif
}

void draw( const Shape2d &shape2d, float approximationScale )
{
//	if(usingGL())
//	{
//		glEnableClientState( GL_VERTEX_ARRAY );
//		for( std::vector<Path2d>::const_iterator contourIt = shape2d.getContours().begin(); contourIt != shape2d.getContours().end(); ++contourIt ) {
//			if( contourIt->getNumSegments() == 0 )
//				continue;
//			std::vector<Vec2f> points = contourIt->subdivide( approximationScale );
//			glVertexPointer( 2, GL_FLOAT, 0, &(points[0]) );
//			glDrawArrays( GL_LINE_STRIP, 0, points.size() );
//		}
//		glDisableClientState( GL_VERTEX_ARRAY );
//	}
//#if defined( CINDER_MSW )
//	else
	{
		auto dx = getDxRenderer();
		for( std::vector<Path2d>::const_iterator contourIt = shape2d.getContours().begin(); contourIt != shape2d.getContours().end(); ++contourIt ) {
			if( contourIt->getNumSegments() == 0 )
				continue;
			std::vector<Vec2f> points = contourIt->subdivide( approximationScale );
			FixedVertex *verts = new FixedVertex[points.size()];
			for(unsigned i = 0; i < points.size(); ++i)
				verts[i] = FixedVertex(Vec3f(points[i], 0), dx->mCurrentNormal, dx->mCurrentUV, dx->mCurrentColor);
			applyDxFixedPipeline(verts, points.size(), COLOR_VERTEX, COLOR_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
			dx->mDeviceContext->Draw(points.size(), 0);
			delete [] verts;
		}
	}
//#endif
}


void drawSolid( const Path2d &path2d, float approximationScale )
{
	draw( Triangulator( path2d ).calcMesh() );
}

void drawSolid( const Shape2d &shape2d, float approximationScale )
{
	draw( Triangulator( shape2d ).calcMesh() );
}

void drawSolid( const PolyLine2f &polyLine )
{
	draw( Triangulator( polyLine ).calcMesh() );
}

// TriMesh2d
void draw( const TriMesh2d &mesh )
{
	if( mesh.getNumVertices() <= 0 )
		return;

//	if(usingGL())
//	{
//		glVertexPointer( 2, GL_FLOAT, 0, &(mesh.getVertices()[0]) );
//		glEnableClientState( GL_VERTEX_ARRAY );
//
//		glDisableClientState( GL_NORMAL_ARRAY );
//	
//		if( mesh.hasColorsRgb() ) {
//			glColorPointer( 3, GL_FLOAT, 0, &(mesh.getColorsRGB()[0]) );
//			glEnableClientState( GL_COLOR_ARRAY );
//		}
//		else if( mesh.hasColorsRgba() ) {
//			glColorPointer( 4, GL_FLOAT, 0, &(mesh.getColorsRGBA()[0]) );
//			glEnableClientState( GL_COLOR_ARRAY );
//		}
//		else 
//			glDisableClientState( GL_COLOR_ARRAY );	
//
//		if( mesh.hasTexCoords() ) {
//			glTexCoordPointer( 2, GL_FLOAT, 0, &(mesh.getTexCoords()[0]) );
//			glEnableClientState( GL_TEXTURE_COORD_ARRAY );
//		}
//		else
//			glDisableClientState( GL_TEXTURE_COORD_ARRAY );
//#if defined ( CINDER_GLES )
//		GLushort * indices = new GLushort[ mesh.getIndices().size() ];
//		for ( size_t i = 0; i < mesh.getIndices().size(); i++ ) {
//			indices[ i ] = static_cast<GLushort>( mesh.getIndices()[ i ] );
//		}
//		glDrawElements( GL_TRIANGLES, mesh.getIndices().size(), GL_UNSIGNED_SHORT, (const GLvoid*)indices );
//		delete [] indices;
//#else
//		glDrawElements( GL_TRIANGLES, mesh.getNumIndices(), GL_UNSIGNED_INT, &(mesh.getIndices()[0]) );
//#endif
//
//		glDisableClientState( GL_VERTEX_ARRAY );
//		glDisableClientState( GL_NORMAL_ARRAY );
//		glDisableClientState( GL_COLOR_ARRAY );
//		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
//	}
//#if defined( CINDER_MSW )
//	else
	{
		unsigned vertCount = mesh.getVertices().size();
		auto dx = getDxRenderer();

		FixedVertex *verts = new FixedVertex[vertCount];
		for(unsigned i = 0; i < vertCount; ++i)
		{
			verts[i].pos = Vec3f(mesh.getVertices()[i], 0);
			verts[i].norm = dx->mCurrentNormal;
			verts[i].uv = (mesh.hasTexCoords()) ? mesh.getTexCoords()[i] : dx->mCurrentUV;
			if(mesh.hasColorsRgb())
				verts[i].color = Vec4f(*(const Vec3f*)&mesh.getColorsRGB()[i], 1);
			else if(mesh.hasColorsRgba())
				verts[i].color = *(const Vec4f*)&mesh.getColorsRGBA()[i];
			else
				verts[i].color = dx->mCurrentColor;
		}
		applyDxFixedPipeline(verts, vertCount, COLOR_VERTEX, COLOR_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		D3D11_MAPPED_SUBRESOURCE subresource;
		dx->mDeviceContext->Map(dx->mIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
		memcpy(subresource.pData, &mesh.getIndices()[0], sizeof(GLuint) * mesh.getIndices().size());
		dx->mDeviceContext->Unmap(dx->mIndexBuffer, 0);
		//D3D11_BOX box = { 0, 0, 0, sizeof(GLuint) * mesh.getIndices().size(), 1, 1 };
		//dx->mDeviceContext->UpdateSubresource(dx->mIndexBuffer, 0, &box, &mesh.getIndices()[0], 0, 0);
		dx->mDeviceContext->IASetIndexBuffer(dx->mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		dx->mDeviceContext->DrawIndexed(mesh.getIndices().size(), 0, 0);
		delete [] verts;
	}
//#endif
}

// TriMesh2d
void drawRange( const TriMesh2d &mesh, size_t startTriangle, size_t triangleCount )
{
//	if(usingGL())
//	{
//		glVertexPointer( 2, GL_FLOAT, 0, &(mesh.getVertices()[0]) );
//		glEnableClientState( GL_VERTEX_ARRAY );
//
//		glDisableClientState( GL_NORMAL_ARRAY );
//
//		if( mesh.hasColorsRgb() ) {
//			glColorPointer( 3, GL_FLOAT, 0, &(mesh.getColorsRGB()[0]) );
//			glEnableClientState( GL_COLOR_ARRAY );
//		}
//		else if( mesh.hasColorsRgba() ) {
//			glColorPointer( 4, GL_FLOAT, 0, &(mesh.getColorsRGBA()[0]) );
//			glEnableClientState( GL_COLOR_ARRAY );
//		}	
//		else 
//			glDisableClientState( GL_COLOR_ARRAY );
//	
//		if( mesh.hasTexCoords() ) {
//			glTexCoordPointer( 2, GL_FLOAT, 0, &(mesh.getTexCoords()[0]) );
//			glEnableClientState( GL_TEXTURE_COORD_ARRAY );
//		}
//		else
//			glDisableClientState( GL_TEXTURE_COORD_ARRAY );
//
//#if defined ( CINDER_GLES )
//		size_t max = math<size_t>::min( mesh.getNumIndices(), 0xFFFF );
//		size_t start = math<size_t>::min( startTriangle * 3, max );
//		size_t count = math<size_t>::min( max - start, triangleCount * 3 );
//		GLushort * indices = new GLushort[ max ];
//		for ( size_t i = 0; i < max; i++ ) {
//			indices[ i ] = static_cast<GLushort>( mesh.getIndices()[ i ] );
//		}
//		glDrawElements( GL_TRIANGLES, count, GL_UNSIGNED_SHORT, (const GLvoid*)( indices + start ) );
//		delete [] indices;
//#else
//		glDrawRangeElements( GL_TRIANGLES, 0, mesh.getNumVertices(), triangleCount * 3, GL_UNSIGNED_INT, &(mesh.getIndices()[startTriangle*3]) );
//#endif
//	
//		glDisableClientState( GL_VERTEX_ARRAY );
//		glDisableClientState( GL_NORMAL_ARRAY );
//		glDisableClientState( GL_COLOR_ARRAY );
//		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
//	}
//#if defined( CINDER_MSW )
//	else
	{
		unsigned vertCount = mesh.getVertices().size();
		auto dx = getDxRenderer();
		FixedVertex *verts = new FixedVertex[vertCount];
		for(unsigned i = 0; i < vertCount; ++i)
		{
			verts[i].pos = Vec3f(mesh.getVertices()[i], 0);
			verts[i].norm = dx->mCurrentNormal;
			verts[i].uv = (mesh.hasTexCoords()) ? mesh.getTexCoords()[i] : dx->mCurrentUV;
			if(mesh.hasColorsRgb())
				verts[i].color = Vec4f(*(const Vec3f*)&mesh.getColorsRGB()[i], 1);
			else if(mesh.hasColorsRgba())
				verts[i].color = *(const Vec4f*)&mesh.getColorsRGBA()[i];
			else
				verts[i].color = dx->mCurrentColor;
		}
		applyDxFixedPipeline(verts, vertCount, COLOR_VERTEX, COLOR_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		D3D11_MAPPED_SUBRESOURCE subresource;
		dx->mDeviceContext->Map(dx->mIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
		memcpy(subresource.pData, &mesh.getIndices()[0], sizeof(GLuint) * mesh.getIndices().size());
		dx->mDeviceContext->Unmap(dx->mIndexBuffer, 0);
		//D3D11_BOX box = { 0, 0, 0, sizeof(GLuint) * mesh.getIndices().size(), 1, 1 };
		//dx->mDeviceContext->UpdateSubresource(dx->mIndexBuffer, 0, &box, &mesh.getIndices()[0], 0, 0);
		dx->mDeviceContext->IASetIndexBuffer(dx->mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		dx->mDeviceContext->DrawIndexed(triangleCount * 3, startTriangle, 0);
		delete [] verts;
	}
//#endif
}

// TriMesh
void draw( const TriMesh &mesh )
{
//	if(usingGL())
//	{
//		glVertexPointer( 3, GL_FLOAT, 0, &(mesh.getVertices()[0]) );
//		glEnableClientState( GL_VERTEX_ARRAY );
//
//		if( mesh.hasNormals() ) {
//			glNormalPointer( GL_FLOAT, 0, &(mesh.getNormals()[0]) );
//			glEnableClientState( GL_NORMAL_ARRAY );
//		}
//		else
//			glDisableClientState( GL_NORMAL_ARRAY );
//	
//		if( mesh.hasColorsRGB() ) {
//			glColorPointer( 3, GL_FLOAT, 0, &(mesh.getColorsRGB()[0]) );
//			glEnableClientState( GL_COLOR_ARRAY );
//		}
//		else if( mesh.hasColorsRGBA() ) {
//			glColorPointer( 4, GL_FLOAT, 0, &(mesh.getColorsRGBA()[0]) );
//			glEnableClientState( GL_COLOR_ARRAY );
//		}
//		else 
//			glDisableClientState( GL_COLOR_ARRAY );	
//
//		if( mesh.hasTexCoords() ) {
//			glTexCoordPointer( 2, GL_FLOAT, 0, &(mesh.getTexCoords()[0]) );
//			glEnableClientState( GL_TEXTURE_COORD_ARRAY );
//		}
//		else
//			glDisableClientState( GL_TEXTURE_COORD_ARRAY );
//#if defined ( CINDER_GLES )
//		GLushort * indices = new GLushort[ mesh.getIndices().size() ];
//		for ( size_t i = 0; i < mesh.getIndices().size(); i++ ) {
//			indices[ i ] = static_cast<GLushort>( mesh.getIndices()[ i ] );
//		}
//		glDrawElements( GL_TRIANGLES, mesh.getIndices().size(), GL_UNSIGNED_SHORT, (const GLvoid*)indices );
//		delete [] indices;
//#else
//		glDrawElements( GL_TRIANGLES, mesh.getNumIndices(), GL_UNSIGNED_INT, &(mesh.getIndices()[0]) );
//#endif
//
//		glDisableClientState( GL_VERTEX_ARRAY );
//		glDisableClientState( GL_NORMAL_ARRAY );
//		glDisableClientState( GL_COLOR_ARRAY );
//		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
//	}
//#if defined( CINDER_MSW )
//	else
	{
		auto dx = getDxRenderer();
		unsigned vertCount = mesh.getVertices().size();
		FixedVertex *verts = new FixedVertex[vertCount];
		for(unsigned i = 0; i < vertCount; ++i)
		{
			verts[i].pos = mesh.getVertices()[i];
			verts[i].norm = dx->mCurrentNormal;
			verts[i].uv = (mesh.hasTexCoords()) ? mesh.getTexCoords()[i] : dx->mCurrentUV;
			if(mesh.hasColorsRGB())
				verts[i].color = Vec4f(*(const Vec3f*)&mesh.getColorsRGB()[i], 1);
			else if(mesh.hasColorsRGBA())
				verts[i].color = *(const Vec4f*)&mesh.getColorsRGBA()[i];
			else
				verts[i].color = dx->mCurrentColor;
		}
		applyDxFixedPipeline(verts, vertCount, COLOR_VERTEX, COLOR_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		D3D11_MAPPED_SUBRESOURCE subresource;
		dx->mDeviceContext->Map(dx->mIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
		memcpy(subresource.pData, &mesh.getIndices()[0], sizeof(GLuint) * mesh.getIndices().size());
		dx->mDeviceContext->Unmap(dx->mIndexBuffer, 0);
		//D3D11_BOX box = { 0, 0, 0, sizeof(GLuint) * mesh.getIndices().size(), 1, 1 };
		//dx->mDeviceContext->UpdateSubresource(dx->mIndexBuffer, 0, &box, &mesh.getIndices()[0], 0, 0);
		dx->mDeviceContext->IASetIndexBuffer(dx->mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		dx->mDeviceContext->DrawIndexed(mesh.getIndices().size(), 0, 0);
		delete [] verts;
	}
//#endif
}

// TriMesh2d
void drawRange( const TriMesh &mesh, size_t startTriangle, size_t triangleCount )
{
//	if(usingGL())
//	{
//		glVertexPointer( 3, GL_FLOAT, 0, &(mesh.getVertices()[0]) );
//		glEnableClientState( GL_VERTEX_ARRAY );
//
//		if( mesh.hasNormals() ) {
//			glNormalPointer( GL_FLOAT, 0, &(mesh.getNormals()[0]) );
//			glEnableClientState( GL_NORMAL_ARRAY );
//		}
//		else
//			glDisableClientState( GL_NORMAL_ARRAY );
//
//		if( mesh.hasColorsRGB() ) {
//			glColorPointer( 3, GL_FLOAT, 0, &(mesh.getColorsRGB()[0]) );
//			glEnableClientState( GL_COLOR_ARRAY );
//		}
//		else if( mesh.hasColorsRGBA() ) {
//			glColorPointer( 4, GL_FLOAT, 0, &(mesh.getColorsRGBA()[0]) );
//			glEnableClientState( GL_COLOR_ARRAY );
//		}	
//		else 
//			glDisableClientState( GL_COLOR_ARRAY );
//	
//		if( mesh.hasTexCoords() ) {
//			glTexCoordPointer( 2, GL_FLOAT, 0, &(mesh.getTexCoords()[0]) );
//			glEnableClientState( GL_TEXTURE_COORD_ARRAY );
//		}
//		else
//			glDisableClientState( GL_TEXTURE_COORD_ARRAY );
//		
//#if defined ( CINDER_GLES )
//		size_t max = math<size_t>::min( mesh.getNumIndices(), 0xFFFF );
//		size_t start = math<size_t>::min( startTriangle * 3, max );
//		size_t count = math<size_t>::min( max - start, triangleCount * 3 );
//		GLushort * indices = new GLushort[ max ];
//		for ( size_t i = 0; i < max; i++ ) {
//			indices[ i ] = static_cast<GLushort>( mesh.getIndices()[ i ] );
//		}
//		glDrawElements( GL_TRIANGLES, count, GL_UNSIGNED_SHORT, (const GLvoid*)( indices + start ) );
//		delete [] indices;
//#else
//		glDrawRangeElements( GL_TRIANGLES, 0, mesh.getNumVertices(), triangleCount * 3, GL_UNSIGNED_INT, &(mesh.getIndices()[startTriangle*3]) );
//#endif
//
//		glDisableClientState( GL_VERTEX_ARRAY );
//		glDisableClientState( GL_NORMAL_ARRAY );
//		glDisableClientState( GL_COLOR_ARRAY );
//		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
//	}
//#if defined( CINDER_MSW )
//	else
	{
		auto dx = getDxRenderer();
		unsigned vertCount = mesh.getVertices().size();
		FixedVertex *verts = new FixedVertex[vertCount];
		for(unsigned i = 0; i < vertCount; ++i)
		{
			verts[i].pos = mesh.getVertices()[i];
			verts[i].norm = dx->mCurrentNormal;
			verts[i].uv = (mesh.hasTexCoords()) ? mesh.getTexCoords()[i] : dx->mCurrentUV;
			if(mesh.hasColorsRGB())
				verts[i].color = Vec4f(*(const Vec3f*)&mesh.getColorsRGB()[i], 1);
			else if(mesh.hasColorsRGBA())
				verts[i].color = *(const Vec4f*)&mesh.getColorsRGBA()[i];
			else
				verts[i].color = dx->mCurrentColor;
		}
		applyDxFixedPipeline(verts, vertCount, COLOR_VERTEX, COLOR_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		D3D11_MAPPED_SUBRESOURCE subresource;
		dx->mDeviceContext->Map(dx->mIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
		memcpy(subresource.pData, &mesh.getIndices()[0], sizeof(GLuint) * mesh.getIndices().size());
		dx->mDeviceContext->Unmap(dx->mIndexBuffer, 0);
		//D3D11_BOX box = { 0, 0, 0, sizeof(GLuint) * mesh.getIndices().size(), 1, 1 };
		//dx->mDeviceContext->UpdateSubresource(dx->mIndexBuffer, 0, &box, &mesh.getIndices()[0], 0, 0);
		dx->mDeviceContext->IASetIndexBuffer(dx->mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		dx->mDeviceContext->DrawIndexed(triangleCount * 3, startTriangle, 0);
		delete [] verts;
	}
//#endif
}

void draw( const VboMesh &vbo )
{
	if( vbo.getNumIndices() > 0 )
		drawRange( vbo, (size_t)0, vbo.getNumIndices() );
	else
		drawArrays( vbo, 0, vbo.getNumVertices() );
}

void drawRange( const VboMesh &vbo, size_t startIndex, size_t indexCount, int vertexStart, int vertexEnd )
{
	if( vbo.getNumIndices() <= 0 )
		return;

	if( vertexStart < 0 ) vertexStart = 0;
	if( vertexEnd < 0 ) vertexEnd = vbo.getNumVertices();

	auto dx = getDxRenderer();
	if(!dx->getRenderFlag(app::AppImplMswRendererDx::CUSTOM_SHADER_ACTIVE))
	{
		ID3D11ShaderResourceView *view;
		dx->mDeviceContext->PSGetShaderResources(0, 1, &view);
		bool hasNormals = vbo.getLayout().hasNormals();
		bool hasColors = vbo.getLayout().hasColorsRGB() || vbo.getLayout().hasColorsRGBA();
		bool hasUVs = vbo.getLayout().hasTexCoords(0);
		bool lightsEnabled = dx->mLightingEnabled;
		if(hasNormals && hasColors && hasUVs)
			dx->mDeviceContext->VSSetShader((lightsEnabled) ? dx->mVboPositionNormalColorTextureLightVS : dx->mVboPositionNormalColorTextureVS, NULL, 0);
		else if(hasNormals && hasColors)
			dx->mDeviceContext->VSSetShader((lightsEnabled) ? dx->mVboPositionNormalColorLightVS : dx->mVboPositionNormalColorVS, NULL, 0);
		else if(hasNormals && hasUVs)
			dx->mDeviceContext->VSSetShader((lightsEnabled) ? dx->mVboPositionNormalTextureLightVS : dx->mVboPositionNormalTextureVS, NULL, 0);
		else if(hasNormals)
			dx->mDeviceContext->VSSetShader((lightsEnabled) ? dx->mVboPositionNormalLightVS : dx->mVboPositionNormalVS, NULL, 0);
		else if(hasColors && hasUVs)
			dx->mDeviceContext->VSSetShader((lightsEnabled) ? dx->mVboPositionColorTextureLightVS : dx->mVboPositionColorTextureVS, NULL, 0);
		else if(hasColors)
			dx->mDeviceContext->VSSetShader((lightsEnabled) ? dx->mVboPositionColorLightVS : dx->mVboPositionColorVS, NULL, 0);
		else if(hasUVs)
			dx->mDeviceContext->VSSetShader((lightsEnabled) ? dx->mVboPositionTextureLightVS : dx->mVboPositionTextureVS, NULL, 0);
		else
			dx->mDeviceContext->VSSetShader((lightsEnabled) ? dx->mVboPositionLightVS : dx->mVboPositionVS, NULL, 0);
		if(view)
			dx->mDeviceContext->PSSetShader(TEXTURE_PIXEL, NULL, 0);
		else
			dx->mDeviceContext->PSSetShader(COLOR_PIXEL, NULL , 0);
	}
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	dx->mDeviceContext->Map(dx->mCBMatrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	reinterpret_cast<Matrix44f*>(mappedResource.pData)[0] = dx->mProjection.top();
	reinterpret_cast<Matrix44f*>(mappedResource.pData)[1] = dx->mModelView.top();
	dx->mDeviceContext->Unmap(dx->mCBMatrices, 0);
	dx->mDeviceContext->Map(dx->mCBFixedParameters, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	reinterpret_cast<Vec4f*>(mappedResource.pData)[0] = dx->mCurrentNormal;
	reinterpret_cast<Vec4f*>(mappedResource.pData)[1] = Vec4f(dx->mCurrentUV.x, dx->mCurrentUV.y, 0, 0);
	reinterpret_cast<Vec4f*>(mappedResource.pData)[2] = dx->mCurrentColor;
	dx->mDeviceContext->Unmap(dx->mCBFixedParameters, 0);
	dx->mDeviceContext->VSSetConstantBuffers(0, 1, &dx->mCBMatrices);
	dx->mDeviceContext->VSSetConstantBuffers(1, 1, &dx->mCBLights);
	dx->mDeviceContext->VSSetConstantBuffers(2, 1, &dx->mCBFixedParameters);
	dx->mDeviceContext->IASetPrimitiveTopology(vbo.getPrimitiveType());
	vbo.bindAllData();
	dx->mDeviceContext->DrawIndexed(indexCount, startIndex, vertexStart);

	//vbo.enableClientStates();
	//vbo.bindAllData();
	//
	//glDrawRangeElements( vbo.getPrimitiveType(), vertexStart, vertexEnd, indexCount, GL_UNSIGNED_INT, (GLvoid*)( sizeof(uint32_t) * startIndex ) );
	//
	//gl::VboMesh::unbindBuffers();
	//vbo.disableClientStates();
}

void drawArrays( const VboMesh &vbo, GLint first, GLsizei count )
{
	if( first < 0 ) first = 0;
	if( count < 0 ) count = vbo.getNumVertices();

	auto dx = getDxRenderer();
	if(!dx->getRenderFlag(app::AppImplMswRendererDx::CUSTOM_SHADER_ACTIVE))
	{
		ID3D11ShaderResourceView *view;
		dx->mDeviceContext->PSGetShaderResources(0, 1, &view);
		bool hasNormals = vbo.getLayout().hasNormals();
		bool hasColors = vbo.getLayout().hasColorsRGB() || vbo.getLayout().hasColorsRGBA();
		bool hasUVs = vbo.getLayout().hasTexCoords(0);
		bool lightsEnabled = dx->mLightingEnabled;
		if(hasNormals && hasColors && hasUVs)
			dx->mDeviceContext->VSSetShader((lightsEnabled) ? dx->mVboPositionNormalColorTextureLightVS : dx->mVboPositionNormalColorTextureVS, NULL, 0);
		else if(hasNormals && hasColors)
			dx->mDeviceContext->VSSetShader((lightsEnabled) ? dx->mVboPositionNormalColorLightVS : dx->mVboPositionNormalColorVS, NULL, 0);
		else if(hasNormals && hasUVs)
			dx->mDeviceContext->VSSetShader((lightsEnabled) ? dx->mVboPositionNormalTextureLightVS : dx->mVboPositionNormalTextureVS, NULL, 0);
		else if(hasNormals)
			dx->mDeviceContext->VSSetShader((lightsEnabled) ? dx->mVboPositionNormalLightVS : dx->mVboPositionNormalVS, NULL, 0);
		else if(hasColors && hasUVs)
			dx->mDeviceContext->VSSetShader((lightsEnabled) ? dx->mVboPositionColorTextureLightVS : dx->mVboPositionColorTextureVS, NULL, 0);
		else if(hasColors)
			dx->mDeviceContext->VSSetShader((lightsEnabled) ? dx->mVboPositionColorLightVS : dx->mVboPositionColorVS, NULL, 0);
		else if(hasUVs)
			dx->mDeviceContext->VSSetShader((lightsEnabled) ? dx->mVboPositionTextureLightVS : dx->mVboPositionTextureVS, NULL, 0);
		else
			dx->mDeviceContext->VSSetShader((lightsEnabled) ? dx->mVboPositionLightVS : dx->mVboPositionVS, NULL, 0);
		if(view)
			dx->mDeviceContext->PSSetShader(TEXTURE_PIXEL, NULL, 0);
		else
			dx->mDeviceContext->PSSetShader(COLOR_PIXEL, NULL , 0);
	}
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	dx->mDeviceContext->Map(dx->mCBMatrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	reinterpret_cast<Matrix44f*>(mappedResource.pData)[0] = dx->mProjection.top();
	reinterpret_cast<Matrix44f*>(mappedResource.pData)[1] = dx->mModelView.top();
	dx->mDeviceContext->Unmap(dx->mCBMatrices, 0);
	dx->mDeviceContext->Map(dx->mCBFixedParameters, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	reinterpret_cast<Vec4f*>(mappedResource.pData)[0] = dx->mCurrentNormal;
	reinterpret_cast<Vec4f*>(mappedResource.pData)[1] = Vec4f(dx->mCurrentUV.x, dx->mCurrentUV.y, 0, 0);
	reinterpret_cast<Vec4f*>(mappedResource.pData)[2] = dx->mCurrentColor;
	dx->mDeviceContext->Unmap(dx->mCBFixedParameters, 0);
	dx->mDeviceContext->VSSetConstantBuffers(0, 1, &dx->mCBMatrices);
	dx->mDeviceContext->VSSetConstantBuffers(1, 1, &dx->mCBLights);
	dx->mDeviceContext->VSSetConstantBuffers(2, 1, &dx->mCBFixedParameters);
	dx->mDeviceContext->IASetPrimitiveTopology(vbo.getPrimitiveType());
	vbo.bindAllData();
	dx->mDeviceContext->Draw(count, first);
	//vbo.enableClientStates();
	//vbo.bindAllData();
	//glDrawArrays( vbo.getPrimitiveType(), first, count );

	//gl::VboMesh::unbindBuffers();
	//vbo.disableClientStates();
}


void drawBillboard( const Vec3f &pos, const Vec2f &scale, float rotationDegrees, const Vec3f &bbRight, const Vec3f &bbUp )
{
	Vec3f verts[4];
	GLfloat texCoords[8] = { 0, 0, 0, 1, 1, 0, 1, 1 };
	//if(usingGL())
	//{
	//	glEnableClientState( GL_VERTEX_ARRAY );
	//	glVertexPointer( 3, GL_FLOAT, 0, &verts[0].x );
	//	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	//	glTexCoordPointer( 2, GL_FLOAT, 0, texCoords );
	//}

	float sinA = math<float>::sin( toRadians( rotationDegrees ) );
	float cosA = math<float>::cos( toRadians( rotationDegrees ) );

	verts[0] = pos + bbRight * ( -0.5f * scale.x * cosA - 0.5f * sinA * scale.y ) + bbUp * ( -0.5f * scale.x * sinA + 0.5f * cosA * scale.y );
	verts[1] = pos + bbRight * ( -0.5f * scale.x * cosA - -0.5f * sinA * scale.y ) + bbUp * ( -0.5f * scale.x * sinA + -0.5f * cosA * scale.y );
	verts[2] = pos + bbRight * ( 0.5f * scale.x * cosA - 0.5f * sinA * scale.y ) + bbUp * ( 0.5f * scale.x * sinA + 0.5f * cosA * scale.y );
	verts[3] = pos + bbRight * ( 0.5f * scale.x * cosA - -0.5f * sinA * scale.y ) + bbUp * ( 0.5f * scale.x * sinA + -0.5f * cosA * scale.y );

//	if(usingGL())
//	{
//		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
//
//		glDisableClientState( GL_VERTEX_ARRAY );
//		glDisableClientState( GL_TEXTURE_COORD_ARRAY );	
//	}
//#if defined( CINDER_MSW )
//	else
	{
		auto dx = getDxRenderer();
		FixedVertex fixedVerts[4] = {
			FixedVertex(verts[0], dx->mCurrentNormal, Vec2f(texCoords[0], texCoords[1]), dx->mCurrentColor),
			FixedVertex(verts[0], dx->mCurrentNormal, Vec2f(texCoords[2], texCoords[3]), dx->mCurrentColor),
			FixedVertex(verts[0], dx->mCurrentNormal, Vec2f(texCoords[4], texCoords[5]), dx->mCurrentColor),
			FixedVertex(verts[0], dx->mCurrentNormal, Vec2f(texCoords[6], texCoords[7]), dx->mCurrentColor)
		};
		applyDxFixedPipeline(fixedVerts, 4, COLOR_VERTEX, COLOR_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		dx->mDeviceContext->Draw(4, 0);
	}
//#endif
}

void draw( const TextureRef &texture )
{
	draw( texture, Area( texture->getCleanBounds() ), texture->getCleanBounds() );
}

void draw( const TextureRef &texture, const Vec2f &pos )
{
	//this group of code draws the texture using a center anchor
	//Vec2f extents(texture.getCleanWidth() * 0.5f, texture.getCleanHeight() * 0.5f);
	//Vec2f lowerLeft = pos - extents;
	//Vec2f upperRight = pos + extents;
	//draw( texture, texture.getCleanBounds(), Rectf( lowerLeft, upperRight ) );

	draw( texture, texture->getCleanBounds(), Rectf( pos.x, pos.y, pos.x + texture->getCleanWidth(), pos.y + texture->getCleanHeight() ) ); 
}

void draw( const TextureRef &texture, const Rectf &rect )
{
	draw( texture, texture->getCleanBounds(), rect );
}

void draw( const TextureRef &texture, const Area &srcArea, const Rectf &destRect )
{
//	if(usingGL())
//	{
//		SaveTextureBindState saveBindState( texture.getTarget() );
//		BoolState saveEnabledState( texture.getTarget() );
//		ClientBoolState vertexArrayState( GL_VERTEX_ARRAY );
//		ClientBoolState texCoordArrayState( GL_TEXTURE_COORD_ARRAY );	
//		texture.enableAndBind();
//
//		glEnableClientState( GL_VERTEX_ARRAY );
//		GLfloat verts[8];
//		glVertexPointer( 2, GL_FLOAT, 0, verts );
//		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
//		GLfloat texCoords[8];
//		glTexCoordPointer( 2, GL_FLOAT, 0, texCoords );
//
//		verts[0*2+0] = destRect.getX2(); verts[0*2+1] = destRect.getY1();	
//		verts[1*2+0] = destRect.getX1(); verts[1*2+1] = destRect.getY1();	
//		verts[2*2+0] = destRect.getX2(); verts[2*2+1] = destRect.getY2();	
//		verts[3*2+0] = destRect.getX1(); verts[3*2+1] = destRect.getY2();	
//
//		const Rectf srcCoords = texture.getAreaTexCoords( srcArea );
//		texCoords[0*2+0] = srcCoords.getX2(); texCoords[0*2+1] = srcCoords.getY1();	
//		texCoords[1*2+0] = srcCoords.getX1(); texCoords[1*2+1] = srcCoords.getY1();	
//		texCoords[2*2+0] = srcCoords.getX2(); texCoords[2*2+1] = srcCoords.getY2();	
//		texCoords[3*2+0] = srcCoords.getX1(); texCoords[3*2+1] = srcCoords.getY2();	
//
//		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
//	}
//#if defined( CINDER_MSW )
//	else
	{
		auto dx = getDxRenderer();
		const Rectf srcCoords = texture->getAreaTexCoords( srcArea );
		
		if(dx->getRenderFlag(app::AppImplMswRendererDx::BATCH_TEXTURE))
		{
			FixedVertex verts[6] = {
				FixedVertex(Vec3f(destRect.getX2(), destRect.getY1(), 0), dx->mCurrentNormal, Vec2f(srcCoords.getX2(), srcCoords.getY1()), dx->mCurrentColor),
				FixedVertex(Vec3f(destRect.getX1(), destRect.getY1(), 0), dx->mCurrentNormal, Vec2f(srcCoords.getX1(), srcCoords.getY1()), dx->mCurrentColor),
				FixedVertex(Vec3f(destRect.getX2(), destRect.getY2(), 0), dx->mCurrentNormal, Vec2f(srcCoords.getX2(), srcCoords.getY2()), dx->mCurrentColor),
				FixedVertex(Vec3f(destRect.getX1(), destRect.getY1(), 0), dx->mCurrentNormal, Vec2f(srcCoords.getX1(), srcCoords.getY1()), dx->mCurrentColor),
				FixedVertex(Vec3f(destRect.getX2(), destRect.getY2(), 0), dx->mCurrentNormal, Vec2f(srcCoords.getX2(), srcCoords.getY2()), dx->mCurrentColor),
				FixedVertex(Vec3f(destRect.getX1(), destRect.getY2(), 0), dx->mCurrentNormal, Vec2f(srcCoords.getX1(), srcCoords.getY2()), dx->mCurrentColor)
			};
			if(dx->mCurrentBatchTexture != texture.get() ) {
				dx->mCurrentBatchTexture = texture.get();
				dx->mBatchedTextures.push_back(std::pair<const Texture*, std::vector<FixedVertex>>( texture.get(), std::vector<FixedVertex>(verts, verts + 6)));
			}
			else
			{
				std::pair<const Texture*, std::vector<FixedVertex>> &back = dx->mBatchedTextures.back();
				std::vector<FixedVertex> &vertices = back.second;
				vertices.insert(vertices.end(), verts, verts + 6);
			}
		}
		else
		{
			FixedVertex verts[4] = {
				FixedVertex(Vec3f(destRect.getX2(), destRect.getY1(), 0), dx->mCurrentNormal, Vec2f(srcCoords.getX2(), srcCoords.getY1()), dx->mCurrentColor),
				FixedVertex(Vec3f(destRect.getX1(), destRect.getY1(), 0), dx->mCurrentNormal, Vec2f(srcCoords.getX1(), srcCoords.getY1()), dx->mCurrentColor),
				FixedVertex(Vec3f(destRect.getX2(), destRect.getY2(), 0), dx->mCurrentNormal, Vec2f(srcCoords.getX2(), srcCoords.getY2()), dx->mCurrentColor),
				FixedVertex(Vec3f(destRect.getX1(), destRect.getY2(), 0), dx->mCurrentNormal, Vec2f(srcCoords.getX1(), srcCoords.getY2()), dx->mCurrentColor)
			};
			applyDxFixedPipeline(verts, 4, TEXTURE_VERTEX, TEXTURE_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			texture->bind();
			dx->mDeviceContext->Draw(4, 0);
			texture->unbind();
		}
	}
//#endif
}

void batchTextureBegin()
{
	getDxRenderer()->setRenderFlag(app::AppImplMswRendererDx::BATCH_TEXTURE);
}

void batchTextureEnd()
{
	auto dx = getDxRenderer();
	std::vector<std::pair<const Texture*, std::vector<FixedVertex>>>::iterator pairEnd = dx->mBatchedTextures.end(), pairIt = dx->mBatchedTextures.begin();
	for(; pairIt != pairEnd; ++pairIt)
	{
		pairIt->first->bind();
		std::vector<FixedVertex> &vertices = pairIt->second;
		applyDxFixedPipeline(&vertices[0], vertices.size(), TEXTURE_VERTEX, TEXTURE_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		const size_t vertexCount = vertices.size();
		int batches = 0;
		for(int verticesProcessed = 0; verticesProcessed < (int)vertexCount - D3D_FL9_1_IA_PRIMITIVE_MAX_COUNT; verticesProcessed += D3D_FL9_1_IA_PRIMITIVE_MAX_COUNT, ++batches)
			dx->mDeviceContext->Draw(std::min(vertexCount, (size_t)D3D_FL9_1_IA_PRIMITIVE_MAX_COUNT), verticesProcessed);
		dx->mDeviceContext->Draw(vertexCount % D3D_FL9_1_IA_PRIMITIVE_MAX_COUNT, D3D_FL9_1_IA_PRIMITIVE_MAX_COUNT * batches);
	}
	dx->mBatchedTextures.clear();
	dx->mCurrentBatchTexture = nullptr;
	dx->clearRenderFlag(app::AppImplMswRendererDx::BATCH_TEXTURE);
}

namespace {
void drawStringHelper( const std::string &str, const Vec2f &pos, const ColorA &color, Font font, int justification )
{
	if( str.empty() )
		return;

	// justification: { left = -1, center = 0, right = 1 }
	//SaveColorState colorState;

	static Font defaultFont = Font::getDefault();
	if( ! font )
		font = defaultFont;

	float baselineOffset;
//#if defined( CINDER_COCOA_TOUCH )
//	Vec2i actualSize;
//	Surface8u pow2Surface( renderStringPow2( str, font, color, &actualSize, &baselineOffset ) );
//	gl::Texture tex( pow2Surface );
//	tex.setCleanTexCoords( actualSize.x / (float)pow2Surface.getWidth(), actualSize.y / (float)pow2Surface.getHeight() );
//	baselineOffset += pow2Surface.getHeight();
//#else
	dx::TextureRef tex = dx::Texture::create( renderString( str, font, color, &baselineOffset ) );
//#endif
	//glColor4ub( 255, 255, 255, 255 );
	auto dx = getDxRenderer();
	Vec4f oldColor = dx->mCurrentColor;
	dx::color(color);
	if( justification == -1 ) // left
		draw( tex, pos - Vec2f( 0, baselineOffset ) );
	else if( justification == 0 ) // center
		draw( tex, pos - Vec2f( tex->getWidth() * 0.5f, baselineOffset ) );	
	else // right
		draw( tex, pos - Vec2f( (float)tex->getWidth(), baselineOffset ) );
	dx->mCurrentColor = oldColor;
}
} // anonymous namespace

void drawString( const std::string &str, const Vec2f &pos, const ColorA &color, Font font )
{
	drawStringHelper( str, pos, color, font, -1 );
}

void drawStringCentered( const std::string &str, const Vec2f &pos, const ColorA &color, Font font )
{
	drawStringHelper( str, pos, color, font, 0 );
}

void drawStringRight( const std::string &str, const Vec2f &pos, const ColorA &color, Font font )
{
	drawStringHelper( str, pos, color, font, 1 );
}

void draw( const TextureRef &texture, const std::vector<float> &verts, const std::vector<float> &texCoords, const std::vector<ColorA8u> &vertColors, const std::vector<uint32_t> &indices )
{
	std::vector<FixedVertex> wholeVertices(verts.size() / 2);
	auto dx = getDxRenderer();
	for(unsigned i = 0; i < wholeVertices.size(); ++i)
	{
		wholeVertices[i].pos.x = verts[i * 2];
		wholeVertices[i].pos.y = verts[i * 2 + 1];
		wholeVertices[i].norm = dx->mCurrentNormal;
		wholeVertices[i].uv.x = texCoords[i * 2];
		wholeVertices[i].uv.y = texCoords[i * 2 + 1];
		wholeVertices[i].color = (vertColors.size()) ? Vec4f(vertColors[i].r / 255.0f, vertColors[i].g / 255.0f, vertColors[i].b / 255.0f, vertColors[i].a / 255.0f) : dx->mCurrentColor;
	}
	texture->bind();
	applyDxFixedPipeline(&wholeVertices[0], wholeVertices.size(), TEXTURE_VERTEX, TEXTURE_PIXEL, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3D11_MAPPED_SUBRESOURCE subresource;
	dx->mDeviceContext->Map(dx->mIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
	memcpy(subresource.pData, &indices[0], sizeof(uint32_t) * indices.size());
	dx->mDeviceContext->Unmap(dx->mIndexBuffer, 0);
	dx->mDeviceContext->IASetIndexBuffer(dx->mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	dx->mDeviceContext->DrawIndexed(indices.size(), 0, 0);
}

} } // namespace gl::cinder
