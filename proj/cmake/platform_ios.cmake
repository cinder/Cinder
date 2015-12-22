cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

set( CINDER_PLATFORM "Cocoa" )
set( CINDER_TARGET_SUBFOLDER "ios" )

include( ${CINDER_CMAKE_DIR}/libcinder_configure_build.cmake )
include( ${CINDER_CMAKE_DIR}/libcinder_source_files.cmake )
include( ${CINDER_CMAKE_DIR}/libcinder_target.cmake )