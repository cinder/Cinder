#include "cinder/app/App.h"
#include "cinder/app/RendererD3d12.h"
#include "cinder/ImageIo.h"
#include "cinder/Log.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <wrl/client.h>

using namespace ci;
using namespace ci::app;
using Microsoft::WRL::ComPtr;

struct Vertex {
	float pos[3];
	float color[3];
};

class D3d12TriangleApp : public App {
  public:
	void setup() override;
	void cleanup() override;
	void draw() override;
	void keyDown( KeyEvent event ) override;

  private:
	void saveScreenshot();

	// Borrowed pointers from renderer (not owned, don't use ComPtr)
	ID3D12Device*		mDevice = nullptr;
	ID3D12CommandQueue* mCommandQueue = nullptr;

	// Per-frame command allocators for proper triple buffering
	ComPtr<ID3D12CommandAllocator>	  mCommandAllocators[RendererD3d12::MaxFrameCount];
	ComPtr<ID3D12GraphicsCommandList> mCommandList;

	// App rendering resources
	ComPtr<ID3D12RootSignature> mRootSignature;
	ComPtr<ID3D12PipelineState> mPipelineState;
	ComPtr<ID3D12Resource>		mVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW	mVertexBufferView = {};

	// Cached renderer pointer
	RendererD3d12Ref mRenderer;
};

void D3d12TriangleApp::setup()
{
	setWindowSize( 800, 800 );

	mRenderer = std::dynamic_pointer_cast<RendererD3d12>( getRenderer() );
	if( ! mRenderer )
		return;

	// Get device and queue from minimal renderer
	mDevice = mRenderer->getDevice();
	mCommandQueue = mRenderer->getCommandQueue();
	if( ! mDevice || ! mCommandQueue )
		return;

	// Create per-frame command allocators for proper triple buffering
	HRESULT hr = S_OK;
	for( UINT i = 0; i < RendererD3d12::MaxFrameCount; i++ ) {
		hr = mDevice->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &mCommandAllocators[i] ) );
		if( FAILED( hr ) )
			return;
	}

	// Create app's own command list (using first allocator initially)
	hr = mDevice->CreateCommandList( 0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocators[0].Get(), nullptr, IID_PPV_ARGS( &mCommandList ) );
	if( FAILED( hr ) )
		return;
	mCommandList->Close(); // Start closed

	// Create root signature
	D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
	rootSigDesc.NumParameters = 0;
	rootSigDesc.pParameters = nullptr;
	rootSigDesc.NumStaticSamplers = 0;
	rootSigDesc.pStaticSamplers = nullptr;
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	hr = ::D3D12SerializeRootSignature( &rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error );
	if( FAILED( hr ) )
		return;

	hr = mDevice->CreateRootSignature( 0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS( &mRootSignature ) );
	if( FAILED( hr ) )
		return;

	// Compile shaders
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

	ComPtr<ID3DBlob> vsBlob;
	ComPtr<ID3DBlob> errorBlob;
	hr = ::D3DCompile( vertexShaderSource, strlen( vertexShaderSource ), nullptr, nullptr, nullptr, "main", "vs_5_0", 0, 0, &vsBlob, &errorBlob );
	if( FAILED( hr ) )
		return;

	ComPtr<ID3DBlob> psBlob;
	hr = ::D3DCompile( pixelShaderSource, strlen( pixelShaderSource ), nullptr, nullptr, nullptr, "main", "ps_5_0", 0, 0, &psBlob, &errorBlob );
	if( FAILED( hr ) )
		return;

	// Create input layout
	D3D12_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	// Create pipeline state - use MSAA sample count from renderer
	UINT msaaSamples = mRenderer->getMsaaSamples();
	CI_LOG_I( "[App] Creating PSO with MSAA " << msaaSamples << "x" );

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = mRootSignature.Get();
	psoDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
	psoDesc.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };
	psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.InputLayout = { layout, _countof( layout ) };
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = msaaSamples;

	hr = mDevice->CreateGraphicsPipelineState( &psoDesc, IID_PPV_ARGS( &mPipelineState ) );
	if( FAILED( hr ) )
		return;

	// Create vertex buffer
	float sideLength = 1.6f;
	float halfSide = sideLength / 2.0f;
	float height = sideLength * 0.866025f;
	float centroidY = height / 3.0f;

	Vertex vertices[] = { { { 0.0f, height - centroidY, 0.0f }, { 1.0f, 0.0f, 0.0f } }, { { halfSide, -centroidY, 0.0f }, { 0.0f, 0.0f, 1.0f } }, { { -halfSide, -centroidY, 0.0f }, { 0.0f, 1.0f, 0.0f } } };

	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC bufferDesc = {};
	bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferDesc.Width = sizeof( vertices );
	bufferDesc.Height = 1;
	bufferDesc.DepthOrArraySize = 1;
	bufferDesc.MipLevels = 1;
	bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	bufferDesc.SampleDesc.Count = 1;
	bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	hr = mDevice->CreateCommittedResource( &heapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( &mVertexBuffer ) );
	if( FAILED( hr ) )
		return;

	// Copy vertex data
	void* data;
	hr = mVertexBuffer->Map( 0, nullptr, &data );
	if( SUCCEEDED( hr ) ) {
		memcpy( data, vertices, sizeof( vertices ) );
		mVertexBuffer->Unmap( 0, nullptr );
	}

	// Create vertex buffer view
	mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
	mVertexBufferView.SizeInBytes = sizeof( vertices );
	mVertexBufferView.StrideInBytes = sizeof( Vertex );
}

void D3d12TriangleApp::cleanup()
{
	// ComPtr handles Release automatically
}

void D3d12TriangleApp::draw()
{
	if( ! mRenderer )
		return;

	// Get back buffer info from renderer
	UINT			frameIndex = mRenderer->getCurrentBackBufferIndex();
	ID3D12Resource* backBuffer = mRenderer->getCurrentBackBuffer();

	// Back buffer may be null during resize - skip this frame
	if( ! backBuffer ) {
		CI_LOG_W( "[App] draw: SKIPPING - backBuffer is null!" );
		return;
	}

	// Get the appropriate render target handle - MSAA target if enabled, back buffer otherwise
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = mRenderer->getRenderTargetHandle();
	bool						msaaEnabled = mRenderer->isMsaaEnabled();

	// Get render target dimensions from MSAA target if enabled, otherwise from back buffer
	UINT bufferWidth, bufferHeight;
	if( msaaEnabled && mRenderer->getMsaaTarget() ) {
		D3D12_RESOURCE_DESC msaaDesc = mRenderer->getMsaaTarget()->GetDesc();
		bufferWidth = static_cast<UINT>( msaaDesc.Width );
		bufferHeight = msaaDesc.Height;
	}
	else {
		D3D12_RESOURCE_DESC bufferDesc = backBuffer->GetDesc();
		bufferWidth = static_cast<UINT>( bufferDesc.Width );
		bufferHeight = bufferDesc.Height;
	}

	// RendererD3d12::startDraw() has already waited for this back buffer's previous GPU work
	// to complete, so per-frame resources indexed by frameIndex are safe to reset here.

	// Reset this frame's command allocator and list
	ID3D12CommandAllocator* frameAllocator = mCommandAllocators[frameIndex].Get();
	HRESULT					hr = frameAllocator->Reset();
	if( FAILED( hr ) ) {
		CI_LOG_E( "[App] draw: FAILED to reset allocator, hr=" << std::hex << hr );
	}
	hr = mCommandList->Reset( frameAllocator, mPipelineState.Get() );
	if( FAILED( hr ) ) {
		CI_LOG_E( "[App] draw: FAILED to reset command list, hr=" << std::hex << hr );
	}

	// When MSAA is disabled, transition back buffer from PRESENT to RENDER_TARGET
	// (MSAA target is always in RENDER_TARGET state)
	if( ! msaaEnabled ) {
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = backBuffer;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		mCommandList->ResourceBarrier( 1, &barrier );
	}

	// Set viewport and scissor based on actual buffer size
	D3D12_VIEWPORT viewport = { 0.0f, 0.0f, (float)bufferWidth, (float)bufferHeight, 0.0f, 1.0f };
	D3D12_RECT	   scissor = { 0, 0, (LONG)bufferWidth, (LONG)bufferHeight };
	mCommandList->RSSetViewports( 1, &viewport );
	mCommandList->RSSetScissorRects( 1, &scissor );

	// Clear and set render target
	float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	mCommandList->ClearRenderTargetView( rtvHandle, clearColor, 0, nullptr );
	mCommandList->OMSetRenderTargets( 1, &rtvHandle, FALSE, nullptr );

	// Compute largest equilateral triangle that fits in the buffer
	// Match MetalTriangle logic: side = min(width, (2/sqrt(3)) * height)
	float width = (float)bufferWidth;
	float height = (float)bufferHeight;
	float sqrt3 = 1.732050808f;

	float side = std::min( width, ( 2.0f / sqrt3 ) * height );
	float halfSide = side * 0.5f;
	float triHeight = side * sqrt3 * 0.5f;
	float topY = triHeight * 0.5f;
	float bottomY = -topY;

	// Normalize to NDC [-1, 1]
	float invWidth = 2.0f / width;
	float invHeight = 2.0f / height;

	Vertex vertices[] = {
		{ { 0.0f, topY * invHeight, 0.0f }, { 1.0f, 0.0f, 0.0f } },					  // Top (red)
		{ { halfSide * invWidth, bottomY * invHeight, 0.0f }, { 0.0f, 0.0f, 1.0f } }, // Bottom-right (blue)
		{ { -halfSide * invWidth, bottomY * invHeight, 0.0f }, { 0.0f, 1.0f, 0.0f } } // Bottom-left (green)
	};

	// Update vertex buffer
	void* data;
	if( SUCCEEDED( mVertexBuffer->Map( 0, nullptr, &data ) ) ) {
		memcpy( data, vertices, sizeof( vertices ) );
		mVertexBuffer->Unmap( 0, nullptr );
	}

	// Draw triangle
	mCommandList->SetGraphicsRootSignature( mRootSignature.Get() );
	mCommandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	mCommandList->IASetVertexBuffers( 0, 1, &mVertexBufferView );
	mCommandList->DrawInstanced( 3, 1, 0, 0 );

	// Handle end-of-frame transitions
	if( msaaEnabled ) {
		// MSAA: resolve to back buffer (handles all barriers internally)
		mRenderer->recordMsaaResolve( mCommandList.Get(), D3D12_RESOURCE_STATE_PRESENT );
	}
	else {
		// No MSAA: transition back buffer from RENDER_TARGET to PRESENT
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = backBuffer;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		mCommandList->ResourceBarrier( 1, &barrier );
	}

	// Close and execute
	hr = mCommandList->Close();
	if( FAILED( hr ) ) {
		CI_LOG_E( "[App] draw: FAILED to close command list, hr=" << std::hex << hr );
	}
	ID3D12CommandList* lists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists( 1, lists );

	// Note: Don't call finishDraw() here - the framework calls it after draw() returns
}

void D3d12TriangleApp::keyDown( KeyEvent event )
{
	if( event.isControlDown() && event.getCode() == KeyEvent::KEY_s )
		saveScreenshot();
	else if( event.isControlDown() && event.getCode() == KeyEvent::KEY_f )
		setFullScreen( ! isFullScreen() );
}

void D3d12TriangleApp::saveScreenshot()
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

CINDER_APP( D3d12TriangleApp, RendererD3d12( RendererD3d12::Options().msaa( 8 ) ) )
