/*
 MetalImageBasic Sample

 Demonstrates basic Metal image loading and display.

 Features:
 - Metal texture creation from Cinder Surface8u
 - Metal shader compilation (vertex and fragment shaders)
 - File drop support for loading images
 - Ctrl+O to open image file dialog
 - Ctrl+S to save screenshot

 Controls:
 - Ctrl+O: Open image file
 - Ctrl+S: Save screenshot
 - Drag and drop: Load image file
*/

#include "cinder/app/App.h"
#include "cinder/app/RendererMetal.h"
#include "cinder/ImageIo.h"
#include "cinder/Surface.h"
#include "cinder/CanvasUi.h"
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

using namespace ci;
using namespace ci::app;

struct Vertex {
	float pos[3];
	float texCoord[2];
};

class MetalImageBasicApp : public App {
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

	id<MTLDevice>				mDevice = nil;
	id<MTLCommandQueue>			mCommandQueue = nil;
	id<MTLRenderPipelineState>	mPipelineState = nil;
	id<MTLBuffer>				mVertexBuffer = nil;
	id<MTLTexture>				mTexture = nil;
	id<MTLSamplerState>			mSamplerState = nil;

	CanvasUi					mCanvas;
	ivec2						mImageSize{ 0, 0 };
	bool						mCaptureMode = false;
	bool						mCaptured = false;
};

void MetalImageBasicApp::setup()
{
	// Check for --capture command line argument
	for( const auto& arg : getCommandLineArgs() ) {
		if( arg == "--capture" ) {
			mCaptureMode = true;
			break;
		}
	}

	setWindowSize( 800, 600 );

	auto renderer = std::dynamic_pointer_cast<RendererMetal>( getRenderer() );
	CI_ASSERT( renderer && renderer->getMetalDevice() && renderer->getMetalCommandQueue() );

	mDevice = (__bridge id<MTLDevice>)renderer->getMetalDevice();
	mCommandQueue = (__bridge id<MTLCommandQueue>)renderer->getMetalCommandQueue();

	// Metal shader source code
	const char* shaderSource = R"(
		#include <metal_stdlib>
		using namespace metal;

		struct Vertex {
			float3 position [[attribute(0)]];
			float2 texCoord [[attribute(1)]];
		};

		struct VertexOut {
			float4 position [[position]];
			float2 texCoord;
		};

		vertex VertexOut vertexShader(Vertex in [[stage_in]]) {
			VertexOut out;
			out.position = float4(in.position, 1.0);
			out.texCoord = in.texCoord;
			return out;
		}

		fragment float4 fragmentShader(VertexOut in [[stage_in]],
		                               texture2d<float> tex [[texture(0)]],
		                               sampler samp [[sampler(0)]]) {
			return tex.sample(samp, in.texCoord);
		}
	)";

	// Compile shaders
	NSError* error = nil;
	id<MTLLibrary> library = [mDevice newLibraryWithSource:[NSString stringWithUTF8String:shaderSource]
												   options:nil
													 error:&error];
	if( ! library ) {
		console() << "Failed to compile Metal shaders" << std::endl;
		if( error ) {
			NSLog(@"Shader compilation error: %@", error);
		}
		return;
	}

	id<MTLFunction> vertexFunction = [library newFunctionWithName:@"vertexShader"];
	id<MTLFunction> fragmentFunction = [library newFunctionWithName:@"fragmentShader"];

	// Create vertex descriptor
	MTLVertexDescriptor* vertexDescriptor = [[MTLVertexDescriptor alloc] init];
	// Position attribute
	vertexDescriptor.attributes[0].format = MTLVertexFormatFloat3;
	vertexDescriptor.attributes[0].offset = 0;
	vertexDescriptor.attributes[0].bufferIndex = 0;
	// TexCoord attribute
	vertexDescriptor.attributes[1].format = MTLVertexFormatFloat2;
	vertexDescriptor.attributes[1].offset = sizeof(float) * 3;
	vertexDescriptor.attributes[1].bufferIndex = 0;
	// Layout
	vertexDescriptor.layouts[0].stride = sizeof(Vertex);
	vertexDescriptor.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;

	// Create render pipeline
	MTLRenderPipelineDescriptor* pipelineDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
	pipelineDescriptor.vertexFunction = vertexFunction;
	pipelineDescriptor.fragmentFunction = fragmentFunction;
	pipelineDescriptor.vertexDescriptor = vertexDescriptor;
	pipelineDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;

	mPipelineState = [mDevice newRenderPipelineStateWithDescriptor:pipelineDescriptor error:&error];
	if( ! mPipelineState ) {
		console() << "Failed to create pipeline state" << std::endl;
		return;
	}

	// Create vertex buffer
	mVertexBuffer = [mDevice newBufferWithLength:sizeof(Vertex) * 4
										 options:MTLResourceStorageModeShared];

	// Create sampler state
	MTLSamplerDescriptor* samplerDescriptor = [[MTLSamplerDescriptor alloc] init];
	samplerDescriptor.minFilter = MTLSamplerMinMagFilterLinear;
	samplerDescriptor.magFilter = MTLSamplerMinMagFilterLinear;
	samplerDescriptor.sAddressMode = MTLSamplerAddressModeClampToEdge;
	samplerDescriptor.tAddressMode = MTLSamplerAddressModeClampToEdge;
	mSamplerState = [mDevice newSamplerStateWithDescriptor:samplerDescriptor];

	// Connect CanvasUi with higher priority so it receives events first
	mCanvas.connect( getWindow(), 100 );

	// Prompt user to open an image
	openImageDialog();
}

void MetalImageBasicApp::cleanup()
{
	if( mSamplerState ) {
		[mSamplerState release];
		mSamplerState = nil;
	}
	if( mTexture ) {
		[mTexture release];
		mTexture = nil;
	}
	if( mVertexBuffer ) {
		[mVertexBuffer release];
		mVertexBuffer = nil;
	}
	if( mPipelineState ) {
		[mPipelineState release];
		mPipelineState = nil;
	}
	// Don't release mDevice, mCommandQueue - renderer owns them
	mCommandQueue = nil;
	mDevice = nil;
}

void MetalImageBasicApp::draw()
{
	if( ! mDevice || ! mCommandQueue )
		return;

	@try {
	@autoreleasepool {
		auto renderer = std::dynamic_pointer_cast<RendererMetal>( getRenderer() );
		if( ! renderer )
			return;

		// Clear background
		renderer->makeCurrentContext();

		CAMetalLayer* metalLayer = (__bridge CAMetalLayer*)renderer->getMetalLayer();
		if( ! metalLayer )
			return;

		id<CAMetalDrawable> drawable = [metalLayer nextDrawable];
		if( ! drawable )
			return;

		MTLRenderPassDescriptor* renderPass = [MTLRenderPassDescriptor renderPassDescriptor];
		renderPass.colorAttachments[0].texture = drawable.texture;
		renderPass.colorAttachments[0].loadAction = MTLLoadActionClear;
		renderPass.colorAttachments[0].clearColor = MTLClearColorMake( 0.2, 0.2, 0.25, 1.0 );
		renderPass.colorAttachments[0].storeAction = MTLStoreActionStore;

		if( ! mTexture ) {
			// Just present the clear color
			id<MTLCommandBuffer> commandBuffer = [mCommandQueue commandBuffer];
			id<MTLRenderCommandEncoder> encoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPass];
			[encoder endEncoding];
			[commandBuffer presentDrawable:drawable];
			[commandBuffer commit];
			return;
		}

		// Use CanvasUi's actual matrix
		mat4 modelMatrix = mCanvas.getModelMatrix();

		// Define quad corners in content space
		float halfWidth = mImageSize.x * 0.5f;
		float halfHeight = mImageSize.y * 0.5f;

		// Transform content-space corners to window-space
		vec4 windowPos[4] = {
			modelMatrix * vec4( -halfWidth, -halfHeight, 0.0f, 1.0f ),  // Top-left
			modelMatrix * vec4(  halfWidth, -halfHeight, 0.0f, 1.0f ),  // Top-right
			modelMatrix * vec4( -halfWidth,  halfHeight, 0.0f, 1.0f ),  // Bottom-left
			modelMatrix * vec4(  halfWidth,  halfHeight, 0.0f, 1.0f )   // Bottom-right
		};

		// Convert window-space to NDC
		vec2 windowSize = vec2( getWindowSize() );

		// Metal uses Y-down in window space, Y-up in NDC
		// Triangle strip order: TL, TR, BL, BR
		Vertex vertices[] = {
			{ { windowPos[0].x / windowSize.x * 2.0f - 1.0f,
				1.0f - windowPos[0].y / windowSize.y * 2.0f, 0.0f }, { 0.0f, 0.0f } },  // TL
			{ { windowPos[1].x / windowSize.x * 2.0f - 1.0f,
				1.0f - windowPos[1].y / windowSize.y * 2.0f, 0.0f }, { 1.0f, 0.0f } },  // TR
			{ { windowPos[2].x / windowSize.x * 2.0f - 1.0f,
				1.0f - windowPos[2].y / windowSize.y * 2.0f, 0.0f }, { 0.0f, 1.0f } },  // BL
			{ { windowPos[3].x / windowSize.x * 2.0f - 1.0f,
				1.0f - windowPos[3].y / windowSize.y * 2.0f, 0.0f }, { 1.0f, 1.0f } }   // BR
		};

		std::memcpy( [mVertexBuffer contents], vertices, sizeof( vertices ) );

		id<MTLCommandBuffer> commandBuffer = [mCommandQueue commandBuffer];
		id<MTLRenderCommandEncoder> encoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPass];

		[encoder setRenderPipelineState:mPipelineState];
		[encoder setVertexBuffer:mVertexBuffer offset:0 atIndex:0];
		[encoder setFragmentTexture:mTexture atIndex:0];
		[encoder setFragmentSamplerState:mSamplerState atIndex:0];
		[encoder drawPrimitives:MTLPrimitiveTypeTriangleStrip vertexStart:0 vertexCount:4];

		[encoder endEncoding];
		[commandBuffer presentDrawable:drawable];
		[commandBuffer commit];
		// Wait until scheduled to prevent tearing from modifying vertex buffer while GPU is using it
		[commandBuffer waitUntilScheduled];

		// Handle capture mode
		if( mCaptureMode && ! mCaptured ) {
			mCaptured = true;
			try {
				writeImage( "metalimagebasic_capture.png", copyWindowSurface() );
			}
			catch( const std::exception& e ) {
				console() << "Failed to save capture: " << e.what() << std::endl;
			}
			quit();
		}
	} // @autoreleasepool
	}
	@catch( NSException* exception ) {
		console() << "Exception in draw: " << [[exception description] UTF8String] << std::endl;
	}
}

void MetalImageBasicApp::keyDown( KeyEvent event )
{
	if( event.isControlDown() && event.getCode() == KeyEvent::KEY_o ) {
		openImageDialog();
	}
	else if( event.isControlDown() && event.getCode() == KeyEvent::KEY_s ) {
		saveScreenshot();
	}
}

void MetalImageBasicApp::fileDrop( FileDropEvent event )
{
	loadImage( event.getFile( 0 ) );
}

void MetalImageBasicApp::loadImage( const fs::path &path )
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

void MetalImageBasicApp::openImageDialog()
{
	fs::path path = getOpenFilePath( "", ImageIo::getLoadExtensions() );
	if( ! path.empty() ) {
		loadImage( path );
	}
}

void MetalImageBasicApp::saveScreenshot()
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

void MetalImageBasicApp::createTextureFromSurface( const Surface8u &surface )
{
	if( ! mDevice )
		return;

	if( mTexture ) {
		[mTexture release];
		mTexture = nil;
	}

	mImageSize = ivec2( surface.getWidth(), surface.getHeight() );

	// Convert to RGBA if necessary
	Surface8u rgbaSurface( surface.getWidth(), surface.getHeight(), true, SurfaceChannelOrder::RGBA );
	rgbaSurface.copyFrom( surface, surface.getBounds() );

	// Create Metal texture
	MTLTextureDescriptor* textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
																								  width:rgbaSurface.getWidth()
																								 height:rgbaSurface.getHeight()
																							  mipmapped:NO];
	textureDescriptor.usage = MTLTextureUsageShaderRead;

	mTexture = [mDevice newTextureWithDescriptor:textureDescriptor];
	if( ! mTexture )
		return;

	MTLRegion region = MTLRegionMake2D( 0, 0, rgbaSurface.getWidth(), rgbaSurface.getHeight() );
	[mTexture replaceRegion:region
				mipmapLevel:0
				  withBytes:rgbaSurface.getData()
				bytesPerRow:rgbaSurface.getRowBytes()];

	// Set canvas bounds - centered at origin
	float halfWidth = mImageSize.x * 0.5f;
	float halfHeight = mImageSize.y * 0.5f;
	mCanvas.setContentBounds( Rectf( -halfWidth, -halfHeight, halfWidth, halfHeight ) );
	mCanvas.fitAll();
}

CINDER_APP( MetalImageBasicApp, RendererMetal )
