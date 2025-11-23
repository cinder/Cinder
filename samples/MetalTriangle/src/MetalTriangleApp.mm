#include "cinder/app/App.h"
#include "cinder/app/RendererMetal.h"
#include "cinder/CinderMath.h"
#include "cinder/ImageIo.h"
#include <cstring>
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

using namespace ci;
using namespace ci::app;

struct Vertex {
	float pos[3];
	float color[3];
};

class MetalTriangleApp : public App {
  public:
	void setup() override;
	void cleanup() override;
	void draw() override;
	void resize() override;
	void keyDown( KeyEvent event ) override;

  private:
	void saveScreenshot();
	void updateTriangleVertices();

	id<MTLDevice>				mDevice = nil;
	id<MTLCommandQueue>			mCommandQueue = nil;
	id<MTLRenderPipelineState>	mPipelineState = nil;
	id<MTLBuffer>				mVertexBuffer = nil;
};

void MetalTriangleApp::setup()
{
	setWindowSize( 800, 800 );

	auto renderer = std::dynamic_pointer_cast<RendererMetal>( getRenderer() );
	if( ! renderer ) {
		console() << "ERROR: Failed to get RendererMetal!" << std::endl;
		return;
	}

	// Get Metal device and command queue from renderer
	mDevice = (__bridge id<MTLDevice>)renderer->getMetalDevice();
	mCommandQueue = (__bridge id<MTLCommandQueue>)renderer->getMetalCommandQueue();

	if( ! mDevice || ! mCommandQueue ) {
		console() << "ERROR: Failed to get Metal objects!" << std::endl;
		return;
	}
	// Metal shader source code
	const char* shaderSource = R"(
		#include <metal_stdlib>
		using namespace metal;

		struct Vertex {
			float3 position [[attribute(0)]];
			float3 color [[attribute(1)]];
		};

		struct VertexOut {
			float4 position [[position]];
			float3 color;
		};

		vertex VertexOut vertexShader(Vertex in [[stage_in]]) {
			VertexOut out;
			out.position = float4(in.position, 1.0);
			out.color = in.color;
			return out;
		}

		fragment float4 fragmentShader(VertexOut in [[stage_in]]) {
			return float4(in.color, 1.0);
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
	@try {
		MTLVertexDescriptor* vertexDescriptor = [[MTLVertexDescriptor alloc] init];
		// Position attribute
		vertexDescriptor.attributes[0].format = MTLVertexFormatFloat3;
		vertexDescriptor.attributes[0].offset = 0;
		vertexDescriptor.attributes[0].bufferIndex = 0;
		// Color attribute
		vertexDescriptor.attributes[1].format = MTLVertexFormatFloat3;
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

		mVertexBuffer = [mDevice newBufferWithLength:sizeof(Vertex) * 3
											 options:MTLResourceStorageModeShared];
		updateTriangleVertices();
	}
	@catch( NSException* exception ) {
		console() << "Exception during setup: " << [[exception description] UTF8String] << std::endl;
	}
}

void MetalTriangleApp::cleanup()
{
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

void MetalTriangleApp::draw()
{
	if( ! mDevice || ! mCommandQueue )
		return;

	@try {
	@autoreleasepool {
		// Get Metal layer from renderer
		auto renderer = std::dynamic_pointer_cast<RendererMetal>( getRenderer() );
		if( ! renderer ) {
			console() << "ERROR: Failed to get RendererMetal!" << std::endl;
			return;
		}

		CAMetalLayer* metalLayer = (__bridge CAMetalLayer*)renderer->getMetalLayer();
		if( ! metalLayer ) {
			console() << "ERROR: No Metal layer!" << std::endl;
			return;
		}

		id<CAMetalDrawable> drawable = [metalLayer nextDrawable];
		if( ! drawable )
			return;

		MTLRenderPassDescriptor* renderPass = [MTLRenderPassDescriptor renderPassDescriptor];
		renderPass.colorAttachments[0].texture = drawable.texture;
		renderPass.colorAttachments[0].loadAction = MTLLoadActionClear;
		renderPass.colorAttachments[0].clearColor = MTLClearColorMake( 0.0, 0.0, 0.0, 1.0 );
		renderPass.colorAttachments[0].storeAction = MTLStoreActionStore;

		id<MTLCommandBuffer> commandBuffer = [mCommandQueue commandBuffer];
		id<MTLRenderCommandEncoder> encoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPass];

		[encoder setRenderPipelineState:mPipelineState];
		[encoder setVertexBuffer:mVertexBuffer offset:0 atIndex:0];
		[encoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];

		[encoder endEncoding];
		[commandBuffer presentDrawable:drawable];
		[commandBuffer commit];
	} // @autoreleasepool
	}
	@catch( NSException* exception ) {
		console() << "Exception in draw: " << [[exception description] UTF8String] << std::endl;
	}
}

void MetalTriangleApp::resize() { updateTriangleVertices(); }

void MetalTriangleApp::keyDown( KeyEvent event )
{
	if( event.isControlDown() && event.getCode() == KeyEvent::KEY_s )
		saveScreenshot();
}

void MetalTriangleApp::saveScreenshot()
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

void MetalTriangleApp::updateTriangleVertices()
{
	if( ! mVertexBuffer )
		return;

	const float width = math<float>::max( 1.0f, float( getWindowWidth() ) );
	const float height = math<float>::max( 1.0f, float( getWindowHeight() ) );
	const float sqrt3 = math<float>::sqrt( 3.0f );
	const float side = math<float>::min( width, ( 2.0f / sqrt3 ) * height );
	const float halfSide = side * 0.5f;
	const float triHeight = side * sqrt3 * 0.5f;
	const float invWidth = 2.0f / width;
	const float invHeight = 2.0f / height;
	const float topY = triHeight * 0.5f;
	const float bottomY = -topY;

	const Vertex vertices[3] = {
		{ { 0.0f, topY * invHeight, 0.0f }, { 1.0f, 0.0f, 0.0f } },
		{ { halfSide * invWidth, bottomY * invHeight, 0.0f }, { 0.0f, 0.0f, 1.0f } },
		{ { -halfSide * invWidth, bottomY * invHeight, 0.0f }, { 0.0f, 1.0f, 0.0f } }
	};

	std::memcpy( [mVertexBuffer contents], vertices, sizeof( vertices ) );
}

CINDER_APP( MetalTriangleApp, RendererMetal )
