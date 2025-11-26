cmake_minimum_required( VERSION 3.15 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( D3d12Triangle )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../.." ABSOLUTE )
get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

ci_make_app(
	SOURCES     ${APP_PATH}/src/D3d12TriangleApp.cpp
	CINDER_PATH ${CINDER_PATH}
	LIBRARIES   d3d12 dxgi d3dcompiler
)
