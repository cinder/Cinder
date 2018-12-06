#include "cinder/CaptureImplLinux.h"

#include <linux/videodev2.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <libv4l2.h>

#include <mutex>

namespace cinder {

CaptureImplLinux::Device::Device( const std::string &deviceFile ):
mDeviceFile( deviceFile ), mIsConnected( false )
{
	int mDeviceFileDescriptor = v4l2_open( deviceFile.c_str(), O_RDWR );
	if ( mDeviceFileDescriptor >= 0 ) {
		struct v4l2_capability deviceInfo;
		int error = v4l2_ioctl( mDeviceFileDescriptor, VIDIOC_QUERYCAP, &deviceInfo );
		v4l2_close( mDeviceFileDescriptor );

		if ( ! error ) {
			mName = std::string( reinterpret_cast<const char*>( &deviceInfo.card[0] ) );
		}
	}
}


bool CaptureImplLinux::Device::checkAvailable() const
{
	return ! mName.empty();
}

CaptureImplLinux::CaptureImplLinux( int32_t width, int32_t height, const Capture::DeviceRef device ):
mDeviceFileDescriptor( -1 ),
mRunning( false ), mNewFrameAvailable( false ),
mDevice( device )
{
	// choose first
	if ( !mDevice ) {
		auto &devices = getDevices();
		if ( devices.empty() ) {
			throw CaptureExcInitFail();
		} else {
			mDevice = getDevices()[0];
		}
	}

	const char *name = mDevice->getUniqueId().c_str();

	// implicty set to blocking io
	mDeviceFileDescriptor = v4l2_open( name , O_RDWR );
	if ( mDeviceFileDescriptor < 0 ) {
		throw CaptureExcInitFail();
	}

	struct v4l2_format format{};
	std::memset( &format, 0, sizeof( v4l2_format ) );

	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	format.fmt.pix.width = static_cast<uint32_t>( width );
	format.fmt.pix.height = static_cast<uint32_t>( height );
	format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
	format.fmt.pix.field = V4L2_FIELD_NONE;

	v4l2_ioctl( mDeviceFileDescriptor, VIDIOC_S_FMT, &format );

	if ( format.fmt.pix.pixelformat != V4L2_PIX_FMT_RGB24 ) {
		throw CaptureExcInvalidChannelOrder();
	}

	mWidth = format.fmt.pix.width;
	mHeight = format.fmt.pix.height;

	struct v4l2_requestbuffers requestBuffers{};
	requestBuffers.count = 2;
	requestBuffers.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	requestBuffers.memory = V4L2_MEMORY_MMAP;

	v4l2_ioctl( mDeviceFileDescriptor, VIDIOC_REQBUFS, &requestBuffers );

	struct v4l2_buffer buffer{};
	buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buffer.memory = V4L2_MEMORY_MMAP;
	buffer.index = 0;

	for ( uint32_t i = 0; i < 2; i++ ) {
		buffer.index = i;

		v4l2_ioctl( mDeviceFileDescriptor, VIDIOC_QUERYBUF, &buffer );
		mRawBuffer[i].length = buffer.length;
		mRawBuffer[i].data = v4l2_mmap( NULL, buffer.length,
				PROT_READ | PROT_WRITE, MAP_SHARED, mDeviceFileDescriptor, buffer.m.offset );

		if ( mRawBuffer[i].data == MAP_FAILED ) {
			throw CaptureExcInitFail();

		} else {
			mSurfaces[i] = Surface::create( reinterpret_cast<unsigned  char*>( mRawBuffer[i].data ),
											mWidth, mHeight, mWidth * 3,
											SurfaceChannelOrder( SurfaceChannelOrder::RGB ) );
		}

		v4l2_ioctl( mDeviceFileDescriptor, VIDIOC_QBUF, &buffer );
	}

	dynamic_cast<CaptureImplLinux::Device*>( mDevice.get() )->setConnected();
}

CaptureImplLinux::~CaptureImplLinux()
{
	stop();
	
	if ( mDeviceFileDescriptor >= 0 ) {
		for ( size_t i = 0; i < 2; i++ ) {
			v4l2_munmap( mRawBuffer[i].data, mRawBuffer[i].length );
		}
		v4l2_close( mDeviceFileDescriptor );
	}

	dynamic_cast<CaptureImplLinux::Device*>( mDevice.get() )->setConnected( false );
}

void CaptureImplLinux::start()
{
	enum v4l2_buf_type bufferType = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	v4l2_ioctl( mDeviceFileDescriptor, VIDIOC_STREAMON, &bufferType );

	mRunning.store( true );
	mThread = std::thread( &CaptureImplLinux::update, this );
}

void CaptureImplLinux::stop()
{
	if ( mRunning ) {
		mRunning.store( false );
		mThread.join();

		enum v4l2_buf_type bufferType = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		v4l2_ioctl( mDeviceFileDescriptor, VIDIOC_STREAMOFF, &bufferType );
	}
}

void CaptureImplLinux::update()
{
	struct v4l2_buffer buffer{};
	buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buffer.memory = V4L2_MEMORY_MMAP;

	while ( mRunning ) {
		v4l2_ioctl( mDeviceFileDescriptor, VIDIOC_DQBUF, &buffer );
		mSurfaceReadIndex.store( buffer.index );
		mNewFrameAvailable.store( true );
		v4l2_ioctl( mDeviceFileDescriptor, VIDIOC_QBUF, &buffer );
	}

}

bool CaptureImplLinux::isCapturing()
{
	return mRunning;
}

bool CaptureImplLinux::checkNewFrame() const
{
	return mNewFrameAvailable;
}

Surface8uRef CaptureImplLinux::getSurface() const
{
	mNewFrameAvailable.store( false );
	return Surface::create( *mSurfaces[mSurfaceReadIndex] );
}

const std::vector<Capture::DeviceRef>& CaptureImplLinux::getDevices( bool forceRefresh  )
{
	static std::vector<Capture::DeviceRef>	devices;

	if ( forceRefresh || devices.empty() ) {
		devices.clear();

		for ( fs::directory_iterator it( "/dev" ); it != fs::directory_iterator(); ++it ) {
			auto deviceFile = it->path().string();
			if ( deviceFile.find( "video" ) != std::string::npos ) {
				devices.emplace_back( std::make_shared<CaptureImplLinux::Device>( deviceFile ) );
			}
		}

	}

	return devices;
}

}
