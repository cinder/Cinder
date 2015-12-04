cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( BasicApp )

if( NOT CINDER_SAMPLES )
	add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/../../../.." "${CMAKE_CURRENT_SOURCE_DIR}/../../../../tmpbuild" )
endif()

get_filename_component( SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../src" ABSOLUTE )

add_executable(
	"${PROJECT_NAME}"
	${SRC_DIR}/BasicApp.cpp
)

target_link_libraries( ${PROJECT_NAME} cinder )
