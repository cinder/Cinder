#pragma once

#include "cinder/gl/platform.h"

#if defined( CINDER_MSW_DESKTOP ) && ! defined( CINDER_GL_ES )

#include <memory>
#include <vector>

namespace cinder {
	namespace gl {

		class BufferObj;
		class TextureBase;

		namespace nv {
			//! Multicast convenience functions relying on the GL_NV_gpu_multicast, GL_NVX_gpu_multicast2 and WGL_NV_multigpu_context extensions.
			namespace multicast {
				//! Gpu device as used by the multicast extension, which is essentially an index and an associated bitfield mask.
				class CI_API Device final {
				public:
					explicit Device( GLuint index ) : mIndex{ index }, mMask{ 1u << index } { }
					
					Device( const Device& other ) = default;
					Device( Device&& other ) = default;
					Device& operator=( const Device& other ) = default;
					Device& operator=( Device&& other ) = default;

					//! Returns the device index.
					GLuint getIndex() const { return mIndex; }
					//! Returns the device mask bitfield.
					GLbitfield getMask() const { return mMask; }
				protected:
					GLuint			mIndex;
					GLbitfield		mMask;
				};
				//! Returns all "linked" devices visible to the nvidia multicast extension (GL_MULTICAST_GPUS_NV).
				CI_API const std::vector<Device>& getDevices();
				//! Returns the bitfield mask for all listed devices.
				CI_API GLbitfield getMask( const std::vector<Device>& devices );

				//! Restricts buffer object data uploads to the specified device mask.
				//! Note: If a function restricted by UploadGpuMaskNVX operates on textures or buffer objects
				//! with GPU-shared storage type (as opposed to per-GPU storage ), UPLOAD_GPU_MASK_NVX is ignored.
				CI_API void enableUploadMask( GLbitfield mask );
				//! Restricts buffer object data uploads to the specified device.
				CI_API void enableUploadMask( const Device& device );
				//! Disable buffer object data uploads (device) restrictions.
				CI_API void disableUploadMask();

				//! Restricts render commands to the specified device mask.
				CI_API void enableRenderMask( GLbitfield mask );
				//! Restricts render commands to the specified device.
				CI_API void enableRenderMask( const Device& device );
				//! Disable render commands (device) restrictions.
				CI_API void disableRenderMask();

				//! Signal all masked devices to wait for this GPU to finish its work.
				CI_API void signalWaitSync( GLbitfield indexSource, GLbitfield targetMask );
				//! Signal other device to wait for this GPU to finish its work. (For multiple target devices, use the bitfield method.)
				CI_API void signalWaitSync( const Device& source, const Device& target );

				//! Buffer subdata only on the target (masked) devices. 
				CI_API void bufferSubData( const std::shared_ptr<BufferObj>& buffer, GLsizeiptr size, const void* data, GLbitfield targetMask );
				//! Buffer subdata only on the target device. (For multiple target devices, use the bitfield method.)
				CI_API void bufferSubData( const std::shared_ptr<BufferObj>& buffer, GLsizeiptr size, const void* data, const Device& targetDevice );
				
				//! Copy image subdata from the source device index onto the target (masked) devices.
				CI_API void copyImageSubData( const std::shared_ptr<TextureBase>& source, const std::shared_ptr<TextureBase>& destination, GLuint sourceDeviceIndex, GLbitfield destinationMask );
				//! Copy image subdata from the source device index onto the target device. (For multiple target devices, use the bitfield method.)
				CI_API void copyImageSubData( const std::shared_ptr<TextureBase>& source, const std::shared_ptr<TextureBase>& destination, const Device& sourceDevice, const Device& targetDevice );

				//! Specify a multicast scissor per device index.
				CI_API void scissor( GLuint deviceIndex, const glm::ivec2& position, const glm::ivec2& size );
				//! Specify a multicast scissor per device.
				CI_API void scissor( const Device& device, const glm::ivec2& position, const glm::ivec2& size );
				
				//! Specify a multicast viewport per device index.
				CI_API void viewport( GLuint deviceIndex, const glm::ivec2& position, const glm::ivec2& size );
				//! Specify a multicast viewport per device.
				CI_API void viewport( const Device& device, const glm::ivec2& position, const glm::ivec2& size );
			}
		}
	}
}

#endif