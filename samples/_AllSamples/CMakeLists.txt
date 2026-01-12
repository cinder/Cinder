# CMakeLists.txt for building all Cinder samples
#
# Usage (standalone):
#   mkdir build && cd build
#   cmake ..
#   cmake --build .
#
# Usage (from Cinder root):
#   cmake -DCINDER_BUILD_ALL_SAMPLES=ON ..

cmake_minimum_required( VERSION 3.16 FATAL_ERROR )
project( CinderAllSamples )

# Find Cinder path - either we're in samples/_AllSamples or user specified it
if( NOT CINDER_PATH )
	get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE )
endif()

# Include Cinder's configuration
include( "${CINDER_PATH}/proj/cmake/configure.cmake" )

# Option to control where sample outputs are placed
option( CINDER_SAMPLES_INLINE_BUILD "Build samples to their individual proj/cmake/build directories" OFF )

if( NOT CINDER_SAMPLES_INLINE_BUILD )
	# Centralized output - all samples build to the main build directory
	# This allows 'cmake --build . --target clean' to fully clean all outputs
	set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" )
endif()

# Include the sample discovery module
include( "${CINDER_PATH}/proj/cmake/modules/findCMakeDirs.cmake" )

#
# Platform-specific skip lists
# Samples are skipped if they don't have native project support for the target platform.
#
# Format: path relative to samples/ directory (e.g., "_opengl/Cube" or "BasicApp")
#

# Windows-only samples (D3D11, NVIDIA-specific, compute shaders)
set( SAMPLES_MSW_ONLY
	D3d11ImageBasic
	D3d11Triangle
	_opengl/LevelOfDetailIndirect
	_opengl/NVidiaComputeParticles
	_opengl/NvidiaMulticast
	_opengl/ParticleSphereCS
)

# macOS-only samples (Metal)
set( SAMPLES_MAC_ONLY
	MetalImageBasic
	MetalTriangle
)

# iOS-only samples (CoreMotion)
set( SAMPLES_IOS_ONLY
	MotionBasic
)

# Samples that work on macOS and iOS but not Windows/Linux (CoreLocation, etc.)
# Note: LocationManager block needs ObjC++ compilation fixes for CMake builds
set( SAMPLES_MAC_IOS_ONLY
	QuickTimeIteration
)

# Samples that need CMake/code fixes before they can build
set( SAMPLES_NEEDS_FIXES
	LocationManager
	QuickTimeAvfWriter
	Renderer2dBasic
)

# Samples with CMake support but no native IDE projects (CMake-first)
# These work on all platforms that CMake supports
set( SAMPLES_CMAKE_ONLY
	CanvasUi
	CanvasUiMinimap
	ImGui
)


# Build the skip list based on current platform
set( CINDER_SKIP_SAMPLES "" )

if( CINDER_MSW )
	# Skip macOS-only, iOS-only, and Mac+iOS samples on Windows
	list( APPEND CINDER_SKIP_SAMPLES ${SAMPLES_MAC_ONLY} )
	list( APPEND CINDER_SKIP_SAMPLES ${SAMPLES_IOS_ONLY} )
	list( APPEND CINDER_SKIP_SAMPLES ${SAMPLES_MAC_IOS_ONLY} )
elseif( CINDER_MAC )
	# Skip Windows-only and iOS-only samples on macOS
	list( APPEND CINDER_SKIP_SAMPLES ${SAMPLES_MSW_ONLY} )
	list( APPEND CINDER_SKIP_SAMPLES ${SAMPLES_IOS_ONLY} )
elseif( CINDER_COCOA_TOUCH )
	# Skip Windows-only and macOS-desktop-only samples on iOS
	list( APPEND CINDER_SKIP_SAMPLES ${SAMPLES_MSW_ONLY} )
	list( APPEND CINDER_SKIP_SAMPLES ${SAMPLES_MAC_ONLY} )
elseif( CINDER_LINUX )
	# Skip all platform-specific samples on Linux
	list( APPEND CINDER_SKIP_SAMPLES ${SAMPLES_MSW_ONLY} )
	list( APPEND CINDER_SKIP_SAMPLES ${SAMPLES_MAC_ONLY} )
	list( APPEND CINDER_SKIP_SAMPLES ${SAMPLES_IOS_ONLY} )
	list( APPEND CINDER_SKIP_SAMPLES ${SAMPLES_MAC_IOS_ONLY} )
endif()

# Skip samples that need code fixes
list( APPEND CINDER_SKIP_SAMPLES ${SAMPLES_NEEDS_FIXES} )

# Also skip the _AllSamples directory itself to avoid recursion
list( APPEND CINDER_SKIP_SAMPLES _AllSamples )

# Find all sample directories with CMake support
set( ALL_SAMPLES "" )
findCMakeDirs( ALL_SAMPLES "${CINDER_PATH}/samples" "${CINDER_SKIP_SAMPLES}" )

# Add each sample as a subdirectory
foreach( sampleDir ${ALL_SAMPLES} )
	# Extract relative path from samples/ for the binary directory name
	# e.g., .../samples/_opengl/Cube/proj/cmake -> _opengl/Cube
	string( REPLACE "${CINDER_PATH}/samples/" "" sampleRelPath "${sampleDir}" )
	string( REPLACE "/proj/cmake" "" sampleRelPath "${sampleRelPath}" )

	message( STATUS "Adding sample: ${sampleRelPath}" )
	add_subdirectory( "${sampleDir}" "${CMAKE_BINARY_DIR}/samples/${sampleRelPath}" )
endforeach()

list( LENGTH ALL_SAMPLES sampleCount )
message( STATUS "Configured ${sampleCount} samples for ${CINDER_TARGET}" )
