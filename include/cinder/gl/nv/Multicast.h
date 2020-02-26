#pragma once

#include "cinder/gl/platform.h"
#include "cinder/Noncopyable.h"

#include <memory>
#include <vector>

namespace cinder {
	namespace gl {

		class BufferObj;
		class TextureBase;

		namespace nv {
			namespace multicast {

				class Device {
				public:
					explicit Device( GLuint index ) : mIndex{ index }, mMask{ 1u << index } { }
					virtual ~Device() { }
					Device( const Device& other ) = default;
					Device( Device&& other ) = default;
					Device& operator=( const Device& other ) = default;
					Device& operator=( Device&& other ) = default;

					GLuint getIndex() const;
					GLbitfield getMask() const;
				protected:
					GLuint			mIndex;
					GLbitfield		mMask;
				};

				class Context : public Noncopyable {
				public:
					static Context&				instance();
					bool						isEnabled() const;
					const std::vector<Device>&	getDevices() const;
					GLbitfield					getMaskAll() const;
				private:
					Context();

					std::vector<Device> mLinkedDevices;
					GLbitfield mMaskAll;
				};

				bool isEnabled();

				const std::vector<Device>& getDevices();

				//! Note: If a function restricted by UploadGpuMaskNVX operates on textures or buffer objects with GPU-shared storage type (as opposed to per-GPU storage ), UPLOAD_GPU_MASK_NVX is ignored.
				void enableUploadMask( const Device& device );
				void enableUploadMask( GLbitfield mask );
				void disableUploadMask();

				void enableRenderMask( const Device& device );
				void enableRenderMask( GLbitfield mask );
				void disableRenderMask();

				//! Signal other device to wait for this GPU to finish its work.
				void signalWaitSync( const Device& source, const Device& target );

				//! Signal all masked devices to wait for this GPU to finish its work.
				void signalWaitSync( GLbitfield indexSource, GLbitfield targetMask );

				void updateMasked( const std::shared_ptr<BufferObj>& buffer, GLsizeiptr size, const void* data, GLbitfield gpuMask );

				void copyTexture( const std::shared_ptr<TextureBase>& source, const std::shared_ptr<TextureBase>& destination, GLuint sourceDeviceIndex, GLbitfield destinationMask );

				void scissor( const Device& device, const glm::ivec2& position, const glm::ivec2& size );
				void viewport( const Device& device, const glm::ivec2& position, const glm::ivec2& size );
			}
		}
	}
}
