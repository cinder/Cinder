cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( OSC-SimpleReceiver )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../.." ABSOLUTE )
get_filename_component( SAMPLE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

set( CMAKE_MODULE_PATH "${CINDER_PATH}/blocks/OSC/proj/cmake" )

find_package( Cinder-OSC REQUIRED )

ci_make_app(
	SOURCES     ${SAMPLE_DIR}/src/SimpleReceiverApp.cpp
	CINDER_PATH ${CINDER_PATH}
)

add_dependencies( OSC-SimpleReceiver Cinder-OSC )
target_link_libraries( OSC-SimpleReceiver Cinder-OSC )
