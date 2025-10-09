#include "cinder/app/App.h"
#include "cinder/app/RendererD3d11.h"
#include "cinder/ImageIo.h"
#include <d3d11_1.h>
#include <d3dcompiler.h>

using namespace ci;
using namespace ci::app;

struct Vertex {
	float pos[3];
	float color[3];
};

class D3d11TriangleApp : public App {
  public:
	void setup() override;
	void cleanup() override;
	void draw() override;
	void keyDown( KeyEvent event ) override;

  private:
	void saveScreenshot();

	ID3D11VertexShader* mVertexShader = nullptr;
	ID3D11PixelShader*	mPixelShader = nullptr;
	ID3D11InputLayout*	mInputLayout = nullptr;
	ID3D11Buffer*		mVertexBuffer = nullptr;
};

void D3d11TriangleApp::setup()
{
	setWindowSize( 800, 800 );

	auto renderer = std::dynamic_pointer_cast<RendererD3d11>( getRenderer() );
	if( ! renderer )
		return;

	auto device = static_cast<ID3D11Device1*>( renderer->getDevice() );
	if( ! device )
		return;

	const char* vertexShaderSource = R"(
		struct VS_INPUT {
			float3 pos : POSITION;
			float3 color : COLOR;
		};
		struct PS_INPUT {
			float4 pos : SV_POSITION;
			float3 color : COLOR;
		};
		PS_INPUT main(VS_INPUT input) {
			PS_INPUT output;
			output.pos = float4(input.pos, 1.0f);
			output.color = input.color;
			return output;
		}
	)";

	const char* pixelShaderSource = R"(
		struct PS_INPUT {
			float4 pos : SV_POSITION;
			float3 color : COLOR;
		};
		float4 main(PS_INPUT input) : SV_TARGET {
			return float4(input.color, 1.0f);
		}
	)";

	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT	  hr = D3DCompile( vertexShaderSource, strlen( vertexShaderSource ), nullptr, nullptr, nullptr, "main", "vs_5_0", 0, 0, &vsBlob, &errorBlob );
	if( FAILED( hr ) ) {
		if( errorBlob )
			errorBlob->Release();
		return;
	}
	if( errorBlob )
		errorBlob->Release();

	device->CreateVertexShader( vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &mVertexShader );

	ID3DBlob* psBlob = nullptr;
	hr = D3DCompile( pixelShaderSource, strlen( pixelShaderSource ), nullptr, nullptr, nullptr, "main", "ps_5_0", 0, 0, &psBlob, &errorBlob );
	if( FAILED( hr ) ) {
		if( errorBlob )
			errorBlob->Release();
		vsBlob->Release();
		return;
	}
	if( errorBlob )
		errorBlob->Release();

	device->CreatePixelShader( psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &mPixelShader );
	psBlob->Release();

	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	device->CreateInputLayout( layout, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &mInputLayout );
	vsBlob->Release();

	float sideLength = 1.6f;
	float halfSide = sideLength / 2.0f;
	float height = sideLength * 0.866025f;
	float centroidY = height / 3.0f;

	Vertex vertices[] = { { { 0.0f, height - centroidY, 0.0f }, { 1.0f, 0.0f, 0.0f } }, { { halfSide, -centroidY, 0.0f }, { 0.0f, 0.0f, 1.0f } }, { { -halfSide, -centroidY, 0.0f }, { 0.0f, 1.0f, 0.0f } } };

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof( vertices );
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices;

	device->CreateBuffer( &bufferDesc, &initData, &mVertexBuffer );
}

void D3d11TriangleApp::cleanup()
{
	if( mVertexBuffer )
		mVertexBuffer->Release();
	if( mInputLayout )
		mInputLayout->Release();
	if( mPixelShader )
		mPixelShader->Release();
	if( mVertexShader )
		mVertexShader->Release();
}

void D3d11TriangleApp::draw()
{
	auto renderer = std::dynamic_pointer_cast<RendererD3d11>( getRenderer() );
	if( ! renderer )
		return;

	renderer->makeCurrentContext();
	renderer->clear( ColorA( 0.0f, 0.0f, 0.0f, 1.0f ) );

	auto context = static_cast<ID3D11DeviceContext1*>( renderer->getDeviceContext() );
	if( ! context )
		return;

	context->VSSetShader( mVertexShader, nullptr, 0 );
	context->PSSetShader( mPixelShader, nullptr, 0 );
	context->IASetInputLayout( mInputLayout );

	UINT stride = sizeof( Vertex );
	UINT offset = 0;
	context->IASetVertexBuffers( 0, 1, &mVertexBuffer, &stride, &offset );
	context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	context->Draw( 3, 0 );
}

void D3d11TriangleApp::keyDown( KeyEvent event )
{
	if( event.isControlDown() && event.getCode() == KeyEvent::KEY_s )
		saveScreenshot();
}

void D3d11TriangleApp::saveScreenshot()
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

CINDER_APP( D3d11TriangleApp, RendererD3d11 )
