cmake_minimum_required( VERSION 3.0 FATAL_ERROR )

set( CINDER_PLATFORM "Mac OS X" )

include( ${CINDER_CMAKE_DIR}/libcinder_configure_build.cmake )
include( ${CINDER_CMAKE_DIR}/libcinder_source_files.cmake )
include( ${CINDER_CMAKE_DIR}/libcinder_target.cmake )