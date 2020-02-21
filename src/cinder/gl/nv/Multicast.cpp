#include "cinder/gl/nv/Multicast.h"

#include "cinder/gl/Texture.h"
#include "cinder/gl/BufferObj.h"
#include "cinder/gl/scoped.h"

namespace cinder { namespace gl { namespace nv { namespace multicast {

GLuint Device::getIndex() const
{
	return mIndex;
}

GLbitfield Device::getMask() const
{
	return mMask;
}

Context& Context::instance()
{
	static Context sContext;
	return sContext;
}

bool Context::isEnabled() const
{
	return mLinkedDevices.size() > 1;
}

const std::vector<Device>& Context::getDevices() const
{
	return mLinkedDevices;
}

GLbitfield Context::getMaskAll() const
{
	return mMaskAll;
}

Context::Context() : mMaskAll{ 0 }
{
	int numGPUs;
	glGetIntegerv( GL_MULTICAST_GPUS_NV, &numGPUs );
	for( int i = 0; i < numGPUs; ++i ) {
		mLinkedDevices.emplace_back( Device{ (GLuint)i } );
		mMaskAll |= mLinkedDevices.back().getMask();
	}
}

bool isEnabled()
{
	return Context::instance().isEnabled();
}

const std::vector<Device>& getDevices()
{
	return Context::instance().getDevices();
} 

void enableUploadMask( const Device& device )
{
	glUploadGpuMaskNVX( device.getMask() );
}

void enableUploadMask( GLbitfield mask )
{
	glUploadGpuMaskNVX( mask );
}

void enableUploadMask( const std::vector<Device>& devices )
{
	GLbitfield mask = 0;
	for( auto& device : devices ) {
		mask |= device.getMask();
	}
	glUploadGpuMaskNVX( mask );
}

void disableUploadMask()
{
	glUploadGpuMaskNVX( Context::instance().getMaskAll() );
}

void enableRenderMask( const Device& device )
{
	glRenderGpuMaskNV( device.getMask() );
}

void enableRenderMask( GLbitfield mask )
{
	glRenderGpuMaskNV( mask );
}

void enableRenderMask( const std::vector<Device>& devices )
{
	GLbitfield mask = 0;
	for( auto& device : devices ) {
		mask |= device.getMask();
	}
	glRenderGpuMaskNV( mask );
}

void disableRenderMask()
{
	glRenderGpuMaskNV( Context::instance().getMaskAll() );
}

void signalWaitSync( const Device& source, const Device& target )
{
	glMulticastWaitSyncNV( source.getIndex(), target.getMask() );
}

void signalWaitSync( GLbitfield indexSource, GLbitfield targetMask )
{
	glMulticastWaitSyncNV( indexSource, targetMask );
}

void updateMasked( const std::shared_ptr<BufferObj>& buffer, GLsizeiptr size, const void* data, GLbitfield gpuMask )
{
	glMulticastBufferSubDataNV( gpuMask, buffer->getId(), 0, size, data );
}

void copyTexture( const std::shared_ptr<TextureBase>& source, const std::shared_ptr<TextureBase>& destination, GLuint sourceDeviceIndex, GLbitfield destinationMask )
{
	glMulticastCopyImageSubDataNV( sourceDeviceIndex, destinationMask, source->getId(), GL_TEXTURE_2D, 0, 0, 0, 0, destination->getId(), GL_TEXTURE_2D, 0, 0, 0, 0, source->getWidth(), source->getHeight(), 1 );
}

void scissor( const Device& device, const ivec2& position, const ivec2& size )
{
	const GLint scissors[4] = { position.x, position.y, size.x, size.y };
	glMulticastScissorArrayvNVX( device.getIndex(), 0, 1, scissors );
}

void viewport( const Device& device, const ivec2& position, const ivec2& size )
{
	const GLfloat viewports[4] = { (float)position.x, (float)position.y, (float)size.x, (float)size.y };
	glMulticastViewportArrayvNVX( device.getIndex(), 0, 1, viewports );
}

} } } }
