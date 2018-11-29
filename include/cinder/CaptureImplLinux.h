#pragma once

#include "cinder/Cinder.h"
#include "cinder/Capture.h"

#include <thread>
#include <condition_variable>

namespace cinder {

class CaptureImplLinux {
public:
	class Device : public Capture::Device {
	public:
		Device( const std::string &deviceFile );

		Capture::DeviceIdentifier getUniqueId() const override { return mDeviceFile; }

		bool checkAvailable() const override;
		bool isConnected() const override { return mIsConnected; }

		void setConnected( bool c=true ) { mIsConnected = c; }

	protected:
		Capture::DeviceIdentifier mDeviceFile;
		bool mIsConnected;
	};

public:
	CaptureImplLinux( int32_t width, int32_t height, const Capture::DeviceRef device );
	~CaptureImplLinux();

	void start();
	void stop();

	bool isCapturing();
	bool checkNewFrame() const;

	Surface8uRef getSurface() const;

	int32_t getWidth() const { return mWidth; }
	int32_t getHeight() const { return mHeight; }

	const Capture::DeviceRef getDevice() const { return mDevice; }

	static const std::vector<Capture::DeviceRef>& getDevices( bool forceRefresh = false );

protected:
	Capture::DeviceRef mDevice;
	int32_t mWidth, mHeight;

	int mDeviceFileDescriptor;

	std::thread mThread;
	std::atomic<bool> mRunning;
	std::condition_variable mStopCondition;
	mutable std::atomic<bool> mNewFrameAvailable;

	void update();

protected:
	struct RawFrame {
		void *data;
		size_t length;
	} mRawBuffer[2];

	SurfaceRef mSurfaces[2];
	std::atomic<size_t> mSurfaceReadIndex;

};

}
