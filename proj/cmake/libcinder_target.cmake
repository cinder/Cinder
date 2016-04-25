cmake_minimum_required( VERSION 3.0 FATAL_ERROR )

cmake_policy( SET CMP0022 NEW )

# note: CINDER_TARGET_SUBFOLDER is defined by each platform config, to be a folder that lives in cinder/lib/*
# note: CINDER_TARGET_GL_SUBFOLDER is necessary for Linux since we can have various builds depending on the target GL.
# e.g on the TK1 we can build both core profile and es2 so this takes care of putting everything on the right place.
# For other platforms than Linux if this var is not needed will just stay empty.
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/${CINDER_TARGET_SUBFOLDER}/${CMAKE_BUILD_TYPE}/${CINDER_TARGET_GL_SUBFOLDER} )

message( "CMAKE_ARCHIVE_OUTPUT_DIRECTORY: ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}" )

add_library(
    cinder STATIC
    ${CINDER_SRC_FILES}
)

target_include_directories( cinder BEFORE PUBLIC ${CINDER_INCLUDE_USER} )
target_include_directories( cinder SYSTEM BEFORE PUBLIC ${CINDER_INCLUDE_SYSTEM} )

target_link_libraries( cinder INTERFACE ${CINDER_LIBS_DEPENDS} )

target_compile_definitions( cinder PUBLIC ${CINDER_DEFINES} )

# Check compiler support for enabling c++11 or c++14.
include( CheckCXXCompilerFlag )
CHECK_CXX_COMPILER_FLAG( "-std=c++14" COMPILER_SUPPORTS_CXX14 )
CHECK_CXX_COMPILER_FLAG( "-std=c++11" COMPILER_SUPPORTS_CXX11 )

if( COMPILER_SUPPORTS_CXX14 )
	set( CINDER_COMPILER_FLAGS "-std=c++14" )
elseif( COMPILER_SUPPORTS_CXX11 )
	set( CINDER_COMPILER_FLAGS "-std=c++11" )
else()
	message( FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has neither C++11 or C++14 support. Please use a different C++ compiler." )
endif()

set( CMAKE_CXX_FLAGS ${CINDER_COMPILER_FLAGS} ${CMAKE_CXX_FLAGS} )
target_compile_options( cinder INTERFACE ${CINDER_COMPILER_FLAGS} )

# This file will contain all dependencies, includes, definition, compiler flags and so on..
export( TARGETS cinder FILE ${PROJECT_BINARY_DIR}/cinderTargets.cmake )

# And this command will generate a file on the ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
# that applications have to pull in order to link successfully with Cinder and its dependencies.
# This specific cinderConfig.cmake file will just hold a path to the above mention cinderTargets.cmake file which holds the actual info.
configure_file( ${CMAKE_CURRENT_LIST_DIR}/modules/cinderConfig.buildtree.cmake.in
	${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/cinderConfig.cmake
)
