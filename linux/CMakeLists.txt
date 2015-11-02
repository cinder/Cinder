cmake_minimum_required( VERSION 3.1 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE on )

execute_process( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE CINDER_ARCH )
execute_process( COMMAND which clang OUTPUT_VARIABLE CLANG_PATH )

set( CINDER_INCLUDE_DEPENDS )
set( CINDER_LIBS_DEPENDS )

set( CINDER_USE_X11 True )
set( CINDER_USE_SYSTEM_GL_HEADERS True )

if( CINDER_LINUX_EGL_RPI2 )
message( "RPI " ${CINDER_LINUX_EGL_ONLY} )
	#if( NOT "arm7vl" STREQUAL "${CINDER_ARCH}" )
	#	message( FATAL_ERROR "${BoldRed}Wrong architecture for Raspberry Pi 2: ${CINDER_ARCH}${ColorReset}" ) 
	#endif()

	set( CINDER_LINUX_EGL_RPI2 true )
	set( CINDER_GL_ES true )
    set( CINDER_USE_X11 False )
    set( CINDER_USE_SYSTEM_GL_HEADERS False )

    list( APPEND CINDER_INCLUDE_DEPENDS /opt/vc/include
                                        /opt/vc/include/interface/vcos/pthreads )

    list( APPEND CINDER_LIBS_DEPENDS /opt/vc/lib/libEGL.so /opt/vc/lib/libGLESv2.so /opt/vc/lib/libbcm_host.so )
endif()

set( CINDER_LIB_SUFFIX )
set( CINDER_OUT_DIR_PREFIX )

if( CINDER_GL_ES_2 OR CINDER_GL_ES_3 OR CINDER_GL_ES_3_1 OR CINDER_GL_ES_3_2 )
	set( CINDER_GL_ES true )
    set( CINDER_USE_SYSTEM_GL_HEADERS False )
    if( NOT CINDER_LINUX_EGL_RPI2 ) # RPI2 is GL libs are include and linked from '/opt/vc'. 
        list( APPEND CINDER_LIBS_DEPENDS EGL GLESv2 )
    endif()
	if( CINDER_GL_ES_2 )
		set( CINDER_GL_ES_FLAGS "-DCINDER_GL_ES_2" )
        set( CINDER_LIB_SUFFIX -es2 )
        set( CINDER_OUT_DIR_PREFIX es2 )
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

if( NOT "${CLANG_PATH}" STREQUAL "" )
    set( CINDER_TOOLCHAIN_CLANG true )
endif()


if( CINDER_TOOLCHAIN_CLANG )
    set( CMAKE_C_COMPILER	"clang" 		CACHE FILEPATH "" FORCE )
    set( CMAKE_CXX_COMPILER	"clang++" 		CACHE FILEPATH "" FORCE )
    set( CMAKE_AR           "llvm-ar"		CACHE FILEPATH "" FORCE )
    set( CMAKE_LINKER       "llvm-link" 	CACHE FILEPATH "" FORCE )
    set( CMAKE_NM           "llvm-nm " 		CACHE FILEPATH "" FORCE )
    set( CMAKE_RANLIB       "llvm-ranlib"	CACHE FILEPATH "" FORCE )
    #set( CMAKE_STRIP        ""				CACHE FILEPATH "" FORCE )

    set( CMAKE_C_FLAGS_INIT					"-Wall -std=c99" CACHE STRING "" FORCE )
    set( CMAKE_C_FLAGS_DEBUG_INIT			"-g" CACHE STRING "" FORCE )
    set( CMAKE_C_FLAGS_MINSIZEREL_INIT		"-Os -DNDEBUG" CACHE STRING "" FORCE )
    set( CMAKE_C_FLAGS_RELEASE_INIT			"-O4 -DNDEBUG" CACHE STRING "" FORCE )
    set( CMAKE_C_FLAGS_RELWITHDEBINFO_INIT	"-O2 -g" CACHE STRING "" FORCE )
    set( CMAKE_C_FLAGS						"${CMAKE_C_FLAGS} -fmessage-length=0 " CACHE STRING "" FORCE )

    set( CMAKE_CXX_FLAGS_INIT				"-Wall" CACHE STRING "" FORCE )
    set( CMAKE_CXX_FLAGS_DEBUG_INIT			"-g" CACHE STRING "" FORCE )
    set( CMAKE_CXX_FLAGS_MINSIZEREL_INIT		"-Os -DNDEBUG" CACHE STRING "" FORCE ) 
    set( CMAKE_CXX_FLAGS_RELEASE_INIT		"-O4 -DNDEBUG" CACHE STRING "" FORCE )
    set( CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT	"-O2 -g" CACHE STRING "" FORCE )
    set( CMAKE_CCC_FLAGS						"${CMAKE_C_FLAGS} -fmessage-length=0 " CACHE STRING "" FORCE )

    set( STDCXXLIB                           "-stdlib=libstdc++" )
else()
    set( CMAKE_C_COMPILER	"gcc-4.9" 		CACHE FILEPATH "" FORCE )
    set( CMAKE_CXX_COMPILER	"g++-4.9" 		CACHE FILEPATH "" FORCE )	
endif()

# Needs to come ater compilers are set
project( cinder_linux )

cmake_policy(SET CMP0022 NEW)

# Output path
set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )
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

set( C_FLAGS "-fvisibility=default ${GLFW_FLAGS} -D_UNIX" )
set( CMAKE_C_FLAGS_DEBUG   "${C_FLAGS} -g" )
set( CMAKE_C_FLAGS_RELEASE "${C_FLAGS} -Os -ffast-math" )

# Disable these warnings, many of which are coming from Boost
set( CXX_DISABLE_WARNINGS "-Wno-reorder -Wno-unused-private-field -Wno-unused-local-typedef -Wno-tautological-compare -Wno-missing-braces" )
set( CXX_FLAGS "${STDCXXLIB} -std=c++11 -fvisibility=default ${CINDER_GL_ES_FLAGS} -D_UNIX ${CXX_DISABLE_WARNINGS}" )

if( CINDER_LINUX_EGL_RPI2 )
	set( CXX_FLAGS "${CXX_FLAGS} -DCINDER_LINUX_EGL_ONLY -DCINDER_LINUX_EGL_RPI2 -DCINDER_GL_ES_2" )
endif()

set( CMAKE_CXX_FLAGS_DEBUG    "${CXX_FLAGS} -g -fexceptions -frtti" )
set( CMAKE_CXX_FLAGS_RELEASE  "${CXX_FLAGS} -Os -fexceptions -frtti -ffast-math" )

include(${CMAKE_MODULE_PATH}/CinderIncludes.cmake)
include(${CMAKE_MODULE_PATH}/CinderSources.cmake)

# If this is set then we should be on Desktop.
if( CINDER_USE_SYSTEM_GL_HEADERS )
    find_package( OpenGL REQUIRED )
    list( APPEND CINDER_INCLUDE_DEPENDS ${OPENGL_INCLUDE_DIR} )
    list( APPEND CINDER_LIBS_DEPENDS ${OPENGL_LIBRARIES} )
endif()

if( CINDER_USE_X11 )
    find_package( X11 REQUIRED )
    list( APPEND CINDER_INCLUDE_DEPENDS ${X11_INCLUDE_DIR} )
    list( APPEND CINDER_LIBS_DEPENDS ${X11_LIBRARIES} Xcursor Xinerama Xrandr Xi )
endif()

find_package( ZLIB REQUIRED )
list( APPEND CINDER_INCLUDE_DEPENDS ${ZLIB_INCLUDE_DIR} )
list( APPEND CINDER_LIBS_DEPENDS ${ZLIB_LIBRARIES} )

list( APPEND CINDER_LIBS_DEPENDS dl )
list( APPEND CINDER_LIBS_DEPENDS pthread )

find_package( FontConfig REQUIRED )
list( APPEND CINDER_INCLUDE_DEPENDS ${FONTGONFIG_INCLUDE_DIRS} )
list( APPEND CINDER_LIBS_DEPENDS ${FONTCONFIG_LIBRARIES} )

set( CINDER_USE_GSTREAMER False )

if( CINDER_USE_GSTREAMER )
    find_package (Glib REQUIRED COMPONENTS gobject )
    list( APPEND CINDER_INCLUDE_DEPENDS ${GLIB_INCLUDE_DIRS} )
    list(APPEND CINDER_LIBS_DEPENDS ${GLIB_GOBJECT_LIBRARIES} ${GLIB_LIBRARIES} )

    find_package( GStreamer REQUIRED )
    list( APPEND CINDER_INCLUDE_DEPENDS ${GSTREAMER_INCLUDE_DIRS} ${GSTREAMER_BASE_INCLUDE_DIRS} ${GSTREAMER_APP_INCLUDE_DIRS} ${GSTREAMER_VIDEO_INCLUDE_DIRS} )
    list( APPEND CINDER_LIBS_DEPENDS ${GSTREAMER_LIBRARIES} ${GSTREAMER_BASE_LIBRARIES} ${GSTREAMER_APP_LIBRARIES} ${GSTREAMER_VIDEO_LIBRARIES} )
endif()

# Set this to use system wide libs instead of compiling dependencies from source.
set( CINDER_USE_SYSTEM_LIBS False )
if( CINDER_USE_SYSTEM_LIBS )
    find_package( Boost 1.54 REQUIRED COMPONENTS system filesystem )
    list( APPEND CINDER_INCLUDE_DEPENDS ${Boost_INCLUDE_DIRS} )
    list( APPEND CINDER_LIBS_DEPENDS ${Boost_LIBRARIES} )

    find_package( Freetype2 REQUIRED )
    list( APPEND CINDER_INCLUDE_DEPENDS ${FREETYPE2_INCLUDE_DIRS} )
    list( APPEND CINDER_LIBS_DEPENDS ${FREETYPE2_LIBRARIES} )

else()
    set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFT2_BUILD_LIBRARY -DFT_DEBUG_LEVEL_TRACE" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DFT2_BUILD_LIBRARY -DFT_DEBUG_LEVEL_TRACE" )

    include( ${CMAKE_MODULE_PATH}/Freetype2Sources.cmake )
    list( APPEND CINDER_C_SRC_FILES ${FREETYPE2_SOURCES} )


    list( APPEND CINDER_INCLUDE_DEPENDS ${CINDER_INC_DIR}/freetype )

    list( APPEND CINDER_LIBS_DEPENDS ${CINDER_LIB_DIR}/libboost_system.a
                                    ${CINDER_LIB_DIR}/libboost_filesystem.a )
endif()

add_library( cinder${CINDER_LIB_SUFFIX} STATIC ${CINDER_C_SRC_FILES} ${CINDER_CXX_SRC_FILES} )

if( NOT CINDER_USE_SYSTEM_LIBS )
    target_include_directories( cinder${CINDER_LIB_SUFFIX} PRIVATE ${CINDER_INC_DIR}/boost> )
endif()

target_include_directories( cinder${CINDER_LIB_SUFFIX} PUBLIC "$<BUILD_INTERFACE:${CINDER_INCLUDE_DIRS};${CINDER_INCLUDE_DEPENDS}>" )

target_link_libraries( cinder${CINDER_LIB_SUFFIX} INTERFACE ${CINDER_LIBS_DEPENDS} )

set_target_properties( cinder${CINDER_LIB_SUFFIX} PROPERTIES DEBUG_POSTFIX "_d" )

export( TARGETS cinder${CINDER_LIB_SUFFIX} FILE "${PROJECT_BINARY_DIR}/cinderTargets.cmake" )

configure_file( "${PROJECT_SOURCE_DIR}/cmake/cinderConfig.buildtree.cmake.in"
                    "${PROJECT_BINARY_DIR}/cinderConfig.cmake" )

set( CINER_BUILD_SAMPLES False )
if( CINDER_BUILD_SAMPLES )
    add_subdirectory( ${CINDER_DIR}/samples/BasicApp/linux ${CINDER_DIR}/samples/BasicApp/linux/${CMAKE_BUILD_TYPE}/${CINDER_OUT_DIR_PREFIX} )
    add_subdirectory( ${CINDER_DIR}/samples/BasicAppMultiWindow/linux ${CINDER_DIR}/samples/BasicAppMultiWindow/linux/${CMAKE_BUILD_TYPE}/${CINDER_OUT_DIR_PREFIX} )
    add_subdirectory( ${CINDER_DIR}/samples/_opengl/ShadowMapping/linux ${CINDER_DIR}/samples/_opengl/ShadowMapping/linux/${CMAKE_BUILD_TYPE}/${CINDER_OUT_DIR_PREFIX} )
    add_subdirectory( ${CINDER_DIR}/samples/_opengl/ShadowMappingBasic/linux ${CINDER_DIR}/samples/_opengl/ShadowMappingBasic/linux/${CMAKE_BUILD_TYPE}/${CINDER_OUT_DIR_PREFIX} )
    add_subdirectory( ${CINDER_DIR}/samples/_opengl/NormalMapping/linux ${CINDER_DIR}/samples/_opengl/NormalMapping/linux/${CMAKE_BUILD_TYPE}/${CINDER_OUT_DIR_PREFIX} )
    add_subdirectory( ${CINDER_DIR}/samples/_opengl/CubeMapping/linux ${CINDER_DIR}/samples/_opengl/CubeMapping/linux/${CMAKE_BUILD_TYPE}/${CINDER_OUT_DIR_PREFIX} )
endif()
