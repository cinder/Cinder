# GStreamer Player.
cmake_minimum_required( VERSION 3.1 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

get_filename_component( CINDER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../.." ABSOLUTE )

include(${CINDER_DIR}/linux/cmake/Cinder.cmake)

project( GStreamerApp )

get_filename_component( SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src" ABSOLUTE )
get_filename_component( INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../include" ABSOLUTE ) 

if( NOT TARGET cinder${CINDER_LIB_SUFFIX} )
    find_package( cinder REQUIRED
        PATHS ${PROJECT_SOURCE_DIR}/../../../../linux/${CMAKE_BUILD_TYPE}/${CINDER_OUT_DIR_PREFIX}
        $ENV{Cinder_DIR}/linux/${CMAKE_BUILD_TYPE}/${CINDER_OUT_DIR_PREFIX}
    )
endif()

set( SRC_FILES
    ${SRC_DIR}/CinderGStreamerApp.cpp
)

### CMAKE_PROJECT_NAME returns the top-level project name
### if present so use PROJECT_NAME instead.
set( EXE_NAME ${PROJECT_NAME} )

add_executable( "${EXE_NAME}" ${SRC_FILES} )

target_include_directories( "${EXE_NAME}" PRIVATE "${INC_DIR}" )

target_link_libraries( "${EXE_NAME}" cinder${CINDER_LIB_SUFFIX} )

