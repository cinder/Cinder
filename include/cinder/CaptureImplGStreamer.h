/*
 Copyright (c) 2025, The Cinder Project, All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

	* Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "cinder/Cinder.h"

#if defined( CINDER_LINUX )

	#include "cinder/Capture.h"
	#include "cinder/Surface.h"

	#include <gst/gst.h>
	#include <gst/app/gstappsink.h>
	#include <gst/video/video.h>

	#include <atomic>
	#include <memory>
	#include <mutex>
	#include <thread>
	#include <vector>

namespace cinder {

/** GStreamer-based video capture implementation for Linux.
 *  Automatically constructs optimal GStreamer pipelines based on camera capabilities.
 *  Cameras provide either uncompressed data (YUV/RGB) or compressed data (JPEG/H.264/HEVC).
 *
 *  Uncompressed path: v4l2src → videoconvert → appsink
 *  Compressed path: v4l2src → [decoder] → videoconvert → appsink
 *
 *  The decoder element depends on format: jpegdec, avdec_h264, avdec_h265, or decodebin fallback.
 */
class CaptureImplGStreamer {
  public:
	class Device;

	//! Creates a GStreamer-based video capture with desired width and height. Algorithm will find best matching resolution.
	CaptureImplGStreamer( int32_t width, int32_t height, const Capture::DeviceRef device );

	//! Creates a GStreamer-based video capture with specific Mode from device's getModes() list.
	CaptureImplGStreamer( const Capture::DeviceRef& device, const Capture::Mode& mode );
	~CaptureImplGStreamer();

	//! Starts video capture. Transitions pipeline to PLAYING state.
	void start();

	//! Stops video capture. Transitions pipeline to NULL state. Safe to call multiple times.
	void stop();

	//! Returns true if pipeline is in PLAYING state.
	bool isCapturing();

	//! Returns true if a new frame is available since last call. Thread-safe and resets the new frame flag.
	bool checkNewFrame() const;

	//! Returns actual capture width (may differ from requested).
	int32_t getWidth() const { return mWidth; }

	//! Returns actual capture height (may differ from requested).
	int32_t getHeight() const { return mHeight; }

	//! Returns the most recent video frame as RGB surface, or nullptr if no frame available.
	Surface8uRef getSurface() const;

	//! Returns the device associated with this capture.
	const Capture::DeviceRef getDevice() const { return mDevice; }

	//! Enumerates all available video capture devices. Set forceRefresh=true to detect newly connected/disconnected devices.
	static const std::vector<Capture::DeviceRef>& getDevices( bool forceRefresh = false );

	//! Device wrapper for GStreamer video devices. Represents a physical video capture device with GStreamer integration.
	class Device : public Capture::Device {
	  public:
		Device( GstDevice* device, const std::string& name, const std::string& uniqueId );
		~Device() override;

		//! Returns true if device can be opened for capture. Tests by attempting to create a GStreamer element.
		bool checkAvailable() const override;

		//! Returns true if device is physically connected (device node exists in file system).
		bool isConnected() const override;

		//! Returns unique device identifier (e.g., "/dev/video0").
		Capture::DeviceIdentifier getUniqueId() const override { return mUniqueId; }

		//! Returns underlying GStreamer device handle for advanced usage.
		GstDevice* getGstDevice() const { return mDevice; }

		//! Returns all supported capture modes for this device. Results are cached for performance.
		std::vector<Capture::Mode> getModes() const override;

	  private:
		std::string mUniqueId;
		GstDevice*	mDevice;

		// Mode enumeration caching
		mutable std::vector<Capture::Mode> mCachedModes;
		mutable bool					   mModesQueried;
	};

  private:
	class SurfaceCache;

	// Initialize pipeline based on device capabilities and target dimensions
	bool initializePipeline( int32_t width, int32_t height );

	// Build pipeline from mode
	bool buildPipeline( const Capture::Mode& mode );

	// Clean up GStreamer pipeline resources
	void cleanupPipeline();

	//! Starts background bus monitoring thread. GStreamer uses a "bus" system for error reporting and state changes.
	void startBusWatch();

	//! Stops background bus monitoring thread.
	void stopBusWatch();

	//! GStreamer callback for new frame arrival. Called by GStreamer when a new frame is available.
	static GstFlowReturn onNewSample( GstAppSink* sink, gpointer userData );

	//! GStreamer callback for dynamic pad connections (decodebin only). Connects dynamically created pads to the pipeline.
	static void onDecoderPadAdded( GstElement* decoder, GstPad* pad, gpointer userData );

	//! Processes incoming video frame. Converts GStreamer video frame to Cinder Surface format.
	GstFlowReturn handleSample( GstSample* sample );

	//! Ensures GStreamer is initialized. Thread-safe initialization of GStreamer library.
	static void ensureGStreamerInitialized();


	// Thread synchronization
	mutable std::mutex			  mMutex;
	std::unique_ptr<SurfaceCache> mSurfaceCache;
	mutable Surface8uRef		  mCurrentFrame;
	mutable bool				  mHasNewFrame;

	// GStreamer pipeline elements
	GstElement* mPipeline;	   ///< Main pipeline container
	GstElement* mSource;	   ///< v4l2src element
	GstElement* mVideoConvert; ///< videoconvert element
	GstElement* mCapsFilter;   ///< Output format filter
	GstElement* mAppSink;	   ///< Application sink for frame delivery
	GstBus*		mBus;		   ///< Pipeline message bus

	// Background processing
	std::thread		  mBusWatchThread; ///< Bus monitoring thread
	std::atomic<bool> mRunBusWatch;	   ///< Bus monitoring control flag

	// Resolution tracking
	int32_t				 mRequestedWidth;  ///< Originally requested width
	int32_t				 mRequestedHeight; ///< Originally requested height
	int32_t				 mBestWidth;	   ///< Best available width from device
	int32_t				 mBestHeight;	   ///< Best available height from device
	std::atomic<int32_t> mWidth;		   ///< Current actual width
	std::atomic<int32_t> mHeight;		   ///< Current actual height
	std::atomic<bool>	 mIsCapturing;	   ///< Capture state flag

	Capture::DeviceRef mDevice; ///< Associated device
};

} // namespace cinder

#endif // defined( CINDER_LINUX )