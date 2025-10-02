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

#include "cinder/CaptureImplGStreamer.h"

#if defined( CINDER_LINUX )

	#include "cinder/Capture.h"
	#include "cinder/Log.h"

	#include <algorithm>
	#include <cmath>
	#include <cstring>
	#include "cinder/Filesystem.h"
	#include <iostream>
	#include <iterator>
	#include <limits>

using namespace std;

namespace cinder {

namespace {

static const SurfaceChannelOrder kCaptureChannelOrder = SurfaceChannelOrder::RGB;

// Custom deleters as inline functions
inline void gstElementDeleter( GstElement* elem )
{
	if( elem )
		gst_object_unref( GST_OBJECT( elem ) );
}
inline void gstCapsDeleter( GstCaps* caps )
{
	if( caps )
		gst_caps_unref( caps );
}
inline void gstSampleDeleter( GstSample* sample )
{
	if( sample )
		gst_sample_unref( sample );
}

// Type aliases for smart pointers using function pointer deleters
using GstElementPtr = std::unique_ptr<GstElement, decltype( &gstElementDeleter )>;
using GstCapsPtr = std::unique_ptr<GstCaps, decltype( &gstCapsDeleter )>;
using GstSamplePtr = std::unique_ptr<GstSample, decltype( &gstSampleDeleter )>;

static string deriveUniqueId( GstDevice* device )
{
	string identifier;
	if( ! device )
		return identifier;

	GstStructure* props = gst_device_get_properties( device );
	if( props ) {
		const gchar* path = gst_structure_get_string( props, "device.path" );
		if( path )
			identifier = path;
		else {
			const gchar* node = gst_structure_get_string( props, "device.node" );
			if( node )
				identifier = node;
		}
		gst_structure_free( props );
	}

	if( identifier.empty() ) {
		identifier = gst_device_get_display_name( device );
	}

	return identifier;
}

static Capture::Mode::PixelFormat gstFormatToPixelFormat( const gchar* format )
{
	if( ! format )
		return Capture::Mode::PixelFormat::Unknown;

	if( g_str_equal( format, "RGB" ) )
		return Capture::Mode::PixelFormat::RGB24;
	else if( g_str_equal( format, "BGR" ) )
		return Capture::Mode::PixelFormat::BGR24;
	else if( g_str_equal( format, "RGBA" ) )
		return Capture::Mode::PixelFormat::ARGB32;
	else if( g_str_equal( format, "BGRA" ) )
		return Capture::Mode::PixelFormat::BGRA32;
	else if( g_str_equal( format, "I420" ) )
		return Capture::Mode::PixelFormat::I420;
	else if( g_str_equal( format, "YV12" ) )
		return Capture::Mode::PixelFormat::YV12;
	else if( g_str_equal( format, "NV12" ) )
		return Capture::Mode::PixelFormat::NV12;
	else if( g_str_equal( format, "YUY2" ) )
		return Capture::Mode::PixelFormat::YUY2;
	else if( g_str_equal( format, "UYVY" ) )
		return Capture::Mode::PixelFormat::UYVY;

	return Capture::Mode::PixelFormat::Unknown;
}

// Pipeline configuration types (used by buildPipeline)
enum class PipelineType {
	RAW_DIRECT,		   // v4l2src -> videoconvert -> appsink (for video/x-raw)
	JPEG_DECODE,	   // v4l2src -> jpegdec -> videoconvert -> appsink (for image/jpeg)
	H264_DECODE,	   // v4l2src -> h264parse -> avdec_h264 -> videoconvert -> appsink (for video/x-h264)
	HEVC_DECODE,	   // v4l2src -> h265parse -> avdec_h265 -> videoconvert -> appsink (for video/x-h265)
	DECODEBIN_FALLBACK // v4l2src -> decodebin -> videoconvert -> appsink (for unknown compressed formats)
};

// Maps codec type to pipeline type and priority (higher = more efficient)
std::pair<PipelineType, int> codecToPipeline( Capture::Mode::Codec codec )
{
	switch( codec ) {
		case Capture::Mode::Codec::Uncompressed:
			return { PipelineType::RAW_DIRECT, 4 };
		case Capture::Mode::Codec::JPEG:
			return { PipelineType::JPEG_DECODE, 3 };
		case Capture::Mode::Codec::H264:
			return { PipelineType::H264_DECODE, 2 };
		case Capture::Mode::Codec::HEVC:
			return { PipelineType::HEVC_DECODE, 1 };
		default:
			return { PipelineType::DECODEBIN_FALLBACK, 0 };
	}
}

} // namespace

// CaptureImplGStreamer::SurfaceCache

class CaptureImplGStreamer::SurfaceCache {
  public:
	SurfaceCache( int32_t width, int32_t height, SurfaceChannelOrder sco, int numSurfaces )
		: mWidth( width )
		, mHeight( height )
		, mSCO( sco )
	{
		mSurfaces.resize( numSurfaces );
		allocateSurfaces();
	}

	Surface8uRef getNewSurface()
	{
		auto it = find_if( mSurfaces.begin(), mSurfaces.end(), []( const Surface8uRef& s ) { return s && s.use_count() == 1; } );
		if( it == mSurfaces.end() ) {
			mSurfaces.push_back( Surface8u::create( mWidth, mHeight, mSCO.hasAlpha(), mSCO ) );
			it = prev( mSurfaces.end() );
		}
		return *it;
	}

	void resize( int32_t width, int32_t height )
	{
		if( ( width == mWidth ) && ( height == mHeight ) )
			return;
		mWidth = width;
		mHeight = height;
		allocateSurfaces();
	}

  private:
	void allocateSurfaces()
	{
		for( auto& surface : mSurfaces ) {
			surface = Surface8u::create( mWidth, mHeight, mSCO.hasAlpha(), mSCO );
		}
	}

	vector<Surface8uRef> mSurfaces;
	int32_t				 mWidth = 0;
	int32_t				 mHeight = 0;
	SurfaceChannelOrder	 mSCO;
};

// static
void CaptureImplGStreamer::ensureGStreamerInitialized()
{
	static std::once_flag flag;
	std::call_once( flag, []() {
		if( ! gst_is_initialized() ) {
			GError* err = nullptr;
			if( ! gst_init_check( nullptr, nullptr, &err ) ) {
				std::string message = "GStreamer initialization failed";
				if( err && err->message )
					message += ": " + std::string( err->message );
				if( err )
					g_error_free( err );
				throw CaptureExcInitFail( message );
			}
		}
	} );
}

CaptureImplGStreamer::Device::Device( GstDevice* device, const std::string& name, const std::string& uniqueId )
	: mUniqueId( uniqueId )
	, mDevice( device )
	, mModesQueried( false )
{
	mName = name;
	if( mDevice )
		gst_object_ref( mDevice );
}

CaptureImplGStreamer::Device::~Device()
{
	if( mDevice ) {
		gst_object_unref( mDevice );
		mDevice = nullptr;
	}
}

bool CaptureImplGStreamer::Device::checkAvailable() const
{
	if( ! mDevice )
		return false;

	GstElement* probe = gst_device_create_element( mDevice, nullptr );
	if( ! probe )
		return false;
	gst_object_unref( probe );
	return true;
}

bool CaptureImplGStreamer::Device::isConnected() const
{
	if( ! mDevice )
		return false;

	string		  path;
	GstStructure* props = gst_device_get_properties( mDevice );
	if( props ) {
		const gchar* devicePath = gst_structure_get_string( props, "device.path" );
		if( devicePath )
			path = devicePath;
		gst_structure_free( props );
	}

	if( path.empty() )
		return true;

	return ci::fs::exists( path );
}

// Enumerates all supported camera modes by parsing GStreamer caps (capabilities).
// Caps describe supported resolutions, framerates, and formats (YUV, JPEG, H.264, etc).
// Results are cached since device capabilities don't change during runtime.
std::vector<Capture::Mode> CaptureImplGStreamer::Device::getModes() const
{
	if( mModesQueried )
		return mCachedModes;

	mCachedModes.clear();

	if( ! mDevice ) {
		mModesQueried = true;
		return mCachedModes;
	}

	// Get device capabilities
	GstCapsPtr caps( gst_device_get_caps( mDevice ), gstCapsDeleter );
	if( ! caps ) {
		CI_LOG_W( "Failed to get capabilities for device: " << mName );
		mModesQueried = true;
		return mCachedModes;
	}

	// Parse each capability structure
	guint numStructures = gst_caps_get_size( caps.get() );

	for( guint i = 0; i < numStructures; ++i ) {
		GstStructure* structure = gst_caps_get_structure( caps.get(), i );
		if( ! structure )
			continue;

		const gchar* mediaType = gst_structure_get_name( structure );
		if( ! mediaType )
			continue;

		// Parse resolution (could be fixed values or ranges)
		std::vector<std::pair<gint, gint>> resolutions;
		const GValue*					   widthValue = gst_structure_get_value( structure, "width" );
		const GValue*					   heightValue = gst_structure_get_value( structure, "height" );

		if( ! widthValue || ! heightValue )
			continue;

		if( G_VALUE_TYPE( widthValue ) == G_TYPE_INT && G_VALUE_TYPE( heightValue ) == G_TYPE_INT ) {
			// Fixed resolution (typical for consumer webcams)
			gint width = g_value_get_int( widthValue );
			gint height = g_value_get_int( heightValue );
			resolutions.emplace_back( width, height );
		}
		else if( GST_VALUE_HOLDS_INT_RANGE( widthValue ) && GST_VALUE_HOLDS_INT_RANGE( heightValue ) ) {
			// Resolution range (rare - typically industrial cameras)
			gint minWidth = gst_value_get_int_range_min( widthValue );
			gint maxWidth = gst_value_get_int_range_max( widthValue );
			gint minHeight = gst_value_get_int_range_min( heightValue );
			gint maxHeight = gst_value_get_int_range_max( heightValue );

			CI_LOG_W( "Device " << mName << " reports resolution range " << minWidth << "x" << minHeight << " to " << maxWidth << "x" << maxHeight << " (using min/max only)" );

			resolutions.emplace_back( minWidth, minHeight );
			if( maxWidth != minWidth || maxHeight != minHeight )
				resolutions.emplace_back( maxWidth, maxHeight );
		}

		// If we couldn't parse resolutions, skip this structure
		if( resolutions.empty() )
			continue;

		// Parse framerate (could be a range or fixed value)
		const GValue* framerateValue = gst_structure_get_value( structure, "framerate" );
		if( ! framerateValue )
			continue;

		// Handle different framerate value types
		std::vector<std::pair<gint, gint>> framerates;

		if( GST_VALUE_HOLDS_FRACTION( framerateValue ) ) {
			// Single framerate (typical)
			gint num = gst_value_get_fraction_numerator( framerateValue );
			gint denom = gst_value_get_fraction_denominator( framerateValue );
			framerates.emplace_back( num, denom );
		}
		else if( GST_VALUE_HOLDS_FRACTION_RANGE( framerateValue ) ) {
			// Framerate range (rare - typically industrial cameras)
			const GValue* minValue = gst_value_get_fraction_range_min( framerateValue );
			const GValue* maxValue = gst_value_get_fraction_range_max( framerateValue );

			gint   minNum = gst_value_get_fraction_numerator( minValue );
			gint   minDenom = gst_value_get_fraction_denominator( minValue );
			gint   maxNum = gst_value_get_fraction_numerator( maxValue );
			gint   maxDenom = gst_value_get_fraction_denominator( maxValue );
			double minFps = (double)minNum / minDenom;
			double maxFps = (double)maxNum / maxDenom;

			CI_LOG_W( "Device " << mName << " reports framerate range " << minFps << " to " << maxFps << " fps (using min/max only)" );

			framerates.emplace_back( minNum, minDenom );
			if( minNum != maxNum || minDenom != maxDenom )
				framerates.emplace_back( maxNum, maxDenom );
		}
		else if( GST_VALUE_HOLDS_LIST( framerateValue ) ) {
			// List of framerates
			guint numRates = gst_value_list_get_size( framerateValue );
			for( guint j = 0; j < numRates; ++j ) {
				const GValue* rateValue = gst_value_list_get_value( framerateValue, j );
				if( GST_VALUE_HOLDS_FRACTION( rateValue ) ) {
					gint num = gst_value_get_fraction_numerator( rateValue );
					gint denom = gst_value_get_fraction_denominator( rateValue );
					framerates.emplace_back( num, denom );
				}
			}
		}

		// Parse codec and pixel format variations
		std::vector<std::pair<Capture::Mode::Codec, Capture::Mode::PixelFormat>> formatCombos;

		if( g_str_equal( mediaType, "video/x-raw" ) ) {
			// Parse pixel format(s) - could be a single value, list, or string
			const GValue* formatValue = gst_structure_get_value( structure, "format" );

			std::vector<Capture::Mode::PixelFormat> pixelFormats;

			if( formatValue ) {
				if( G_VALUE_TYPE( formatValue ) == G_TYPE_STRING ) {
					// Single format
					const gchar*				format = g_value_get_string( formatValue );
					Capture::Mode::PixelFormat pf = gstFormatToPixelFormat( format );
					if( pf != Capture::Mode::PixelFormat::Unknown )
						pixelFormats.push_back( pf );
				}
				else if( GST_VALUE_HOLDS_LIST( formatValue ) ) {
					// List of formats
					guint numFormats = gst_value_list_get_size( formatValue );
					for( guint j = 0; j < numFormats; ++j ) {
						const GValue* fmtValue = gst_value_list_get_value( formatValue, j );
						if( G_VALUE_TYPE( fmtValue ) == G_TYPE_STRING ) {
							const gchar*				format = g_value_get_string( fmtValue );
							Capture::Mode::PixelFormat pf = gstFormatToPixelFormat( format );
							if( pf != Capture::Mode::PixelFormat::Unknown )
								pixelFormats.push_back( pf );
						}
					}
				}
			}

			// If no formats found, add a default
			if( pixelFormats.empty() ) {
				pixelFormats.push_back( Capture::Mode::PixelFormat::Unknown );
			}

			// Create combos for uncompressed formats
			for( const auto& pf : pixelFormats ) {
				formatCombos.emplace_back( Capture::Mode::Codec::Uncompressed, pf );
			}
		}
		else if( g_str_equal( mediaType, "image/jpeg" ) ) {
			formatCombos.emplace_back( Capture::Mode::Codec::JPEG, Capture::Mode::PixelFormat::YUV420P );
		}
		else if( g_str_equal( mediaType, "video/x-h264" ) ) {
			formatCombos.emplace_back( Capture::Mode::Codec::H264, Capture::Mode::PixelFormat::YUV420P );
		}
		else if( g_str_equal( mediaType, "video/x-h265" ) || g_str_equal( mediaType, "video/x-hevc" ) ) {
			formatCombos.emplace_back( Capture::Mode::Codec::HEVC, Capture::Mode::PixelFormat::YUV420P );
		}
		else {
			// Unknown format
			formatCombos.emplace_back( Capture::Mode::Codec::Unknown, Capture::Mode::PixelFormat::Unknown );
		}

		// Create Mode objects for each combination of resolution, framerate, and format
		for( const auto& res : resolutions ) {
			for( const auto& rate : framerates ) {
				for( const auto& formatCombo : formatCombos ) {
					if( rate.first > 0 && rate.second > 0 ) {
						// GStreamer gives us framerate as num/denom (e.g., 30/1 for 30fps)
						// MediaTime expects frame duration: seconds per frame
						// So for 30fps: MediaTime should be (1, 30) meaning 1/30 seconds per frame
						// GStreamer gives 30/1, so we need to swap: MediaTime(denom, num)
						MediaTime frameRate( rate.second, rate.first ); // MediaTime(frame_duration_num, frame_duration_denom)

						std::string description = std::string( mediaType );

						// Create mode with single frame rate
						Capture::Mode mode( res.first, res.second, frameRate, formatCombo.first, formatCombo.second, description );

						// Store platform-specific data for exact pipeline construction
						// For raw formats, include the format in the caps
						if( g_str_equal( mediaType, "video/x-raw" ) && formatCombo.second != Capture::Mode::PixelFormat::Unknown ) {
							// Map PixelFormat back to GStreamer format string
							const char* gstFormat = nullptr;
							switch( formatCombo.second ) {
								case Capture::Mode::PixelFormat::RGB24:
									gstFormat = "RGB";
									break;
								case Capture::Mode::PixelFormat::BGR24:
									gstFormat = "BGR";
									break;
								case Capture::Mode::PixelFormat::ARGB32:
									gstFormat = "RGBA";
									break;
								case Capture::Mode::PixelFormat::BGRA32:
									gstFormat = "BGRA";
									break;
								case Capture::Mode::PixelFormat::I420:
									gstFormat = "I420";
									break;
								case Capture::Mode::PixelFormat::YV12:
									gstFormat = "YV12";
									break;
								case Capture::Mode::PixelFormat::NV12:
									gstFormat = "NV12";
									break;
								case Capture::Mode::PixelFormat::YUY2:
									gstFormat = "YUY2";
									break;
								case Capture::Mode::PixelFormat::UYVY:
									gstFormat = "UYVY";
									break;
								default:
									gstFormat = nullptr;
									break;
							}

							if( gstFormat ) {
								GstCapsPtr caps(
									gst_caps_new_simple( mediaType, "width", G_TYPE_INT, res.first, "height", G_TYPE_INT, res.second, "framerate", GST_TYPE_FRACTION, rate.first, rate.second, "format", G_TYPE_STRING, gstFormat, nullptr ),
									gstCapsDeleter );

								gchar* capsStr = gst_caps_to_string( caps.get() );
								if( capsStr ) {
									mode.setPlatformData( std::string( capsStr ) );
									g_free( capsStr );
								}
							}
						}
						else {
							// For compressed formats, don't include format
							GstCapsPtr caps( gst_caps_new_simple( mediaType, "width", G_TYPE_INT, res.first, "height", G_TYPE_INT, res.second, "framerate", GST_TYPE_FRACTION, rate.first, rate.second, nullptr ), gstCapsDeleter );

							gchar* capsStr = gst_caps_to_string( caps.get() );
							if( capsStr ) {
								mode.setPlatformData( std::string( capsStr ) );
								g_free( capsStr );
							}
						}

						mCachedModes.push_back( mode );
					}
				}
			}
		}
	}

	// Sort modes by resolution, then framerate
	std::sort( mCachedModes.begin(), mCachedModes.end() );

	mModesQueried = true;
	return mCachedModes;
}

CaptureImplGStreamer::CaptureImplGStreamer( int32_t width, int32_t height, const Capture::DeviceRef device )
	: mSurfaceCache()
	, mCurrentFrame()
	, mHasNewFrame( false )
	, mPipeline( nullptr )
	, mSource( nullptr )
	, mVideoConvert( nullptr )
	, mCapsFilter( nullptr )
	, mAppSink( nullptr )
	, mBus( nullptr )
	, mRunBusWatch( false )
	, mRequestedWidth( width )
	, mRequestedHeight( height )
	, mBestWidth( width )
	, mBestHeight( height )
	, mWidth( width )
	, mHeight( height )
	, mIsCapturing( false )
	, mDevice( device )
{
	ensureGStreamerInitialized();

	// Find the best matching resolution for the requested size
	Capture::DeviceRef selectedDevice = device;
	if( ! selectedDevice ) {
		const auto& devices = getDevices();
		if( ! devices.empty() )
			selectedDevice = devices.front();
	}

	mBestWidth = width;
	mBestHeight = height;

	if( ! initializePipeline( mBestWidth, mBestHeight ) )
		throw CaptureExcInitFail( "Failed to initialize capture pipeline" );
}

CaptureImplGStreamer::CaptureImplGStreamer( const Capture::DeviceRef& device, const Capture::Mode& mode )
	: mSurfaceCache()
	, mCurrentFrame()
	, mHasNewFrame( false )
	, mPipeline( nullptr )
	, mSource( nullptr )
	, mVideoConvert( nullptr )
	, mCapsFilter( nullptr )
	, mAppSink( nullptr )
	, mBus( nullptr )
	, mRunBusWatch( false )
	, mRequestedWidth( mode.getWidth() )
	, mRequestedHeight( mode.getHeight() )
	, mBestWidth( mode.getWidth() )
	, mBestHeight( mode.getHeight() )
	, mWidth( mode.getWidth() )
	, mHeight( mode.getHeight() )
	, mIsCapturing( false )
	, mDevice( device )
{
	ensureGStreamerInitialized();

	if( ! device )
		throw CaptureExcInitFail( "Device cannot be null for Mode-based capture" );

	// If mode has platformData (caps string from enumeration), use it directly
	const std::string& platformData = mode.getPlatformData();
	if( ! platformData.empty() ) {
		// Use Mode dimensions
		mBestWidth = mode.getWidth();
		mBestHeight = mode.getHeight();
		mWidth = mode.getWidth();
		mHeight = mode.getHeight();

		// Build pipeline with exact caps
		if( ! buildPipeline( mode ) )
			throw CaptureExcInitFail( "Failed to initialize capture pipeline with specified mode" );
	}
	else {
		// Fallback: no platformData, use width/height-based selection
		if( ! initializePipeline( mBestWidth, mBestHeight ) )
			throw CaptureExcInitFail( "Failed to initialize capture pipeline" );
	}
}

CaptureImplGStreamer::~CaptureImplGStreamer()
{
	stop();
	cleanupPipeline();
}

void CaptureImplGStreamer::start()
{
	if( mIsCapturing )
		return;


	if( ! mPipeline && ! initializePipeline( mRequestedWidth, mRequestedHeight ) )
		throw CaptureExcInitFail( "Failed to initialize capture pipeline" );

	GstStateChangeReturn result = gst_element_set_state( mPipeline, GST_STATE_PLAYING );
	if( result == GST_STATE_CHANGE_FAILURE ) {
		CI_LOG_E( "Failed to set pipeline to PLAYING state" );

		// Get error details
		GstBus*		bus = gst_element_get_bus( mPipeline );
		GstMessage* msg = gst_bus_pop_filtered( bus, GST_MESSAGE_ERROR );
		if( msg ) {
			GError* err = nullptr;
			gchar*	dbg = nullptr;
			gst_message_parse_error( msg, &err, &dbg );
			if( err ) {
				CI_LOG_E( "Pipeline error: " << err->message );
				g_error_free( err );
			}
			if( dbg ) {
				CI_LOG_E( "Debug info: " << dbg );
				g_free( dbg );
			}
			gst_message_unref( msg );
		}
		gst_object_unref( bus );

		throw CaptureExcInitFail( "Failed to start GStreamer pipeline" );
	}

	mIsCapturing = true;
	{
		lock_guard<mutex> lock( mMutex );
		mHasNewFrame = false;
	}
	startBusWatch();
}

void CaptureImplGStreamer::stop()
{
	if( ! mIsCapturing )
		return;

	mIsCapturing = false;
	stopBusWatch();

	// Fully cleanup the pipeline to ensure v4l2 device is released
	// This is critical for rapid mode switching - we must wait for the device
	// to be completely released before creating a new pipeline
	cleanupPipeline();
}

bool CaptureImplGStreamer::isCapturing()
{
	// Check both our flag and the actual pipeline state
	if( ! mIsCapturing )
		return false;

	// Verify pipeline is in a valid state
	if( mPipeline ) {
		GstState			 state;
		GstStateChangeReturn ret = gst_element_get_state( mPipeline, &state, nullptr, 0 );

		// Consider PLAYING and PAUSED as valid capturing states
		// PAUSED can be normal during startup or when seeking
		bool pipelineValid = ( ret != GST_STATE_CHANGE_FAILURE ) && ( state == GST_STATE_PLAYING || state == GST_STATE_PAUSED );

		// Only update flag for actual failures
		if( mIsCapturing && ret == GST_STATE_CHANGE_FAILURE ) {
			CI_LOG_W( "Pipeline failure detected - marking capture as stopped" );
			mIsCapturing = false;
		}

		return mIsCapturing && pipelineValid;
	}

	return mIsCapturing;
}

bool CaptureImplGStreamer::checkNewFrame() const
{
	lock_guard<mutex> lock( mMutex );
	bool			  hasNewFrame = mHasNewFrame;
	mHasNewFrame = false;
	return hasNewFrame;
}

Surface8uRef CaptureImplGStreamer::getSurface() const
{
	lock_guard<mutex> lock( mMutex );
	return mCurrentFrame;
}

namespace {

// Finds closest matching mode from available options. Prioritizes: 1) smallest area difference, 2) highest framerate, 3) most efficient pipeline.
Capture::Mode findBestMode( const std::vector<Capture::Mode>& modes, int32_t targetWidth, int32_t targetHeight )
{
	if( modes.empty() )
		return Capture::Mode();

	int32_t targetArea = targetWidth * targetHeight;

	// Best match tracking: prioritize area, then framerate, then pipeline type
	const Capture::Mode* bestMode = nullptr;
	int32_t bestAreaDiff = INT32_MAX;
	double bestFrameRate = 0.0;
	int bestPipelinePriority = -1;

	for( const auto& mode : modes ) {
		int32_t currentArea = mode.getWidth() * mode.getHeight();
		int32_t areaDiff = std::abs( currentArea - targetArea );
		double frameRate = mode.getFrameRateFloat();
		int pipelinePriority = codecToPipeline( mode.getCodec() ).second;

		// Compare: area first, then framerate, then pipeline priority
		bool isBetter = false;
		if( areaDiff < bestAreaDiff ) {
			isBetter = true;
		}
		else if( areaDiff == bestAreaDiff ) {
			if( frameRate > bestFrameRate ) {
				isBetter = true;
			}
			else if( frameRate == bestFrameRate && pipelinePriority > bestPipelinePriority ) {
				isBetter = true;
			}
		}

		if( isBetter ) {
			bestMode = &mode;
			bestAreaDiff = areaDiff;
			bestFrameRate = frameRate;
			bestPipelinePriority = pipelinePriority;
		}
	}

	return bestMode ? *bestMode : Capture::Mode();
}

} // anonymous namespace

// Selects best camera mode matching target resolution and initializes GStreamer pipeline.
// Prioritizes closest resolution, then highest framerate, then most efficient pipeline type.
bool CaptureImplGStreamer::initializePipeline( int32_t width, int32_t height )
{
	if( mPipeline )
		return true;

	CI_ASSERT( mDevice );

	// Find best matching mode from device capabilities
	std::vector<Capture::Mode> modes = mDevice->getModes();
	Capture::Mode bestMode = findBestMode( modes, width, height );

	return buildPipeline( bestMode );
}

// Constructs GStreamer processing pipeline from camera → decoder (if needed) → color conversion → output.
// Pipeline structure varies by codec: raw video is direct, JPEG/H.264/HEVC add appropriate decoder elements.
bool CaptureImplGStreamer::buildPipeline( const Capture::Mode& mode )
{
	// Create v4l2src
	GstElementPtr source( gst_element_factory_make( "v4l2src", "camera-source" ), gstElementDeleter );
	if( ! source ) {
		throw CaptureExcInitFail( "Failed to create GStreamer source element" );
	}

	// Set device path
	std::string devicePath;
	if( mDevice ) {
		auto gstDevice = std::dynamic_pointer_cast<CaptureImplGStreamer::Device>( mDevice );
		if( gstDevice && gstDevice->getGstDevice() ) {
			GstStructure* props = gst_device_get_properties( gstDevice->getGstDevice() );
			if( props ) {
				// Try different property names for device path
				const gchar* device_path = gst_structure_get_string( props, "api.v4l2.path" );
				if( ! device_path ) {
					device_path = gst_structure_get_string( props, "device.path" );
				}
				if( device_path ) {
					devicePath = device_path;
					g_object_set( source.get(), "device", device_path, nullptr );
				}
				else {
					throw CaptureExcInitFail( "Cannot determine device path for selected camera" );
				}
				gst_structure_free( props );
			}
		}
		else {
			throw CaptureExcInitFail( "No camera device available or selected" );
		}
	}

	// Set source format from Mode's platform data (caps string)
	GstElementPtr sourceCapsFilter( nullptr, gstElementDeleter );
	const std::string& capsString = mode.getPlatformData();
	if( ! capsString.empty() ) {
		sourceCapsFilter.reset( gst_element_factory_make( "capsfilter", "source-capsfilter" ) );
		if( sourceCapsFilter ) {
			GstCapsPtr sourceCaps( gst_caps_from_string( capsString.c_str() ), gstCapsDeleter );
			if( ! sourceCaps || gst_caps_is_empty( sourceCaps.get() ) ) {
				throw CaptureExcInitFail( "Failed to parse caps string from mode" );
			}
			g_object_set( sourceCapsFilter.get(), "caps", sourceCaps.get(), nullptr );
		}
	}

	// Create common elements
	GstElementPtr videoConvert( gst_element_factory_make( "videoconvert", "videoconvert" ), gstElementDeleter );
	GstElementPtr capsFilter( gst_element_factory_make( "capsfilter", "capsfilter" ), gstElementDeleter );
	GstElementPtr appSink( gst_element_factory_make( "appsink", "appsink" ), gstElementDeleter );

	if( ! videoConvert || ! capsFilter || ! appSink ) {
		throw CaptureExcInitFail( "Failed to create common GStreamer elements" );
	}

	// Determine pipeline type from codec
	PipelineType pipelineType = codecToPipeline( mode.getCodec() ).first;

	// Create pipeline-specific decoder elements
	GstElementPtr decoder( nullptr, gstElementDeleter );
	GstElementPtr parser( nullptr, gstElementDeleter );

	switch( pipelineType ) {
		case PipelineType::RAW_DIRECT:
			// No decoder needed for raw video
			break;

		case PipelineType::JPEG_DECODE:
			decoder.reset( gst_element_factory_make( "jpegdec", "jpegdec" ) );
			if( ! decoder ) {
				throw CaptureExcInitFail( "Failed to create JPEG decoder" );
			}
			break;

		case PipelineType::H264_DECODE:
			parser.reset( gst_element_factory_make( "h264parse", "h264parse" ) );
			decoder.reset( gst_element_factory_make( "avdec_h264", "avdec_h264" ) );
			if( ! parser || ! decoder ) {
				throw CaptureExcInitFail( "Failed to create H.264 decoder elements" );
			}
			break;

		case PipelineType::HEVC_DECODE:
			parser.reset( gst_element_factory_make( "h265parse", "h265parse" ) );
			decoder.reset( gst_element_factory_make( "avdec_h265", "avdec_h265" ) );
			if( ! parser || ! decoder ) {
				throw CaptureExcInitFail( "Failed to create HEVC/H.265 decoder elements" );
			}
			break;

		case PipelineType::DECODEBIN_FALLBACK:
		default:
			decoder.reset( gst_element_factory_make( "decodebin", "decodebin" ) );
			if( ! decoder ) {
				throw CaptureExcInitFail( "Failed to create decodebin fallback" );
			}
			break;
	}

	// Set RGB format but let resolution negotiate automatically
	GstCapsPtr caps( gst_caps_new_simple( "video/x-raw", "format", G_TYPE_STRING, "RGB", nullptr ), gstCapsDeleter );
	g_object_set( capsFilter.get(), "caps", caps.get(), nullptr );

	g_object_set( appSink.get(), "emit-signals", FALSE, "sync", FALSE, "max-buffers", 1, "drop", TRUE, nullptr );

	static GstAppSinkCallbacks sCallbacks = { nullptr, nullptr, &CaptureImplGStreamer::onNewSample };
	gst_app_sink_set_callbacks( GST_APP_SINK( appSink.get() ), &sCallbacks, this, nullptr );

	// Create pipeline
	GstElementPtr pipeline( gst_pipeline_new( "cinder-capture" ), gstElementDeleter );
	if( ! pipeline ) {
		throw CaptureExcInitFail( "Failed to create GStreamer pipeline" );
	}

	// Build and link pipeline based on type
	switch( pipelineType ) {
		case PipelineType::RAW_DIRECT:
			{
				// Pipeline: v4l2src [-> capsfilter] -> videoconvert -> capsfilter -> appsink
				if( sourceCapsFilter ) {
					gst_bin_add_many( GST_BIN( pipeline.get() ), source.get(), sourceCapsFilter.get(), videoConvert.get(), capsFilter.get(), appSink.get(), nullptr );
					if( ! gst_element_link_many( source.get(), sourceCapsFilter.get(), videoConvert.get(), capsFilter.get(), appSink.get(), nullptr ) ) {
						throw CaptureExcInitFail( "Failed to link RAW pipeline with source caps filter" );
					}
				}
				else {
					gst_bin_add_many( GST_BIN( pipeline.get() ), source.get(), videoConvert.get(), capsFilter.get(), appSink.get(), nullptr );
					if( ! gst_element_link_many( source.get(), videoConvert.get(), capsFilter.get(), appSink.get(), nullptr ) ) {
						throw CaptureExcInitFail( "Failed to link RAW pipeline" );
					}
				}
				break;
			}

		case PipelineType::JPEG_DECODE:
			{
				// Pipeline: v4l2src [-> capsfilter] -> jpegdec -> videoconvert -> capsfilter -> appsink
				if( sourceCapsFilter ) {
					gst_bin_add_many( GST_BIN( pipeline.get() ), source.get(), sourceCapsFilter.get(), decoder.get(), videoConvert.get(), capsFilter.get(), appSink.get(), nullptr );
					if( ! gst_element_link_many( source.get(), sourceCapsFilter.get(), decoder.get(), videoConvert.get(), capsFilter.get(), appSink.get(), nullptr ) ) {
						throw CaptureExcInitFail( "Failed to link JPEG pipeline with source caps filter" );
					}
				}
				else {
					gst_bin_add_many( GST_BIN( pipeline.get() ), source.get(), decoder.get(), videoConvert.get(), capsFilter.get(), appSink.get(), nullptr );
					if( ! gst_element_link_many( source.get(), decoder.get(), videoConvert.get(), capsFilter.get(), appSink.get(), nullptr ) ) {
						throw CaptureExcInitFail( "Failed to link JPEG pipeline" );
					}
				}
				break;
			}

		case PipelineType::H264_DECODE:
			{
				// Pipeline: v4l2src [-> capsfilter] -> h264parse -> avdec_h264 -> videoconvert -> capsfilter -> appsink
				if( sourceCapsFilter ) {
					gst_bin_add_many( GST_BIN( pipeline.get() ), source.get(), sourceCapsFilter.get(), parser.get(), decoder.get(), videoConvert.get(), capsFilter.get(), appSink.get(), nullptr );
					if( ! gst_element_link_many( source.get(), sourceCapsFilter.get(), parser.get(), decoder.get(), videoConvert.get(), capsFilter.get(), appSink.get(), nullptr ) ) {
						throw CaptureExcInitFail( "Failed to link H.264 pipeline with source caps filter" );
					}
				}
				else {
					gst_bin_add_many( GST_BIN( pipeline.get() ), source.get(), parser.get(), decoder.get(), videoConvert.get(), capsFilter.get(), appSink.get(), nullptr );
					if( ! gst_element_link_many( source.get(), parser.get(), decoder.get(), videoConvert.get(), capsFilter.get(), appSink.get(), nullptr ) ) {
						throw CaptureExcInitFail( "Failed to link H.264 pipeline" );
					}
				}
				break;
			}

		case PipelineType::HEVC_DECODE:
			{
				// Pipeline: v4l2src [-> capsfilter] -> h265parse -> avdec_h265 -> videoconvert -> capsfilter -> appsink
				if( sourceCapsFilter ) {
					gst_bin_add_many( GST_BIN( pipeline.get() ), source.get(), sourceCapsFilter.get(), parser.get(), decoder.get(), videoConvert.get(), capsFilter.get(), appSink.get(), nullptr );
					if( ! gst_element_link_many( source.get(), sourceCapsFilter.get(), parser.get(), decoder.get(), videoConvert.get(), capsFilter.get(), appSink.get(), nullptr ) ) {
						throw CaptureExcInitFail( "Failed to link HEVC pipeline with source caps filter" );
					}
				}
				else {
					gst_bin_add_many( GST_BIN( pipeline.get() ), source.get(), parser.get(), decoder.get(), videoConvert.get(), capsFilter.get(), appSink.get(), nullptr );
					if( ! gst_element_link_many( source.get(), parser.get(), decoder.get(), videoConvert.get(), capsFilter.get(), appSink.get(), nullptr ) ) {
						throw CaptureExcInitFail( "Failed to link HEVC pipeline" );
					}
				}
				break;
			}

		case PipelineType::DECODEBIN_FALLBACK:
		default:
			{
				// Pipeline: v4l2src [-> capsfilter] -> decodebin -> videoconvert -> capsfilter -> appsink
				if( sourceCapsFilter ) {
					gst_bin_add_many( GST_BIN( pipeline.get() ), source.get(), sourceCapsFilter.get(), decoder.get(), videoConvert.get(), capsFilter.get(), appSink.get(), nullptr );
					if( ! gst_element_link_many( source.get(), sourceCapsFilter.get(), decoder.get(), nullptr ) ) {
						throw CaptureExcInitFail( "Failed to link source through caps filter to decodebin" );
					}
				}
				else {
					gst_bin_add_many( GST_BIN( pipeline.get() ), source.get(), decoder.get(), videoConvert.get(), capsFilter.get(), appSink.get(), nullptr );
					if( ! gst_element_link( source.get(), decoder.get() ) ) {
						throw CaptureExcInitFail( "Failed to link source to decodebin" );
					}
				}

				// Link videoconvert to capsfilter to appsink (separate for decodebin's dynamic pads)
				if( ! gst_element_link_many( videoConvert.get(), capsFilter.get(), appSink.get(), nullptr ) ) {
					throw CaptureExcInitFail( "Failed to link decodebin video processing elements" );
				}

				// Store for decodebin pad-added callback
				mVideoConvert = videoConvert.get();

				// Connect decodebin's dynamic pad
				g_signal_connect( decoder.get(), "pad-added", G_CALLBACK( onDecoderPadAdded ), videoConvert.get() );
				break;
			}
	}

	// Transfer ownership to member variables (release from unique_ptr)
	// Note: All elements added to the pipeline are now owned by it
	mPipeline = pipeline.release();
	mSource = source.release();
	mVideoConvert = videoConvert.release();
	mCapsFilter = capsFilter.release();
	mAppSink = appSink.release();
	mBus = gst_element_get_bus( mPipeline );

	// Release ownership of elements that were added to pipeline but aren't stored as members
	sourceCapsFilter.release(); // Owned by pipeline now
	decoder.release();			// Owned by pipeline now
	parser.release();			// Owned by pipeline now

	return true;
}

// Safely tears down GStreamer pipeline and releases all resources. Waits for pipeline to reach NULL state.
void CaptureImplGStreamer::cleanupPipeline()
{
	// Guard against multiple calls (from stop() and destructor)
	if( ! mPipeline && ! mBus )
		return; // Already cleaned up

	stopBusWatch();

	if( mPipeline ) {
		// Set to NULL state and wait for completion before unreferencing
		gst_element_set_state( mPipeline, GST_STATE_NULL );

		// Wait for state change to complete - use longer timeout for complex pipelines
		// v4l2 devices with decoders/converters need time to fully release resources
		GstState state;
		gst_element_get_state( mPipeline, &state, nullptr, 5 * GST_SECOND );

		gst_object_unref( mPipeline );
	}

	if( mBus )
		gst_object_unref( mBus );

	mPipeline = nullptr;
	mSource = nullptr;
	mVideoConvert = nullptr;
	mCapsFilter = nullptr;
	mAppSink = nullptr;
	mBus = nullptr;
}

// Starts background thread to monitor pipeline messages. GStreamer "bus" is the message system for errors, state changes, etc.
void CaptureImplGStreamer::startBusWatch()
{
	if( ! mBus || mRunBusWatch )
		return;

	if( mBusWatchThread.joinable() )
		mBusWatchThread.join();

	mRunBusWatch = true;
	mBusWatchThread = std::thread( [this]() {
		while( mRunBusWatch ) {
			GstMessage* message = gst_bus_timed_pop( mBus, GST_MSECOND * 100 );
			if( ! message )
				continue;

			switch( GST_MESSAGE_TYPE( message ) ) {
				case GST_MESSAGE_ERROR:
					{
						GError* err = nullptr;
						gchar*	dbg = nullptr;
						gst_message_parse_error( message, &err, &dbg );
						if( err && err->message )
							CI_LOG_E( "Capture error: " << err->message );
						if( dbg )
							CI_LOG_E( "Debug info: " << dbg );

						// Mark as not capturing when pipeline errors occur
						// This handles device disconnection and other critical failures
						mIsCapturing = false;
						CI_LOG_W( "Device disconnection or pipeline failure detected - marking capture as stopped" );

						if( err )
							g_error_free( err );
						if( dbg )
							g_free( dbg );
						mRunBusWatch = false;
						break;
					}
				case GST_MESSAGE_EOS:
					mRunBusWatch = false;
					break;
				default:
					break;
			}

			gst_message_unref( message );
		}
	} );
}

// Stops bus monitoring thread and waits for it to finish.
void CaptureImplGStreamer::stopBusWatch()
{
	mRunBusWatch = false;
	if( mBusWatchThread.joinable() )
		mBusWatchThread.join();
}

// Callback for when decodebin creates output pads dynamically. "Pad" is GStreamer's term for element connection points.
// Decodebin inspects compressed data and creates appropriate output pads once it knows the format.
// static
void CaptureImplGStreamer::onDecoderPadAdded( GstElement* decoder, GstPad* pad, gpointer userData )
{
	GstElement* videoConvert = static_cast<GstElement*>( userData );
	GstPad*		sinkPad = gst_element_get_static_pad( videoConvert, "sink" );

	if( gst_pad_is_linked( sinkPad ) ) {
		gst_object_unref( sinkPad );
		return;
	}

	// Check if this is a video pad
	GstCaps*	  caps = gst_pad_query_caps( pad, nullptr );
	GstStructure* str = gst_caps_get_structure( caps, 0 );
	const gchar*  name = gst_structure_get_name( str );

	if( g_str_has_prefix( name, "video/" ) ) {
		GstPadLinkReturn ret = gst_pad_link( pad, sinkPad );
		if( GST_PAD_LINK_FAILED( ret ) ) {
			CI_LOG_E( "Failed to link decoder to videoconvert" );
		}
		else {
		}
	}

	gst_caps_unref( caps );
	gst_object_unref( sinkPad );
}

GstFlowReturn CaptureImplGStreamer::onNewSample( GstAppSink* sink, gpointer userData )
{
	auto* impl = static_cast<CaptureImplGStreamer*>( userData );
	return impl->handleSample( gst_app_sink_pull_sample( sink ) );
}

// Processes incoming video frame from GStreamer. "Sample" is GStreamer's container for a video frame plus metadata.
// Extracts pixel data and copies to Cinder surface format.
GstFlowReturn CaptureImplGStreamer::handleSample( GstSample* sample )
{
	if( ! sample ) {
		return GST_FLOW_OK;
	}

	// Take ownership of the sample
	GstSamplePtr samplePtr( sample, gstSampleDeleter );

	GstCaps*   caps = gst_sample_get_caps( samplePtr.get() );
	GstBuffer* buffer = gst_sample_get_buffer( samplePtr.get() );
	if( ! caps || ! buffer ) {
		return GST_FLOW_OK;
	}

	GstVideoInfo info;
	if( ! gst_video_info_from_caps( &info, caps ) ) {
		return GST_FLOW_OK;
	}

	GstMapInfo mapInfo;
	if( ! gst_buffer_map( buffer, &mapInfo, GST_MAP_READ ) ) {
		return GST_FLOW_OK;
	}

	int32_t width = GST_VIDEO_INFO_WIDTH( &info );
	int32_t height = GST_VIDEO_INFO_HEIGHT( &info );
	int32_t stride = GST_VIDEO_INFO_PLANE_STRIDE( &info, 0 );

	{
		lock_guard<mutex> lock( mMutex );

		if( ! mSurfaceCache )
			mSurfaceCache = make_unique<SurfaceCache>( width, height, kCaptureChannelOrder, 4 );
		else
			mSurfaceCache->resize( width, height );

		Surface8uRef   surface = mSurfaceCache->getNewSurface();
		uint8_t*	   dst = surface->getData();
		int32_t		   dstStride = surface->getRowBytes();
		const uint8_t* src = mapInfo.data;

		for( int32_t row = 0; row < height; ++row ) {
			memcpy( dst + row * dstStride, src + row * stride, std::min<int32_t>( dstStride, stride ) );
		}

		mCurrentFrame = surface;
		mHasNewFrame = true;
		mWidth = width;
		mHeight = height;
	}

	gst_buffer_unmap( buffer, &mapInfo );
	// samplePtr automatically cleaned up

	return GST_FLOW_OK;
}

// static
const vector<Capture::DeviceRef>& CaptureImplGStreamer::getDevices( bool forceRefresh )
{
	ensureGStreamerInitialized();

	static vector<Capture::DeviceRef> sDevices;
	static mutex					  sDeviceMutex;
	static bool						  sEnumerated = false;

	if( forceRefresh ) {
		lock_guard<mutex> lock( sDeviceMutex );
		sDevices.clear();
		sEnumerated = false;
	}

	if( sEnumerated )
		return sDevices;

	lock_guard<mutex> lock( sDeviceMutex );
	if( sEnumerated )
		return sDevices;

	GstDeviceMonitor* monitor = gst_device_monitor_new();
	gst_device_monitor_add_filter( monitor, "Video/Source", nullptr );
	gst_device_monitor_start( monitor );

	GList* devices = gst_device_monitor_get_devices( monitor );
	for( GList* it = devices; it != nullptr; it = it->next ) {
		GstDevice* gstDevice = GST_DEVICE( it->data );
		if( ! gstDevice )
			continue;

		string name = gst_device_get_display_name( gstDevice );
		string uniqueId = deriveUniqueId( gstDevice );
		auto   device = make_shared<CaptureImplGStreamer::Device>( gstDevice, name, uniqueId );
		sDevices.push_back( device );
	}

	g_list_free_full( devices, g_object_unref );
	gst_device_monitor_stop( monitor );
	gst_object_unref( monitor );

	sEnumerated = true;
	return sDevices;
}

} // namespace cinder

#endif // defined( CINDER_LINUX )
