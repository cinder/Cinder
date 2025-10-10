/*
 D3d11ImageBasic Sample

 Demonstrates basic Direct3D 11 image loading and display.

 Features:
 - Direct3D 11 texture creation from Cinder Surface8u
 - HLSL shader compilation (vertex and pixel shaders)
 - File drop support for loading images
 - Ctrl+O to open image file dialog
 - Ctrl+S to save screenshot

 Controls:
 - Ctrl+O: Open image file
 - Ctrl+S: Save screenshot
 - Drag and drop: Load image file
*/

#include "cinder/app/App.h"
#include "cinder/app/RendererD3d11.h"
#include "cinder/ImageIo.h"
#include "cinder/Surface.h"
#include "cinder/CanvasUi.h"
#include <d3d11_1.h>
#include <d3dcompiler.h>

using namespace ci;
using namespace ci::app;

struct Vertex {
	float pos[3];
	float texCoord[2];
};

class D3d11ImageBasicApp : public App {
  public:
	void setup() override;
	void cleanup() override;
	void draw() override;
	void keyDown( KeyEvent event ) override;
	void fileDrop( FileDropEvent event ) override;

  private:
	void loadImage( const fs::path &path );
	void openImageDialog();
	void saveScreenshot();
	void createTextureFromSurface( const Surface8u &surface );

	ID3D11VertexShader*			mVertexShader = nullptr;
	ID3D11PixelShader*			mPixelShader = nullptr;
	ID3D11InputLayout*			mInputLayout = nullptr;
	ID3D11Buffer*				mVertexBuffer = nullptr;
	ID3D11Texture2D*			mTexture = nullptr;
	ID3D11ShaderResourceView*	mTextureView = nullptr;
	ID3D11SamplerState*			mSamplerState = nullptr;

	CanvasUi					mCanvas;
	ivec2						mImageSize{ 0, 0 };
	bool						mCaptureMode = false;
	bool						mCaptured = false;
};

void D3d11ImageBasicApp::setup()
{
	// Check for --capture command line argument
	for( const auto& arg : getCommandLineArgs() ) {
		if( arg == "--capture" ) {
			mCaptureMode = true;
			break;
		}
	}

	setWindowSize( 800, 600 );

	auto renderer = std::dynamic_pointer_cast<RendererD3d11>( getRenderer() );
	if( ! renderer )
		return;

	auto device = static_cast<ID3D11Device1*>( renderer->getDevice() );
	if( ! device )
		return;

	const char* vertexShaderSource = R"(
		struct VS_INPUT {
			float3 pos : POSITION;
			float2 texCoord : TEXCOORD;
		};
		struct PS_INPUT {
			float4 pos : SV_POSITION;
			float2 texCoord : TEXCOORD;
		};
		PS_INPUT main(VS_INPUT input) {
			PS_INPUT output;
			output.pos = float4(input.pos, 1.0f);
			output.texCoord = input.texCoord;
			return output;
		}
	)";

	const char* pixelShaderSource = R"(
		Texture2D shaderTexture : register(t0);
		SamplerState samplerState : register(s0);

		struct PS_INPUT {
			float4 pos : SV_POSITION;
			float2 texCoord : TEXCOORD;
		};
		float4 main(PS_INPUT input) : SV_TARGET {
			return shaderTexture.Sample(samplerState, input.texCoord);
		}
	)";

	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompile( vertexShaderSource, strlen( vertexShaderSource ), nullptr, nullptr, nullptr,
		"main", "vs_5_0", 0, 0, &vsBlob, &errorBlob );
	if( FAILED( hr ) ) {
		if( errorBlob ) errorBlob->Release();
		return;
	}
	if( errorBlob ) errorBlob->Release();

	device->CreateVertexShader( vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &mVertexShader );

	ID3DBlob* psBlob = nullptr;
	hr = D3DCompile( pixelShaderSource, strlen( pixelShaderSource ), nullptr, nullptr, nullptr,
		"main", "ps_5_0", 0, 0, &psBlob, &errorBlob );
	if( FAILED( hr ) ) {
		if( errorBlob ) errorBlob->Release();
		vsBlob->Release();
		return;
	}
	if( errorBlob ) errorBlob->Release();

	device->CreatePixelShader( psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &mPixelShader );
	psBlob->Release();

	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	device->CreateInputLayout( layout, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &mInputLayout );
	vsBlob->Release();

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof( Vertex ) * 4;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	device->CreateBuffer( &bufferDesc, nullptr, &mVertexBuffer );

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState( &samplerDesc, &mSamplerState );

	// Connect CanvasUi
	mCanvas.connect( getWindow() );

	// Prompt user to open an image
	openImageDialog();
}

void D3d11ImageBasicApp::cleanup()
{
	if( mSamplerState ) mSamplerState->Release();
	if( mTextureView ) mTextureView->Release();
	if( mTexture ) mTexture->Release();
	if( mVertexBuffer ) mVertexBuffer->Release();
	if( mInputLayout ) mInputLayout->Release();
	if( mPixelShader ) mPixelShader->Release();
	if( mVertexShader ) mVertexShader->Release();
}

void D3d11ImageBasicApp::draw()
{
	auto renderer = std::dynamic_pointer_cast<RendererD3d11>( getRenderer() );
	if( ! renderer )
		return;

	renderer->makeCurrentContext();
	renderer->clear( ColorA( 0.2f, 0.2f, 0.25f, 1.0f ) );

	if( ! mTextureView )
		return;

	auto context = static_cast<ID3D11DeviceContext1*>( renderer->getDeviceContext() );
	if( ! context )
		return;

	// Use CanvasUi's actual matrix
	mat4 modelMatrix = mCanvas.getModelMatrix();

	// Define quad corners in content space
	float halfWidth = mImageSize.x * 0.5f;
	float halfHeight = mImageSize.y * 0.5f;

	// Transform content-space corners to window-space
	// CanvasUi designed for OpenGL (Y-up in both content and NDC), but we're using D3D11 (Y-down in window)
	// Flip content Y to compensate: what was "top" (+Y) in content should go to top (small Y) in window
	vec4 windowPos[4] = {
		modelMatrix * vec4( -halfWidth, -halfHeight, 0.0f, 1.0f ),  // Top-left (flip: use negative Y)
		modelMatrix * vec4(  halfWidth, -halfHeight, 0.0f, 1.0f ),  // Top-right
		modelMatrix * vec4( -halfWidth,  halfHeight, 0.0f, 1.0f ),  // Bottom-left (flip: use positive Y)
		modelMatrix * vec4(  halfWidth,  halfHeight, 0.0f, 1.0f )   // Bottom-right
	};

	// Convert window-space to NDC and arrange for triangle strip
	// Triangle strip needs: TL, TR, BL, BR (in NDC space where Y increases upward)
	// After conversion, windowPos top/bottom are flipped due to Y-axis difference
	vec2 windowSize = vec2( getWindowSize() );

	// windowPos[0/1] have smaller window Y (top in window space) -> larger NDC Y (top in NDC)
	// windowPos[2/3] have larger window Y (bottom in window space) -> smaller NDC Y (bottom in NDC)
	// Triangle strip order: TL, TR, BL, BR
	Vertex vertices[] = {
		{ { windowPos[0].x / windowSize.x * 2.0f - 1.0f,
			1.0f - windowPos[0].y / windowSize.y * 2.0f, 0.0f }, { 0.0f, 0.0f } },  // TL (window top)
		{ { windowPos[1].x / windowSize.x * 2.0f - 1.0f,
			1.0f - windowPos[1].y / windowSize.y * 2.0f, 0.0f }, { 1.0f, 0.0f } },  // TR (window top)
		{ { windowPos[2].x / windowSize.x * 2.0f - 1.0f,
			1.0f - windowPos[2].y / windowSize.y * 2.0f, 0.0f }, { 0.0f, 1.0f } },  // BL (window bottom)
		{ { windowPos[3].x / windowSize.x * 2.0f - 1.0f,
			1.0f - windowPos[3].y / windowSize.y * 2.0f, 0.0f }, { 1.0f, 1.0f } }   // BR (window bottom)
	};

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if( SUCCEEDED( context->Map( mVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) ) ) {
		memcpy( mappedResource.pData, vertices, sizeof( vertices ) );
		context->Unmap( mVertexBuffer, 0 );
	}

	context->VSSetShader( mVertexShader, nullptr, 0 );
	context->PSSetShader( mPixelShader, nullptr, 0 );
	context->IASetInputLayout( mInputLayout );

	UINT stride = sizeof( Vertex );
	UINT offset = 0;
	context->IASetVertexBuffers( 0, 1, &mVertexBuffer, &stride, &offset );
	context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	context->PSSetShaderResources( 0, 1, &mTextureView );
	context->PSSetSamplers( 0, 1, &mSamplerState );

	context->Draw( 4, 0 );

	// Handle capture mode
	if( mCaptureMode && ! mCaptured ) {
		mCaptured = true;
		try {
			writeImage( "d3d11imagebasic_capture.png", copyWindowSurface() );
		}
		catch( const std::exception& e ) {
			console() << "Failed to save capture: " << e.what() << std::endl;
		}
		quit();
	}
}

void D3d11ImageBasicApp::keyDown( KeyEvent event )
{
	if( event.isControlDown() && event.getCode() == KeyEvent::KEY_o ) {
		openImageDialog();
	}
	else if( event.isControlDown() && event.getCode() == KeyEvent::KEY_s ) {
		saveScreenshot();
	}
}

void D3d11ImageBasicApp::fileDrop( FileDropEvent event )
{
	loadImage( event.getFile( 0 ) );
}

void D3d11ImageBasicApp::loadImage( const fs::path &path )
{
	try {
		ImageSourceRef imageSource = ci::loadImage( path );
		Surface8u surface = Surface8u( imageSource );
		createTextureFromSurface( surface );
		getWindow()->setTitle( path.filename().string() );
	}
	catch( const std::exception& e ) {
		console() << "Failed to load image: " << e.what() << std::endl;
	}
}

void D3d11ImageBasicApp::openImageDialog()
{
	fs::path path = getOpenFilePath( "", ImageIo::getLoadExtensions() );
	if( ! path.empty() ) {
		loadImage( path );
	}
}

void D3d11ImageBasicApp::saveScreenshot()
{
	fs::path savePath = getSaveFilePath();
	if( ! savePath.empty() ) {
		try {
			writeImage( savePath, copyWindowSurface() );
		}
		catch( const std::exception& e ) {
			console() << "Failed to save screenshot: " << e.what() << std::endl;
		}
	}
}

void D3d11ImageBasicApp::createTextureFromSurface( const Surface8u &surface )
{
	auto renderer = std::dynamic_pointer_cast<RendererD3d11>( getRenderer() );
	if( ! renderer )
		return;

	auto device = static_cast<ID3D11Device1*>( renderer->getDevice() );
	if( ! device )
		return;

	if( mTextureView ) {
		mTextureView->Release();
		mTextureView = nullptr;
	}
	if( mTexture ) {
		mTexture->Release();
		mTexture = nullptr;
	}

	mImageSize = ivec2( surface.getWidth(), surface.getHeight() );

	// Convert to RGBA if necessary
	Surface8u rgbaSurface( surface.getWidth(), surface.getHeight(), true, SurfaceChannelOrder::RGBA );
	rgbaSurface.copyFrom( surface, surface.getBounds() );

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = rgbaSurface.getWidth();
	textureDesc.Height = rgbaSurface.getHeight();
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = rgbaSurface.getData();
	initData.SysMemPitch = static_cast<UINT>( rgbaSurface.getRowBytes() );

	HRESULT hr = device->CreateTexture2D( &textureDesc, &initData, &mTexture );
	if( FAILED( hr ) )
		return;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView( mTexture, &srvDesc, &mTextureView );

	// Set canvas bounds - centered at origin
	float halfWidth = mImageSize.x * 0.5f;
	float halfHeight = mImageSize.y * 0.5f;
	mCanvas.setContentBounds( Rectf( -halfWidth, -halfHeight, halfWidth, halfHeight ) );
	mCanvas.fitAll();
}

CINDER_APP( D3d11ImageBasicApp, RendererD3d11 )
