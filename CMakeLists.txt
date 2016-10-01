cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( cinder )

option( CINDER_BUILD_SAMPLES "Build all samples." OFF )
option( CINDER_BUILD_TESTS "Build all unit tests." OFF )
set( CINDER_BUILD_SAMPLE "" CACHE STRING "Build a specific sample by specifying its path relative to the samples directory (ex. '_opengl/Cube')." )

set( CINDER_PATH      "${CMAKE_CURRENT_SOURCE_DIR}" )
set( CINDER_CMAKE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/proj/cmake" )

include( ${CINDER_CMAKE_DIR}/configure.cmake )
include( ${CINDER_CMAKE_DIR}/libcinder_configure.cmake )
include( ${CINDER_CMAKE_DIR}/libcinder_source_files.cmake )

set( CINDER_SKIP_SAMPLES "" )

if( CINDER_LINUX )
	include( "${CINDER_CMAKE_DIR}/platform_linux.cmake" )
elseif( CINDER_MAC )
	include( "${CINDER_CMAKE_DIR}/platform_macosx.cmake" )
elseif( CINDER_ANDROID )
	include( "${CINDER_CMAKE_DIR}/platform_android.cmake" )
elseif( CINDER_MSW )
	include( "${CINDER_CMAKE_DIR}/platform_msw.cmake" )
else()
	message( FATAL_ERROR "no target defined for system: '${CMAKE_SYSTEM_NAME}.'" )
endif()

include( ${CINDER_CMAKE_DIR}/libcinder_target.cmake )

if( CINDER_BUILD_SAMPLES )
	include( ${CINDER_CMAKE_DIR}/modules/findCMakeDirs.cmake )

	set( allSamples "" )
	findCMakeDirs( allSamples "${CINDER_PATH}/samples" "${CINDER_SKIP_SAMPLES}" )

	foreach( sampleDir ${allSamples} )
		if( CINDER_VERBOSE )
			message( STATUS "adding sample: ${sampleDir}" )
		endif()
		add_subdirectory( ${sampleDir} )
	endforeach()
elseif( CINDER_BUILD_SAMPLE )
	if( CINDER_VERBOSE )
		message( STATUS "adding sample: ${CINDER_BUILD_SAMPLE}" )
	endif()
	add_subdirectory( ${CINDER_PATH}/samples/${CINDER_BUILD_SAMPLE}/proj/cmake )
endif()

if( CINDER_BUILD_TESTS )
	enable_testing()
	add_subdirectory( ${CINDER_PATH}/test/unit/proj/cmake )
	add_custom_target( check COMMAND ${CMAKE_CTEST_COMMAND} --verbose )
endif()
