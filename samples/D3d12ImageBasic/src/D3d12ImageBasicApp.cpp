/*
 D3d12ImageBasic Sample - Proof of Concept

 Demonstrates basic Direct3D 12 image loading with ImGui integration

 Features:
 - Direct3D 12 texture creation from Cinder Surface8u
 - HLSL shader compilation (vertex and pixel shaders)
 - File drop support for loading images
 - Ctrl+O to open image file dialog
 - Ctrl+S to save screenshot
 - App-managed D3D12 command lists (minimal renderer pattern)
 - ImGui integration for image info display

 Controls:
 - Ctrl+O: Open image file
 - Ctrl+S: Save screenshot
 - Drag and drop: Load image file
*/

#include "cinder/app/App.h"
#include "cinder/app/RendererD3d12.h"
#include "cinder/ImageIo.h"
#include "cinder/Surface.h"
#include "cinder/CanvasUi.h"
#include "cinder/Log.h"
#include <d3d12.h>
#include <d3dcompiler.h>
#include <wrl/client.h>

// ImGui - uses CinderImGui for input handling, imgui_impl_dx12 for rendering
#include "cinder/CinderImGui.h"
#include "imgui/imgui_impl_dx12.h"

using namespace ci;
using namespace ci::app;
using Microsoft::WRL::ComPtr;

struct Vertex {
	float pos[3];
	float texCoord[2];
};

class D3d12ImageBasicApp : public App {
  public:
	void setup() override;
	void update() override;
	void draw() override;
	void keyDown( KeyEvent event ) override;
	void fileDrop( FileDropEvent event ) override;

  private:
	void loadImage( const fs::path& path );
	void processFileDialogs();
	void saveScreenshot( const fs::path& path );
	void createTextureFromSurface( const Surface8u& surface );
	void drawGui();

	// Borrowed pointers from renderer (not owned, don't use ComPtr)
	ID3D12Device*		mDevice = nullptr;
	ID3D12CommandQueue* mCommandQueue = nullptr;

	// Per-frame command allocators for proper triple buffering
	ComPtr<ID3D12CommandAllocator>	  mCommandAllocators[RendererD3d12::MaxFrameCount];
	ComPtr<ID3D12GraphicsCommandList> mCommandList;

	// App rendering resources
	ComPtr<ID3D12RootSignature> mRootSignature;
	ComPtr<ID3D12PipelineState> mPipelineState;
	// Per-frame vertex buffers to avoid GPU/CPU races during animation
	ComPtr<ID3D12Resource>	 mVertexBuffers[RendererD3d12::MaxFrameCount];
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferViews[RendererD3d12::MaxFrameCount] = {};
	ComPtr<ID3D12Resource>	 mTexture;
	ComPtr<ID3D12Resource>	 mTextureUploadHeap;
	ComPtr<ID3D12DescriptorHeap> mSrvHeap;

	// Cached renderer pointer
	RendererD3d12Ref mRenderer;

	CanvasUi mCanvas;
	ivec2	 mImageSize{ 0, 0 };
	bool	 mCaptureMode = false;
	bool	 mCaptured = false;

	// Image source info for ImGui display
	ImageSourceRef mImageSource;
	fs::path	   mImagePath;
	std::string	   mErrorMessage;

	// Deferred file dialog requests (avoid modal dialogs during draw)
	bool mOpenRequested = false;
	bool mSaveRequested = false;
};

void D3d12ImageBasicApp::setup()
{
	// Check for --capture command line argument
	for( const auto& arg : getCommandLineArgs() ) {
		if( arg == "--capture" ) {
			mCaptureMode = true;
			break;
		}
	}

	setWindowSize( 800, 600 );

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

	// Create root signature with one descriptor table (SRV for texture) and one static sampler
	D3D12_DESCRIPTOR_RANGE ranges[1] = {};
	ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	ranges[0].NumDescriptors = 1;
	ranges[0].BaseShaderRegister = 0;
	ranges[0].RegisterSpace = 0;
	ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER rootParameters[1] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
	rootParameters[0].DescriptorTable.pDescriptorRanges = ranges;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC staticSampler = {};
	staticSampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSampler.MinLOD = 0;
	staticSampler.MaxLOD = D3D12_FLOAT32_MAX;
	staticSampler.ShaderRegister = 0;
	staticSampler.RegisterSpace = 0;
	staticSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
	rootSigDesc.NumParameters = 1;
	rootSigDesc.pParameters = rootParameters;
	rootSigDesc.NumStaticSamplers = 1;
	rootSigDesc.pStaticSamplers = &staticSampler;
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
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	// Create pipeline state
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
	psoDesc.SampleDesc.Count = 1;

	hr = mDevice->CreateGraphicsPipelineState( &psoDesc, IID_PPV_ARGS( &mPipelineState ) );
	if( FAILED( hr ) )
		return;

	// Create vertex buffer (dynamic - will be updated per frame)
	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC bufferDesc = {};
	bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferDesc.Width = sizeof( Vertex ) * 4;
	bufferDesc.Height = 1;
	bufferDesc.DepthOrArraySize = 1;
	bufferDesc.MipLevels = 1;
	bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	bufferDesc.SampleDesc.Count = 1;
	bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// Create per-frame vertex buffers
	for( UINT i = 0; i < RendererD3d12::MaxFrameCount; i++ ) {
		hr = mDevice->CreateCommittedResource( &heapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( &mVertexBuffers[i] ) );
		if( FAILED( hr ) )
			return;

		mVertexBufferViews[i].BufferLocation = mVertexBuffers[i]->GetGPUVirtualAddress();
		mVertexBufferViews[i].SizeInBytes = sizeof( Vertex ) * 4;
		mVertexBufferViews[i].StrideInBytes = sizeof( Vertex );
	}

	// Create SRV descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	hr = mDevice->CreateDescriptorHeap( &srvHeapDesc, IID_PPV_ARGS( &mSrvHeap ) );
	if( FAILED( hr ) )
		return;

	// Connect CanvasUi
	mCanvas.connect( getWindow() );

	// Initialize ImGui - CinderImGui handles input via signals, we handle D3D12 rendering
	ImGui::Initialize();

	// Request file dialog on first frame (deferred to avoid modal during setup)
	mOpenRequested = true;
}

void D3d12ImageBasicApp::update()
{
	// Process deferred file dialogs outside of draw loop
	processFileDialogs();
}

void D3d12ImageBasicApp::processFileDialogs()
{
	if( mOpenRequested ) {
		mOpenRequested = false;
		fs::path path = getOpenFilePath( "", ImageIo::getLoadExtensions() );
		if( ! path.empty() ) {
			loadImage( path );
		}
	}

	if( mSaveRequested ) {
		mSaveRequested = false;
		fs::path path = getSaveFilePath();
		if( ! path.empty() ) {
			saveScreenshot( path );
		}
	}
}

void D3d12ImageBasicApp::draw()
{
	if( ! mRenderer )
		return;

	// Get back buffer info from renderer
	UINT						frameIndex = mRenderer->getCurrentBackBufferIndex();
	ID3D12Resource*				backBuffer = mRenderer->getCurrentBackBuffer();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = mRenderer->getCurrentRtvHandle();

	// Back buffer may be null during resize - skip this frame
	if( ! backBuffer )
		return;

	// Get actual back buffer dimensions (may differ from window size during resize)
	D3D12_RESOURCE_DESC bufferDesc = backBuffer->GetDesc();
	UINT				bufferWidth = static_cast<UINT>( bufferDesc.Width );
	UINT				bufferHeight = bufferDesc.Height;

	// RendererD3d12::startDraw() has already waited for this back buffer's previous GPU work
	// to complete, so per-frame resources indexed by frameIndex are safe to reset here.

	// Reset this frame's command allocator and list
	ID3D12CommandAllocator* frameAllocator = mCommandAllocators[frameIndex].Get();
	frameAllocator->Reset();
	mCommandList->Reset( frameAllocator, mPipelineState.Get() );

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
	D3D12_RECT	   scissor = { 0, 0, (LONG)bufferWidth, (LONG)bufferHeight };
	mCommandList->RSSetViewports( 1, &viewport );
	mCommandList->RSSetScissorRects( 1, &scissor );

	// Clear and set render target
	float clearColor[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	mCommandList->ClearRenderTargetView( rtvHandle, clearColor, 0, nullptr );
	mCommandList->OMSetRenderTargets( 1, &rtvHandle, FALSE, nullptr );

	// Draw image if we have a texture
	if( mTexture ) {
		// Use CanvasUi's matrix
		mat4 modelMatrix = mCanvas.getModelMatrix();
		vec2 bufferSize = vec2( bufferWidth, bufferHeight );

		// Define quad corners in content space
		float halfWidth = mImageSize.x * 0.5f;
		float halfHeight = mImageSize.y * 0.5f;

		// Transform content-space corners to window-space
		vec4 windowPos[4] = { modelMatrix * vec4( -halfWidth, -halfHeight, 0.0f, 1.0f ), modelMatrix * vec4( halfWidth, -halfHeight, 0.0f, 1.0f ), modelMatrix * vec4( -halfWidth, halfHeight, 0.0f, 1.0f ),
			modelMatrix * vec4( halfWidth, halfHeight, 0.0f, 1.0f ) };

		vec2   windowSize = bufferSize;
		Vertex vertices[] = { { { windowPos[0].x / windowSize.x * 2.0f - 1.0f, 1.0f - windowPos[0].y / windowSize.y * 2.0f, 0.0f }, { 0.0f, 0.0f } },
			{ { windowPos[1].x / windowSize.x * 2.0f - 1.0f, 1.0f - windowPos[1].y / windowSize.y * 2.0f, 0.0f }, { 1.0f, 0.0f } },
			{ { windowPos[2].x / windowSize.x * 2.0f - 1.0f, 1.0f - windowPos[2].y / windowSize.y * 2.0f, 0.0f }, { 0.0f, 1.0f } },
			{ { windowPos[3].x / windowSize.x * 2.0f - 1.0f, 1.0f - windowPos[3].y / windowSize.y * 2.0f, 0.0f }, { 1.0f, 1.0f } } };

		// Update this frame's vertex buffer
		void*	data;
		HRESULT hr = mVertexBuffers[frameIndex]->Map( 0, nullptr, &data );
		if( SUCCEEDED( hr ) ) {
			memcpy( data, vertices, sizeof( vertices ) );
			mVertexBuffers[frameIndex]->Unmap( 0, nullptr );
		}

		// Set descriptor heaps and root signature
		ID3D12DescriptorHeap* heaps[] = { mSrvHeap.Get() };
		mCommandList->SetDescriptorHeaps( 1, heaps );
		mCommandList->SetGraphicsRootSignature( mRootSignature.Get() );
		mCommandList->SetGraphicsRootDescriptorTable( 0, mSrvHeap->GetGPUDescriptorHandleForHeapStart() );

		// Draw quad using this frame's vertex buffer
		mCommandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		mCommandList->IASetVertexBuffers( 0, 1, &mVertexBufferViews[frameIndex] );
		mCommandList->DrawInstanced( 4, 1, 0, 0 );
	}

	// Draw ImGui - CinderImGui manages context and input, we just need to render
	if( ID3D12DescriptorHeap* imguiHeap = ImGui::GetD3D12SrvHeap() ) {
		// Set ImGui descriptor heap
		ID3D12DescriptorHeap* imguiHeaps[] = { imguiHeap };
		mCommandList->SetDescriptorHeaps( 1, imguiHeaps );

		ImGui_ImplDX12_NewFrame();

		// Set display size and delta time (CinderImGui handles input, but D3D12 apps
		// must set these since auto-render is disabled)
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2( (float)bufferWidth, (float)bufferHeight );
		static double lastTime = 0.0;
		double		  currentTime = getElapsedSeconds();
		io.DeltaTime = lastTime > 0.0 ? (float)( currentTime - lastTime ) : ( 1.0f / 60.0f );
		lastTime = currentTime;

		ImGui::NewFrame();
		drawGui();
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData( ImGui::GetDrawData(), mCommandList.Get() );
	}

	// Transition back buffer from RENDER_TARGET to PRESENT
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	mCommandList->ResourceBarrier( 1, &barrier );

	// Close and execute
	mCommandList->Close();
	ID3D12CommandList* lists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists( 1, lists );

	// Handle capture mode
	if( mCaptureMode && ! mCaptured ) {
		mCaptured = true;
		try {
			writeImage( "d3d12imagebasic_capture.png", copyWindowSurface() );
		}
		catch( const std::exception& e ) {
			console() << "Failed to save capture: " << e.what() << std::endl;
		}
		quit();
	}
}

void D3d12ImageBasicApp::keyDown( KeyEvent event )
{
	// App shortcuts (ImGui input is handled by CinderImGui via signals)
	if( event.isControlDown() && event.getCode() == KeyEvent::KEY_o ) {
		mOpenRequested = true;
	}
	else if( event.isControlDown() && event.getCode() == KeyEvent::KEY_s ) {
		mSaveRequested = true;
	}
	else if( event.isControlDown() && event.getCode() == KeyEvent::KEY_f ) {
		setFullScreen( ! isFullScreen() );
	}
}

void D3d12ImageBasicApp::fileDrop( FileDropEvent event )
{
	loadImage( event.getFile( 0 ) );
}

void D3d12ImageBasicApp::loadImage( const fs::path& path )
{
	try {
		mImagePath = path;
		mImageSource = ci::loadImage( path );
		Surface8u surface = Surface8u( mImageSource );
		createTextureFromSurface( surface );
		mCanvas.setContentBounds( Rectf( -mImageSize.x * 0.5f, -mImageSize.y * 0.5f, mImageSize.x * 0.5f, mImageSize.y * 0.5f ) );
		mCanvas.fitAll();
		mErrorMessage.clear();
		getWindow()->setTitle( path.filename().string() );
	}
	catch( const std::exception& e ) {
		CI_LOG_EXCEPTION( "failed to load image.", e );
		mTexture.Reset();
		mImageSource.reset();
		mImagePath.clear();
		mErrorMessage = e.what();
	}
}

void D3d12ImageBasicApp::saveScreenshot( const fs::path& path )
{
	try {
		writeImage( path, copyWindowSurface() );
	}
	catch( const std::exception& e ) {
		console() << "Failed to save screenshot: " << e.what() << std::endl;
	}
}

void D3d12ImageBasicApp::createTextureFromSurface( const Surface8u& surface )
{
	if( ! mDevice )
		return;

	// Wait for GPU before releasing old resources
	if( mRenderer )
		mRenderer->waitForGpu();

	mTextureUploadHeap.Reset();
	mTexture.Reset();

	mImageSize = ivec2( surface.getWidth(), surface.getHeight() );

	// Convert to RGBA if necessary
	Surface8u rgbaSurface( surface.getWidth(), surface.getHeight(), true, SurfaceChannelOrder::RGBA );
	rgbaSurface.copyFrom( surface, surface.getBounds() );

	// Create texture resource
	D3D12_RESOURCE_DESC textureDesc = {};
	textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureDesc.Width = rgbaSurface.getWidth();
	textureDesc.Height = rgbaSurface.getHeight();
	textureDesc.DepthOrArraySize = 1;
	textureDesc.MipLevels = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	D3D12_HEAP_PROPERTIES defaultHeapProps = {};
	defaultHeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

	HRESULT hr = mDevice->CreateCommittedResource( &defaultHeapProps, D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS( &mTexture ) );
	if( FAILED( hr ) )
		return;

	// Calculate required upload buffer size manually
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprintCalc = {};
	UINT							   numRowsCalc = 0;
	UINT64							   rowSizeInBytesCalc = 0;
	UINT64							   uploadBufferSize = 0;

	D3D12_RESOURCE_DESC descCalc = mTexture->GetDesc();
	mDevice->GetCopyableFootprints( &descCalc, 0, 1, 0, &footprintCalc, &numRowsCalc, &rowSizeInBytesCalc, &uploadBufferSize );

	// Create upload heap
	D3D12_HEAP_PROPERTIES uploadHeapProps = {};
	uploadHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC uploadBufferDesc = {};
	uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	uploadBufferDesc.Width = uploadBufferSize;
	uploadBufferDesc.Height = 1;
	uploadBufferDesc.DepthOrArraySize = 1;
	uploadBufferDesc.MipLevels = 1;
	uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	uploadBufferDesc.SampleDesc.Count = 1;
	uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	hr = mDevice->CreateCommittedResource( &uploadHeapProps, D3D12_HEAP_FLAG_NONE, &uploadBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( &mTextureUploadHeap ) );
	if( FAILED( hr ) )
		return;

	// Create temporary command list for upload
	ComPtr<ID3D12CommandAllocator> uploadAllocator;
	hr = mDevice->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &uploadAllocator ) );
	if( FAILED( hr ) )
		return;

	ComPtr<ID3D12GraphicsCommandList> uploadList;
	hr = mDevice->CreateCommandList( 0, D3D12_COMMAND_LIST_TYPE_DIRECT, uploadAllocator.Get(), nullptr, IID_PPV_ARGS( &uploadList ) );
	if( FAILED( hr ) )
		return;

	// Manually copy texture data without using UpdateSubresources helper
	void* uploadData;
	hr = mTextureUploadHeap->Map( 0, nullptr, &uploadData );
	if( SUCCEEDED( hr ) ) {
		const BYTE* srcData = rgbaSurface.getData();
		BYTE*		destData = static_cast<BYTE*>( uploadData );
		size_t		srcRowPitch = rgbaSurface.getRowBytes();

		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
		UINT							   numRows = 0;
		UINT64							   rowSizeInBytes = 0;
		UINT64							   totalBytes = 0;

		D3D12_RESOURCE_DESC desc = mTexture->GetDesc();
		mDevice->GetCopyableFootprints( &desc, 0, 1, 0, &footprint, &numRows, &rowSizeInBytes, &totalBytes );

		// Copy row by row
		for( UINT y = 0; y < numRows; y++ )
			memcpy( destData + footprint.Footprint.RowPitch * y, srcData + srcRowPitch * y, rowSizeInBytes );

		mTextureUploadHeap->Unmap( 0, nullptr );

		// Copy from upload heap to texture
		D3D12_TEXTURE_COPY_LOCATION dst = {};
		dst.pResource = mTexture.Get();
		dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dst.SubresourceIndex = 0;

		D3D12_TEXTURE_COPY_LOCATION src = {};
		src.pResource = mTextureUploadHeap.Get();
		src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		src.PlacedFootprint = footprint;

		uploadList->CopyTextureRegion( &dst, 0, 0, 0, &src, nullptr );
	}

	// Transition texture to shader resource
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = mTexture.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	uploadList->ResourceBarrier( 1, &barrier );

	uploadList->Close();
	ID3D12CommandList* uploadLists[] = { uploadList.Get() };
	mCommandQueue->ExecuteCommandLists( 1, uploadLists );

	// Wait for upload to complete before releasing resources
	mRenderer->waitForGpu();

	// Create SRV
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	mDevice->CreateShaderResourceView( mTexture.Get(), &srvDesc, mSrvHeap->GetCPUDescriptorHandleForHeapStart() );

	// Set canvas bounds - centered at origin
	float halfWidth = mImageSize.x * 0.5f;
	float halfHeight = mImageSize.y * 0.5f;
	mCanvas.setContentBounds( Rectf( -halfWidth, -halfHeight, halfWidth, halfHeight ) );
	mCanvas.fitAll();
}

void D3d12ImageBasicApp::drawGui()
{
	ImGui::Begin( "Image Info" );

	if( ImGui::Button( "Open Image" ) )
		mOpenRequested = true;

	ImGui::SameLine();

	ImGui::BeginDisabled( ! mTexture );
	if( ImGui::Button( "Save Image" ) )
		mSaveRequested = true;
	ImGui::EndDisabled();

	ImGui::SameLine();

	ImGui::BeginDisabled( ! mTexture );
	if( ImGui::Button( "Fit to Window" ) ) {
		mCanvas.setContentBounds( Rectf( -mImageSize.x * 0.5f, -mImageSize.y * 0.5f, mImageSize.x * 0.5f, mImageSize.y * 0.5f ) );
		mCanvas.fitAll();
	}
	ImGui::EndDisabled();

	ImGui::Separator();

	if( mTexture && mImageSource ) {
		ImGui::Text( "File: %s", mImagePath.filename().string().c_str() );
		ImGui::Separator();
		ImGui::Text( "Dimensions: %d x %d", mImageSize.x, mImageSize.y );
		ImGui::Text( "Texture Format: DXGI_FORMAT_R8G8B8A8_UNORM" );
		ImGui::Separator();

		// Image source information
		const char* colorModelStr = "Unknown";
		switch( mImageSource->getColorModel() ) {
			case ImageIo::CM_RGB:
				colorModelStr = "RGB";
				break;
			case ImageIo::CM_GRAY:
				colorModelStr = "Grayscale";
				break;
			default:
				break;
		}
		ImGui::Text( "Color Model: %s", colorModelStr );

		const char* dataTypeStr = "Unknown";
		switch( mImageSource->getDataType() ) {
			case ImageIo::UINT8:
				dataTypeStr = "UINT8";
				break;
			case ImageIo::UINT16:
				dataTypeStr = "UINT16";
				break;
			case ImageIo::FLOAT16:
				dataTypeStr = "FLOAT16";
				break;
			case ImageIo::FLOAT32:
				dataTypeStr = "FLOAT32";
				break;
			default:
				break;
		}
		ImGui::Text( "Data Type: %s", dataTypeStr );

		const char* channelOrderStr = "Unknown";
		switch( mImageSource->getChannelOrder() ) {
			case ImageIo::Y:
				channelOrderStr = "Y (Grayscale)";
				break;
			case ImageIo::YA:
				channelOrderStr = "YA (Grayscale + Alpha)";
				break;
			case ImageIo::RGB:
				channelOrderStr = "RGB";
				break;
			case ImageIo::RGBA:
				channelOrderStr = "RGBA";
				break;
			case ImageIo::RGBX:
				channelOrderStr = "RGBX";
				break;
			case ImageIo::BGR:
				channelOrderStr = "BGR";
				break;
			case ImageIo::BGRA:
				channelOrderStr = "BGRA";
				break;
			case ImageIo::BGRX:
				channelOrderStr = "BGRX";
				break;
			default:
				break;
		}
		ImGui::Text( "Channel Order: %s", channelOrderStr );

		ImGui::Text( "Has Alpha: %s", mImageSource->hasAlpha() ? "Yes" : "No" );
		ImGui::Text( "Premultiplied: %s", mImageSource->isPremultiplied() ? "Yes" : "No" );
	}
	else {
		ImGui::Text( "No image loaded" );
		ImGui::Text( "Click 'Open Image' or drag & drop an image file" );

		if( ! mErrorMessage.empty() ) {
			ImGui::Separator();
			ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.0f, 0.3f, 0.3f, 1.0f ) );
			ImGui::TextWrapped( "Error: %s", mErrorMessage.c_str() );
			ImGui::PopStyleColor();
		}
	}

	ImGui::End();
}

CINDER_APP( D3d12ImageBasicApp, RendererD3d12 )
