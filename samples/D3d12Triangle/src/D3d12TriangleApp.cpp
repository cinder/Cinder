#include "cinder/app/App.h"
#include "cinder/app/RendererD3d12.h"
#include "cinder/app/msw/RendererImplD3d12.h"
#include "cinder/ImageIo.h"
#include "cinder/Log.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

using namespace ci;
using namespace ci::app;

struct Vertex {
	float pos[3];
	float color[3];
};

class D3d12TriangleApp : public App {
  public:
	void setup() override;
	void cleanup() override;
	void resize() override;
	void draw() override;
	void keyDown( KeyEvent event ) override;

  private:
	void saveScreenshot();
	void waitForGpu();

	// App-managed D3D12 resources (minimal renderer pattern)
	ID3D12Device*				mDevice = nullptr;
	ID3D12CommandQueue*			mCommandQueue = nullptr;

	// Per-frame command allocators for proper triple buffering
	static const UINT FrameCount = 3;
	ID3D12CommandAllocator*		mCommandAllocators[FrameCount] = {};
	ID3D12GraphicsCommandList*	mCommandList = nullptr;

	// App rendering resources
	ID3D12RootSignature*	mRootSignature = nullptr;
	ID3D12PipelineState*	mPipelineState = nullptr;
	ID3D12Resource*			mVertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView = {};

	// App-managed synchronization (per-frame fences for triple buffering)
	ID3D12Fence*	mFence = nullptr;
	UINT64			mFenceValues[FrameCount] = {};
	HANDLE			mFenceEvent = nullptr;
};

void D3d12TriangleApp::setup()
{
	setWindowSize( 800, 800 );

	auto renderer = std::dynamic_pointer_cast<RendererD3d12>( getRenderer() );
	if( ! renderer )
		return;

	// Get device and queue from minimal renderer
	mDevice = static_cast<ID3D12Device*>( renderer->getDevice() );
	mCommandQueue = static_cast<ID3D12CommandQueue*>( renderer->getCommandQueue() );
	if( ! mDevice || ! mCommandQueue )
		return;

	// Create per-frame command allocators for proper triple buffering
	HRESULT hr = S_OK;
	for( UINT i = 0; i < FrameCount; i++ ) {
		hr = mDevice->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS( &mCommandAllocators[i] ) );
		if( FAILED( hr ) )
			return;
	}

	// Create app's own command list (using first allocator initially)
	hr = mDevice->CreateCommandList( 0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		mCommandAllocators[0], nullptr, IID_PPV_ARGS( &mCommandList ) );
	if( FAILED( hr ) )
		return;
	mCommandList->Close(); // Start closed

	// Create app's own fence for synchronization
	hr = mDevice->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &mFence ) );
	if( FAILED( hr ) )
		return;
	for( UINT i = 0; i < FrameCount; i++ )
		mFenceValues[i] = 0;
	mFenceEvent = CreateEvent( nullptr, FALSE, FALSE, nullptr );

	// Create root signature
	D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
	rootSigDesc.NumParameters = 0;
	rootSigDesc.pParameters = nullptr;
	rootSigDesc.NumStaticSamplers = 0;
	rootSigDesc.pStaticSamplers = nullptr;
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* signature = nullptr;
	ID3DBlob* error = nullptr;
	hr = D3D12SerializeRootSignature( &rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&signature, &error );
	if( FAILED( hr ) ) {
		if( error )
			error->Release();
		return;
	}
	if( error )
		error->Release();

	hr = mDevice->CreateRootSignature( 0, signature->GetBufferPointer(),
		signature->GetBufferSize(), IID_PPV_ARGS( &mRootSignature ) );
	signature->Release();
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

	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	hr = D3DCompile( vertexShaderSource, strlen( vertexShaderSource ), nullptr, nullptr, nullptr,
		"main", "vs_5_0", 0, 0, &vsBlob, &errorBlob );
	if( FAILED( hr ) ) {
		if( errorBlob )
			errorBlob->Release();
		return;
	}
	if( errorBlob )
		errorBlob->Release();

	ID3DBlob* psBlob = nullptr;
	hr = D3DCompile( pixelShaderSource, strlen( pixelShaderSource ), nullptr, nullptr, nullptr,
		"main", "ps_5_0", 0, 0, &psBlob, &errorBlob );
	if( FAILED( hr ) ) {
		if( errorBlob )
			errorBlob->Release();
		vsBlob->Release();
		return;
	}
	if( errorBlob )
		errorBlob->Release();

	// Create input layout
	D3D12_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	// Create pipeline state
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = mRootSignature;
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
	psoDesc.SampleDesc.Count = 1;

	hr = mDevice->CreateGraphicsPipelineState( &psoDesc, IID_PPV_ARGS( &mPipelineState ) );
	vsBlob->Release();
	psBlob->Release();
	if( FAILED( hr ) )
		return;

	// Create vertex buffer
	float sideLength = 1.6f;
	float halfSide = sideLength / 2.0f;
	float height = sideLength * 0.866025f;
	float centroidY = height / 3.0f;

	Vertex vertices[] = {
		{ { 0.0f, height - centroidY, 0.0f }, { 1.0f, 0.0f, 0.0f } },
		{ { halfSide, -centroidY, 0.0f }, { 0.0f, 0.0f, 1.0f } },
		{ { -halfSide, -centroidY, 0.0f }, { 0.0f, 1.0f, 0.0f } }
	};

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

	hr = mDevice->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS( &mVertexBuffer )
	);
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
	// Don't call waitForGpu() here - the renderer may have already destroyed the queue
	// The renderer's kill() already waits for GPU idle before destroying resources

	if( mVertexBuffer )
		mVertexBuffer->Release();
	if( mPipelineState )
		mPipelineState->Release();
	if( mRootSignature )
		mRootSignature->Release();
	if( mCommandList )
		mCommandList->Release();
	for( UINT i = 0; i < FrameCount; i++ ) {
		if( mCommandAllocators[i] )
			mCommandAllocators[i]->Release();
	}
	if( mFence )
		mFence->Release();
	if( mFenceEvent )
		CloseHandle( mFenceEvent );
}

void D3d12TriangleApp::resize()
{
	CI_LOG_I( "[App] resize: Starting..." );
	// Wait for all app GPU work to complete before resize
	// The renderer's defaultResize() will wait for present operations,
	// but we need to wait for our own command lists too
	waitForGpu();
	CI_LOG_I( "[App] resize: waitForGpu complete" );

	auto renderer = std::dynamic_pointer_cast<RendererD3d12>( getRenderer() );
	if( renderer )
		renderer->defaultResize();

	CI_LOG_I( "[App] resize: defaultResize complete, resetting fence values" );
	// Reset fence values after resize since buffers are new
	for( UINT i = 0; i < FrameCount; i++ ) {
		mFenceValues[i] = 0;
	}
	CI_LOG_I( "[App] resize: Complete" );
}

void D3d12TriangleApp::draw()
{
	auto renderer = std::dynamic_pointer_cast<RendererD3d12>( getRenderer() );
	if( ! renderer )
		return;

	// Get renderer's implementation to access back buffers and RTVs
	auto rendererImpl = renderer->mImpl;
	if( ! rendererImpl )
		return;

	UINT frameIndex = rendererImpl->mFrameIndex;
	ID3D12Resource* backBuffer = rendererImpl->mBackBuffers[frameIndex].get();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rendererImpl->mRtvHandles[frameIndex];

	// Back buffer may be null during resize - skip this frame
	if( ! backBuffer )
		return;

	// Get actual back buffer dimensions (may differ from window size during resize)
	D3D12_RESOURCE_DESC bufferDesc = backBuffer->GetDesc();
	UINT bufferWidth = static_cast<UINT>( bufferDesc.Width );
	UINT bufferHeight = bufferDesc.Height;

	// Wait for this frame's GPU work to complete before reusing its allocator
	const UINT64 currentFrameFenceValue = mFenceValues[frameIndex];
	if( currentFrameFenceValue != 0 && mFence->GetCompletedValue() < currentFrameFenceValue ) {
		mFence->SetEventOnCompletion( currentFrameFenceValue, mFenceEvent );
		WaitForSingleObject( mFenceEvent, INFINITE );
	}

	// Reset this frame's command allocator and list
	ID3D12CommandAllocator* frameAllocator = mCommandAllocators[frameIndex];
	frameAllocator->Reset();
	mCommandList->Reset( frameAllocator, mPipelineState );

	// Transition back buffer from PRESENT to RENDER_TARGET
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = backBuffer;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	mCommandList->ResourceBarrier( 1, &barrier );

	// Set viewport and scissor based on actual buffer size
	D3D12_VIEWPORT viewport = { 0.0f, 0.0f, (float)bufferWidth, (float)bufferHeight, 0.0f, 1.0f };
	D3D12_RECT scissor = { 0, 0, (LONG)bufferWidth, (LONG)bufferHeight };
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
		{ { 0.0f, topY * invHeight, 0.0f }, { 1.0f, 0.0f, 0.0f } },           // Top (red)
		{ { halfSide * invWidth, bottomY * invHeight, 0.0f }, { 0.0f, 0.0f, 1.0f } },   // Bottom-right (blue)
		{ { -halfSide * invWidth, bottomY * invHeight, 0.0f }, { 0.0f, 1.0f, 0.0f } }   // Bottom-left (green)
	};

	// Update vertex buffer
	void* data;
	if( SUCCEEDED( mVertexBuffer->Map( 0, nullptr, &data ) ) ) {
		memcpy( data, vertices, sizeof( vertices ) );
		mVertexBuffer->Unmap( 0, nullptr );
	}

	// Draw triangle
	mCommandList->SetGraphicsRootSignature( mRootSignature );
	mCommandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	mCommandList->IASetVertexBuffers( 0, 1, &mVertexBufferView );
	mCommandList->DrawInstanced( 3, 1, 0, 0 );

	// Transition back buffer from RENDER_TARGET to PRESENT
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	mCommandList->ResourceBarrier( 1, &barrier );

	// Close and execute
	mCommandList->Close();
	ID3D12CommandList* lists[] = { mCommandList };
	mCommandQueue->ExecuteCommandLists( 1, lists );

	// Signal fence for this frame
	const UINT64 currentFenceValue = mFenceValues[frameIndex] + 1;
	mCommandQueue->Signal( mFence, currentFenceValue );
	mFenceValues[frameIndex] = currentFenceValue;

	// Note: Don't call finishDraw() here - the framework calls it after draw() returns
}

void D3d12TriangleApp::waitForGpu()
{
	if( mFence && mCommandQueue ) {
		// Wait for all frames to complete
		UINT64 maxFenceValue = 0;
		for( UINT i = 0; i < FrameCount; i++ ) {
			if( mFenceValues[i] > maxFenceValue )
				maxFenceValue = mFenceValues[i];
		}

		if( maxFenceValue > 0 && mFence->GetCompletedValue() < maxFenceValue ) {
			mFence->SetEventOnCompletion( maxFenceValue, mFenceEvent );
			WaitForSingleObject( mFenceEvent, INFINITE );
		}
	}
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

CINDER_APP( D3d12TriangleApp, RendererD3d12 )
