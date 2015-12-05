cmake_minimum_required( VERSION 3.1 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE on )

# Module path
set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )

# Find architecture name
execute_process( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE CINDER_ARCH )

set( CINDER_INCLUDE_DEPENDS )
set( CINDER_LIBS_DEPENDS )

set( CINDER_USE_X11 				true )
set( CINDER_USE_SYSTEM_GL_HEADERS 	true )

if( CINDER_LINUX_EGL_RPI2 )
	#if( NOT "arm7vl" STREQUAL "${CINDER_ARCH}" )
	#	message( FATAL_ERROR "${BoldRed}Wrong architecture for Raspberry Pi 2: ${CINDER_ARCH}${ColorReset}" ) 
	#endif()

	set( CINDER_LINUX_EGL_RPI2 			true )
	set( CINDER_GL_ES 					true )
    set( CINDER_USE_X11 				false )
    set( CINDER_USE_SYSTEM_GL_HEADERS 	false )

    list( APPEND CINDER_INCLUDE_DEPENDS 
		/opt/vc/include
		/opt/vc/include/interface/vcos/pthreads 
	)

    list( APPEND CINDER_LIBS_DEPENDS 
		/opt/vc/lib/libEGL.so
		/opt/vc/lib/libGLESv2.so 
		/opt/vc/lib/libbcm_host.so 
	)
endif()

set( CINDER_LIB_SUFFIX )
set( CINDER_OUT_DIR_PREFIX )

if( CINDER_GL_ES_2 OR CINDER_GL_ES_3 OR CINDER_GL_ES_3_1 OR CINDER_GL_ES_3_2 )
	set( CINDER_GL_ES 					true )
    set( CINDER_USE_SYSTEM_GL_HEADERS 	false )

	# RPI2 is GL libs are include and linked from '/opt/vc'.
    if( NOT CINDER_LINUX_EGL_RPI2 )
        list( APPEND CINDER_LIBS_DEPENDS EGL GLESv2 )
    endif()

	if( CINDER_GL_ES_2 )
		set( CINDER_GL_ES_FLAGS "-DCINDER_GL_ES_2" )
        set( CINDER_LIB_SUFFIX -es2 )
        set( CINDER_OUT_DIR_PREFIX es2 )
		# Raspberry Pi special flags
		if( CINDER_LINUX_EGL_RPI2 )
			set( CINDER_GL_ES_FLAGS "${CINDER_GL_ES_FLAGS} -DCINDER_LINUX_EGL_ONLY -DCINDER_LINUX_EGL_RPI2" )
		endif()		
	elseif( CINDER_GL_ES_3 )
		set( CINDER_GL_ES_FLAGS "-DCINDER_GL_ES_3" )
        set( CINDER_LIB_SUFFIX -es3 )
        set( CINDER_OUT_DIR_PREFIX es3 )
	elseif( CINDER_GL_ES_3_1 )
		set( CINDER_GL_ES_FLAGS "-DCINDER_GL_ES_3_1" )
        set( CINDER_LIB_SUFFIX -es31 )
        set( CINDER_OUT_DIR_PREFIX es31 )
	elseif( CINDER_GL_ES_3_2 )
		set( CINDER_GL_ES_FLAGS "-DCINDER_GL_ES_3_2" )
        set( CINDER_LIB_SUFFIX -es32 )
        set( CINDER_OUT_DIR_PREFIX es32 )
    else()
        set( CINDER_LIB_SUFFIX )
        set( CINDER_OUT_DIR_PREFIX ogl )
	endif()
endif()

get_filename_component( CINDER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE )
set( CINDER_INC_DIR ${CINDER_DIR}/include )
set( CINDER_SRC_DIR ${CINDER_DIR}/src )
set( CINDER_LIB_DIR ${CINDER_DIR}/lib/linux/${CINDER_ARCH} )

find_package( CLANG )

if( NOT CINDER_TOOLCHAIN_GCC AND CLANG_FOUND )
    set( CMAKE_C_COMPILER						"${CLANG_CLANG}"		CACHE FILEPATH "" FORCE )
    set( CMAKE_CXX_COMPILER						"${CLANG_CLANGXX}"		CACHE FILEPATH "" FORCE )
    set( CMAKE_AR          						"${CLANG_LLVM_AR}"		CACHE FILEPATH "" FORCE )
    set( CMAKE_LINKER       					"${CLANG_LLVM_LINK}"	CACHE FILEPATH "" FORCE )
    set( CMAKE_NM           					"${CLANG_LLVM_NM}"		CACHE FILEPATH "" FORCE )
    set( CMAKE_RANLIB       					"${CLANG_LLVM_RANLIB}"	CACHE FILEPATH "" FORCE )

    set( CMAKE_C_FLAGS_INIT						"-Wall -std=c99" 	CACHE STRING "" FORCE )
    set( CMAKE_C_FLAGS_DEBUG_INIT				"-g" 				CACHE STRING "" FORCE )
    set( CMAKE_C_FLAGS_MINSIZEREL_INIT			"-Os -DNDEBUG" 		CACHE STRING "" FORCE )
    set( CMAKE_C_FLAGS_RELEASE_INIT				"-O4 -DNDEBUG" 		CACHE STRING "" FORCE )
    set( CMAKE_C_FLAGS_RELWITHDEBINFO_INIT		"-O2 -g" 			CACHE STRING "" FORCE )
    set( CMAKE_C_FLAGS							"${CMAKE_C_FLAGS} -fmessage-length=0 " CACHE STRING "" FORCE )

    set( CMAKE_CXX_FLAGS_INIT					"-Wall" 		CACHE STRING "" FORCE )
    set( CMAKE_CXX_FLAGS_DEBUG_INIT				"-g" 			CACHE STRING "" FORCE )
    set( CMAKE_CXX_FLAGS_MINSIZEREL_INIT		"-Os -DNDEBUG" 	CACHE STRING "" FORCE ) 
    set( CMAKE_CXX_FLAGS_RELEASE_INIT			"-O4 -DNDEBUG" 	CACHE STRING "" FORCE )
    set( CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT	"-O2 -g" 		CACHE STRING "" FORCE )
    set( CMAKE_CCC_FLAGS						"${CMAKE_C_FLAGS} -fmessage-length=0 " CACHE STRING "" FORCE )

    set( STDCXXLIB                           	"-stdlib=libstdc++" )
else()
	# Keep these versionless
    set( CMAKE_C_COMPILER						"gcc"	CACHE FILEPATH "" FORCE )
    set( CMAKE_CXX_COMPILER						"g++" 	CACHE FILEPATH "" FORCE )
	set( CINDER_TOOLCHAIN_GCC 					true 	CACHE BOOL "" FORCE )
	set( CINDER_TOOLCHAIN_CLANG 				false 	CACHE BOOL "" FORCE )
endif()

# Needs to come after compilers are set
project( cinder_linux )

cmake_policy( SET CMP0022 NEW )

# Output path
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CINDER_DIR}/lib/linux/${CINDER_ARCH}" )

include(${CMAKE_MODULE_PATH}/DebugColours.cmake)

if( CINDER_LINUX_EGL_RPI2 )
	message( "${BoldGreen}CINDER_ARCH : ${CINDER_ARCH}${ColorReset}" )
	message( "${BoldGreen}CINDER_LINUX: ${CINDER_LINUX}${ColorReset}" )
else()
	message( "${BoldGreen}CINDER_ARCH: ${CINDER_ARCH}${ColorReset}" )
endif()

message( "" )
message( "${BoldGreen}CINDER_INC_DIR: ${CINDER_INC_DIR}${ColorReset}" )
message( "${BoldGreen}CINDER_SRC_DIR: ${CINDER_SRC_DIR}${ColorReset}" )
message( "" )
message( "${BoldGreen}LIBRARY_OUTPUT_PATH: ${LIBRARY_OUTPUT_PATH}${ColorReset}" )
message( "" )
message( "${BoldGreen}CMAKE_C_COMPILER  : ${CMAKE_C_COMPILER}${ColorReset}" )
message( "${BoldGreen}CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}${ColorReset}" )
message( "${BoldGreen}CMAKE_AR          : ${CMAKE_AR}${ColorReset}" ) 
message( "${BoldGreen}CMAKE_LINKER      : ${CMAKE_LINKER}${ColorReset}" ) 
message( "${BoldGreen}CMAKE_NM          : ${CMAKE_NM}${ColorReset}" ) 
message( "${BoldGreen}CMAKE_RANLIB      : ${CMAKE_RANLIB}${ColorReset}" ) 
message( "" )


if( NOT CINDER_LINUX_EGL_RPI2 )
	if( CINDER_GL_ES )
		set( GLFW_FLAGS "-D_GLFW_X11 -D_GLFW_EGL -D_GLFW_USE_GLESV2" )
	else()
		set( GLFW_FLAGS "-D_GLFW_X11 -D_GLFW_GLX -D_GLFW_USE_OPENGL" )
	endif()
endif()

# C flags
set( C_FLAGS "-fvisibility=default -D_UNIX ${GLFW_FLAGS} ${CINDER_GL_ES_FLAGS}" )

# Disable warnings for C files - append at end
set( C_DISABLE_WARNINGS "-Wno-unused-function" )

# Set C flags
set( CMAKE_C_FLAGS_DEBUG   "${C_FLAGS} -g3 -ggdb3 -O0 ${C_DISABLE_WARNINGS}" )
set( CMAKE_C_FLAGS_RELEASE "${C_FLAGS} -O3 -ffast-math ${C_DISABLE_WARNINGS}" )

# C++ flags - TODO: Add logic for the case when GCC5's new C++ ABI is desired.
set( CXX_FLAGS "-D_GLIBCXX_USE_CXX11_ABI=0 ${STDCXXLIB} -std=c++11 -fvisibility=default -D_UNIX" )

if( CINDER_TOOLCHAIN_CLANG )
	# Disable these warnings, many of which are coming from Boost - append at end
	set( CXX_DISABLE_WARNINGS "-Wno-reorder -Wno-unused-function -Wno-unused-private-field -Wno-unused-local-typedef -Wno-tautological-compare -Wno-missing-braces" )
elseif( CINDER_TOOLCHAIN_GCC )
	execute_process( COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION )
	if( ( "${GCC_VERSION}" VERSION_GREATER 5.0 ) OR ( "${GCC_VERSION}" VERSION_EQUAL 5.0 ) )
		# Disable these warnings, many of which are coming from Boost - append at end
		set( CXX_DISABLE_WARNINGS "-Wno-deprecated-declarations" )
	endif()
endif()

# C++ flags
set( CMAKE_CXX_FLAGS_DEBUG    "${CXX_FLAGS} -g3 -ggdb3 -O0 -fexceptions -frtti ${CINDER_GL_ES_FLAGS} ${CXX_DISABLE_WARNINGS}" )
set( CMAKE_CXX_FLAGS_RELEASE  "${CXX_FLAGS} -O3 -fexceptions -frtti -ffast-math ${CINDER_GL_ES_FLAGS} ${CXX_DISABLE_WARNINGS}" )

# Include directories
include(${CMAKE_MODULE_PATH}/CinderIncludes.cmake)

# Source files
include(${CMAKE_MODULE_PATH}/CinderSources.cmake)

# Required defaults
list( APPEND CINDER_LIBS_DEPENDS dl )
list( APPEND CINDER_LIBS_DEPENDS pthread )

### If this is set then we should be on Desktop.
if( CINDER_USE_SYSTEM_GL_HEADERS )
    find_package( OpenGL REQUIRED )
    list( APPEND CINDER_INCLUDE_DEPENDS ${OPENGL_INCLUDE_DIR} )
    list( APPEND CINDER_LIBS_DEPENDS 	${OPENGL_LIBRARIES} )
endif()

### No X for RPI2.
if( CINDER_USE_X11 )
    find_package( X11 REQUIRED )
    list( APPEND CINDER_INCLUDE_DEPENDS ${X11_INCLUDE_DIR} )
    list( APPEND CINDER_LIBS_DEPENDS 	${X11_LIBRARIES} Xcursor Xinerama Xrandr Xi )
endif()

# zlib
find_package( ZLIB REQUIRED )
list( APPEND CINDER_INCLUDE_DEPENDS ${ZLIB_INCLUDE_DIR} )
list( APPEND CINDER_LIBS_DEPENDS 	${ZLIB_LIBRARIES} )

# curl
find_package( CURL REQUIRED )
list( APPEND CINDER_LIBS_DEPENDS ${CURL_LIBRARIES} )

# fontconfig
find_package( FontConfig REQUIRED )
list( APPEND CINDER_INCLUDE_DEPENDS ${FONTGONFIG_INCLUDE_DIRS} )
list( APPEND CINDER_LIBS_DEPENDS 	${FONTCONFIG_LIBRARIES} )

# PulseAudio
find_package( PulseAudio REQUIRED )
list( APPEND CINDER_LIBS_DEPENDS ${PULSEAUDIO_LIBRARY} )

# mpg123
find_package( MPG123 REQUIRED )
list( APPEND CINDER_INCLUDE_DEPENDS ${MPG123_INCLUDE_DIR} )
list( APPEND CINDER_LIBS_DEPENDS 	${MPG123_LIBRARY} )

# sndfile
find_package( SNDFILE REQUIRED )
list( APPEND CINDER_LIBS_DEPENDS ${SNDFILE_LIBRARY} )

# GStreamer
set( CINDER_USE_GSTREAMER true )

if( CINDER_USE_GSTREAMER )
    find_package (Glib REQUIRED COMPONENTS gobject )
    list( APPEND CINDER_INCLUDE_DEPENDS ${GLIB_INCLUDE_DIRS} )
    list( APPEND CINDER_LIBS_DEPENDS 	${GLIB_GOBJECT_LIBRARIES} ${GLIB_LIBRARIES} )

    find_package( GStreamer REQUIRED )
    list( APPEND CINDER_INCLUDE_DEPENDS 
		${GSTREAMER_INCLUDE_DIRS} 
		${GSTREAMER_BASE_INCLUDE_DIRS}
		${GSTREAMER_APP_INCLUDE_DIRS}
		${GSTREAMER_VIDEO_INCLUDE_DIRS}
        ${GSTREAMER_GL_INCLUDE_DIRS}
	)

    list( APPEND CINDER_LIBS_DEPENDS 
		${GSTREAMER_LIBRARIES} 
		${GSTREAMER_BASE_LIBRARIES} 
		${GSTREAMER_APP_LIBRARIES} 
		${GSTREAMER_VIDEO_LIBRARIES} 
        ${GSTREAMER_GL_LIBRARIES}
	)
endif()

### Set this to use system wide libs instead of compiling dependencies from source.
set( CINDER_USE_SYSTEM_LIBS false )
if( CINDER_USE_SYSTEM_LIBS )
    find_package( Boost 1.54 REQUIRED COMPONENTS system filesystem )
    list( APPEND CINDER_INCLUDE_DEPENDS ${Boost_INCLUDE_DIRS} )
    list( APPEND CINDER_LIBS_DEPENDS 	${Boost_LIBRARIES} )

    find_package( Freetype2 REQUIRED )
    list( APPEND CINDER_INCLUDE_DEPENDS ${FREETYPE2_INCLUDE_DIRS} )
    list( APPEND CINDER_LIBS_DEPENDS 	${FREETYPE2_LIBRARIES} )

else()
    set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFT2_BUILD_LIBRARY -DFT_DEBUG_LEVEL_TRACE" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DFT2_BUILD_LIBRARY -DFT_DEBUG_LEVEL_TRACE" )

    include( ${CMAKE_MODULE_PATH}/Freetype2Sources.cmake )

    list( APPEND CINDER_C_SRC_FILES 
		${FREETYPE2_SOURCES} 
	)

    list( APPEND CINDER_INCLUDE_DEPENDS 
		${CINDER_INC_DIR}/freetype 
	)

    list( APPEND CINDER_LIBS_DEPENDS 
		${CINDER_LIB_DIR}/libboost_system.a
		${CINDER_LIB_DIR}/libboost_filesystem.a 
	)
endif()

### Create our Cinder target.
add_library( cinder${CINDER_LIB_SUFFIX} STATIC ${CINDER_C_SRC_FILES} ${CINDER_CXX_SRC_FILES} )

### https://cmake.org/cmake/help/v3.0/command/target_include_directories.html
###
### target_include_directories:
### ---------------------------
### Arguments specified with the PRIVATE keyword WILL NOT populate the INTERFACE_INCLUDE_DIRECTORIES property.
### Arguments specified with the INTERFACE or PUBLIC keywords WILL populate the INTERFACE_INCLUDE_DIRECTORIES.
### This essentially means that when a project links with Cinder it does not have to respecify include directories 
### that are parts of Cinder's INTERFACE_INCLUDE_DIRECTORY property.

### https://cmake.org/cmake/help/v3.0/command/target_link_libraries.html 
###
### target_link_libraries:
### ----------------------
### The same idea as for target_include_directories but in this case the INTERFACE_LINK_LIBRARIES property
### gets populated. Again, this essentially means that libraries that have been specified with the 
### INTERFACE or PUBLIC keyword do not need to be respecified when a project links with Cinder.
###
### e.g you could do :
###
### target_link_libraries( binary cinder )
###
### and this will pull all Cinder's dependencies specified with INTERFACE or PUBLIC.

### There is a conflict between boost/regex.h and /usr/include/regex.h when building the examples
### so avoid propagating the boost dirs across the compilation of samples.
if( NOT CINDER_USE_SYSTEM_LIBS )
    target_include_directories( cinder${CINDER_LIB_SUFFIX} PRIVATE ${CINDER_INC_DIR}/boost> )
endif()

target_include_directories( cinder${CINDER_LIB_SUFFIX} PUBLIC "$<BUILD_INTERFACE:${CINDER_INCLUDE_DIRS};${CINDER_INCLUDE_DEPENDS}>" )

target_link_libraries( cinder${CINDER_LIB_SUFFIX} INTERFACE ${CINDER_LIBS_DEPENDS} )

set_target_properties( cinder${CINDER_LIB_SUFFIX} PROPERTIES DEBUG_POSTFIX "_d" )

### https://cmake.org/cmake/help/v3.0/command/export.html
###
### export
### ------
### This command exports a config file named 'cinderConfig.cmake' in the build directory of Cinder. 
### This config file loads the 'cinderTargets.cmake' file which CMake generates for us with the export command under the current build directory.
### This file 'describes' Cinder as a target and its dependencies so that can then be consumed by projects linking with Cinder. 
### The INTERFACE_INCLUDE_DIRECTORIES and INTERFACE_LINK_LIBRARIES properies mentioned above are for example part of this file.
###
### Check one of the samples for example usage of the generated config file.

export( TARGETS cinder${CINDER_LIB_SUFFIX} FILE "${PROJECT_BINARY_DIR}/cinderTargets.cmake" )

configure_file( 
	"${PROJECT_SOURCE_DIR}/cmake/cinderConfig.buildtree.cmake.in"
	"${PROJECT_BINARY_DIR}/cinderConfig.cmake" 
)


if( CINDER_BUILD_SAMPLES )
    add_subdirectory( ${CINDER_DIR}/samples/BasicApp/linux ${CINDER_DIR}/samples/BasicApp/linux/${CMAKE_BUILD_TYPE}/${CINDER_OUT_DIR_PREFIX} )
    add_subdirectory( ${CINDER_DIR}/samples/BasicAppMultiWindow/linux ${CINDER_DIR}/samples/BasicAppMultiWindow/linux/${CMAKE_BUILD_TYPE}/${CINDER_OUT_DIR_PREFIX} )
    add_subdirectory( ${CINDER_DIR}/samples/_opengl/Cube/linux ${CINDER_DIR}/samples/_opengl/Cube/linux/${CMAKE_BUILD_TYPE}/${CINDER_OUT_DIR_PREFIX} )
    add_subdirectory( ${CINDER_DIR}/samples/_opengl/CubeMapping/linux ${CINDER_DIR}/samples/_opengl/CubeMapping/linux/${CMAKE_BUILD_TYPE}/${CINDER_OUT_DIR_PREFIX} )
    add_subdirectory( ${CINDER_DIR}/samples/_opengl/NormalMapping/linux ${CINDER_DIR}/samples/_opengl/NormalMapping/linux/${CMAKE_BUILD_TYPE}/${CINDER_OUT_DIR_PREFIX} )
    add_subdirectory( ${CINDER_DIR}/samples/_opengl/NormalMappingBasic/linux ${CINDER_DIR}/samples/_opengl/NormalMappingBasic/linux/${CMAKE_BUILD_TYPE}/${CINDER_OUT_DIR_PREFIX} )
    add_subdirectory( ${CINDER_DIR}/samples/_opengl/ShadowMapping/linux ${CINDER_DIR}/samples/_opengl/ShadowMapping/linux/${CMAKE_BUILD_TYPE}/${CINDER_OUT_DIR_PREFIX} )
    add_subdirectory( ${CINDER_DIR}/samples/_opengl/ShadowMappingBasic/linux ${CINDER_DIR}/samples/_opengl/ShadowMappingBasic/linux/${CMAKE_BUILD_TYPE}/${CINDER_OUT_DIR_PREFIX} )
endif()
