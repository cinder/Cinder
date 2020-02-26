#include "cinder/gl/nv/Multicast.h"

#if defined( CINDER_MSW_DESKTOP ) && ! defined( CINDER_GL_ES )

#include "cinder/gl/Texture.h"
#include "cinder/gl/BufferObj.h"
#include "cinder/gl/scoped.h"
#include "cinder/Noncopyable.h"

namespace cinder { namespace gl { namespace nv { namespace multicast {

class Context : public Noncopyable {
public:
	static Context& instance() {
		static Context sContext;
		return sContext;
	}
	const std::vector<Device>& getDevices() const { return mLinkedDevices; }
	GLbitfield					getMaskAll() const { return mMaskAll; }
private:
	Context() : mMaskAll{ 0 } {
		int numGPUs;
		glGetIntegerv( GL_MULTICAST_GPUS_NV, &numGPUs );
		for( int i = 0; i < numGPUs; ++i ) {
			mLinkedDevices.emplace_back( Device{ (GLuint)i } );
			mMaskAll |= mLinkedDevices.back().getMask();
		}
	}
	std::vector<Device> mLinkedDevices;
	GLbitfield			mMaskAll;
};
} // multicast namespace

const std::vector<multicast::Device>& multicast::getDevices()
{
	return multicast::Context::instance().getDevices();
}
GLbitfield multicast::getMask( const std::vector<Device>& devices )
{
	GLbitfield mask = 0;
	for( const Device& device : devices ) {
		mask |= device.getMask();
	}
	return mask;
}

void multicast::enableUploadMask( GLbitfield mask )
{
	glUploadGpuMaskNVX( mask );
}

void multicast::enableUploadMask( const Device& device )
{
	glUploadGpuMaskNVX( device.getMask() );
}

void multicast::disableUploadMask()
{
	glUploadGpuMaskNVX( multicast::Context::instance().getMaskAll() );
}

void multicast::enableRenderMask( GLbitfield mask )
{
	glRenderGpuMaskNV( mask );
}

void multicast::enableRenderMask( const Device& device )
{
	glRenderGpuMaskNV( device.getMask() );
}

void multicast::disableRenderMask()
{
	glRenderGpuMaskNV( multicast::Context::instance().getMaskAll() );
}

void multicast::signalWaitSync( const multicast::Device& source, const multicast::Device& target )
{
	glMulticastWaitSyncNV( source.getIndex(), target.getMask() );
}

void multicast::signalWaitSync( GLbitfield indexSource, GLbitfield targetMask )
{
	glMulticastWaitSyncNV( indexSource, targetMask );
}

void multicast::bufferSubData( const std::shared_ptr<BufferObj>& buffer, GLsizeiptr size, const void* data, GLbitfield targetMask )
{
	glMulticastBufferSubDataNV( targetMask, buffer->getId(), 0, size, data );
}

void multicast::bufferSubData( const std::shared_ptr<BufferObj>& buffer, GLsizeiptr size, const void* data, const Device& targetDevice )
{
	glMulticastBufferSubDataNV( targetDevice.getMask(), buffer->getId(), 0, size, data );
}

void multicast::copyImageSubData( const std::shared_ptr<TextureBase>& source, const std::shared_ptr<TextureBase>& destination, GLuint sourceDeviceIndex, GLbitfield destinationMask )
{
	glMulticastCopyImageSubDataNV( sourceDeviceIndex, destinationMask, source->getId(), GL_TEXTURE_2D, 0, 0, 0, 0, destination->getId(), GL_TEXTURE_2D, 0, 0, 0, 0, source->getWidth(), source->getHeight(), 1 );
}

void multicast::copyImageSubData( const std::shared_ptr<TextureBase>& source, const std::shared_ptr<TextureBase>& destination, const Device& sourceDevice, const Device& targetDevice )
{
	glMulticastCopyImageSubDataNV( sourceDevice.getIndex(), targetDevice.getMask(), source->getId(), GL_TEXTURE_2D, 0, 0, 0, 0, destination->getId(), GL_TEXTURE_2D, 0, 0, 0, 0, source->getWidth(), source->getHeight(), 1 );
}

void multicast::scissor( GLuint deviceIndex, const glm::ivec2& position, const glm::ivec2& size )
{
	const GLint scissors[4] = { position.x, position.y, size.x, size.y };
	glMulticastScissorArrayvNVX( deviceIndex, 0, 1, scissors );
}

void multicast::scissor( const Device& device, const ivec2& position, const ivec2& size )
{
	const GLint scissors[4] = { position.x, position.y, size.x, size.y };
	glMulticastScissorArrayvNVX( device.getIndex(), 0, 1, scissors );
}

void multicast::viewport( GLuint deviceIndex, const glm::ivec2& position, const glm::ivec2& size )
{
	const GLfloat viewports[4] = { (float)position.x, (float)position.y, (float)size.x, (float)size.y };
	glMulticastViewportArrayvNVX( deviceIndex, 0, 1, viewports );
}

void multicast::viewport( const Device& device, const ivec2& position, const ivec2& size )
{
	const GLfloat viewports[4] = { (float)position.x, (float)position.y, (float)size.x, (float)size.y };
	glMulticastViewportArrayvNVX( device.getIndex(), 0, 1, viewports );
}

} } } // ci::gl::nv namespaces

#endif