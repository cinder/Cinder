#
# Env var or param:
#     ANDROID_NDK_ROOT - should point NDK
#     ANDROID_NDK_STANDALONE - should point to standalone toolchain
#

cmake_minimum_required( VERSION 2.8 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColorReset    "${Esc}[m"     )
  set(ColorBold     "${Esc}[1m"    )
  set(Red           "${Esc}[31m"   )
  set(Green         "${Esc}[32m"   )
  set(Yellow        "${Esc}[33m"   )
  set(Blue          "${Esc}[34m"   )
  set(Magenta       "${Esc}[35m"   )
  set(Cyan          "${Esc}[36m"   )
  set(White         "${Esc}[37m"   )
  set(BoldRed       "${Esc}[1;31m" )
  set(BoldGreen     "${Esc}[1;32m" )
  set(BoldYellow    "${Esc}[1;33m" )
  set(BoldBlue      "${Esc}[1;34m" )
  set(BoldMagenta   "${Esc}[1;35m" )
  set(BoldCyan      "${Esc}[1;36m" )
  set(BoldWhite     "${Esc}[1;37m" )
endif()


# ------------------------------------------------------------------------
# NDK_ROOT
# ------------------------------------------------------------------------
# Check env vars for ANDROID_NDK_ROOT first
if( DEFINED ENV{ANDROID_NDK_ROOT} )
    set( NDK_ROOT $ENV{ANDROID_NDK_ROOT} )
endif()

# Check if ANDROID_NDK_ROOT was overriden as a param
if( ANDROID_NDK_ROOT )
    message( "milk! ${ANDROID_NDK_ROOT}" )
    set( NDK_ROOT ${ANDROID_NDK_ROOT} )
endif()

# Bail if we haven't set NDK_ROOT
if( NOT NDK_ROOT )
    message( FATAL_ERROR "${BoldRed}ANDROID_NDK_ROOT not set!${ColorReset}" )
endif()
# ------------------------------------------------------------------------

# ------------------------------------------------------------------------
# NDK_STANDALONE
# ------------------------------------------------------------------------
# Check env vars for ANDROID_NDK_STANDALONE first
if( DEFINED ENV{ANDROID_NDK_STANDALONE} )
    set( NDK_STANDALONE $ENV{ANDROID_NDK_STANDALONE} )
endif()

# Check if ANDROID_NDK_STANDALONE was overriden as a parm
if( ANDROID_NDK_STANDALONE )
    message( "milk! ${ANDROID_NDK_STANDALONE}" )
    set( NDK_STANDALONE ${ANDROID_NDK_STANDALONE} )
endif()

# Bail if we haven't set NDK_STANDALONE
if( NOT NDK_STANDALONE )
    message( FATAL_ERROR "${BoldRed}ANDROID_NDK_STANDALONE not set!${ColorReset}" )
endif()
# ------------------------------------------------------------------------


message( "${BoldBlue}ANDROID_NDK_ROOT      : ${NDK_ROOT}${ColorReset}" )
message( "${BoldBlue}ANDROID_NDK_STANDALONE: ${NDK_STANDALONE}${ColorReset}" )
message( "" )

# User this for now
set( TOOLCHAIN_PREFIX "arm-linux-androideabi" )

set( CMAKE_C_COMPILER   ${NDK_STANDALONE}/bin/${TOOLCHAIN_PREFIX}-gcc    CACHE FILEPATH "" FORCE )
set( CMAKE_CXX_COMPILER ${NDK_STANDALONE}/bin/${TOOLCHAIN_PREFIX}-g++    CACHE FILEPATH "" FORCE )
set( CMAKE_AR           ${NDK_STANDALONE}/bin/${TOOLCHAIN_PREFIX}-ar     CACHE FILEPATH "" FORCE )
set( CMAKE_LINKER       ${NDK_STANDALONE}/bin/${TOOLCHAIN_PREFIX}-ld     CACHE FILEPATH "" FORCE )
set( CMAKE_NM           ${NDK_STANDALONE}/bin/${TOOLCHAIN_PREFIX}-nm     CACHE FILEPATH "" FORCE )
set( CMAKE_RANLIB       ${NDK_STANDALONE}/bin/${TOOLCHAIN_PREFIX}-ranlib CACHE FILEPATH "" FORCE )
set( CMAKE_STRIP        ${NDK_STANDALONE}/bin/${TOOLCHAIN_PREFIX}-strip  CACHE FILEPATH "" FORCE )

project( cinder )

get_filename_component( CINDER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE )
set( CINDER_INC_DIR ${CINDER_DIR}/include )
set( CINDER_SRC_DIR ${CINDER_DIR}/src )
set( BOOST_INC_DIR ${CINDER_DIR}/boost )

set( LIBRARY_OUTPUT_PATH ${CINDER_DIR}/lib )

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
message( "${BoldGreen}CMAKE_STRIP       : ${CMAKE_STRIP}${ColorReset}" ) 
message( "" )

include_directories( 
    ${CINDER_INC_DIR} 
    ${CINDER_INC_DIR}/jsoncpp 
    ${CINDER_SRC_DIR}/linebreak 
    ${CINDER_SRC_DIR}/libtess2 
    ${BOOST_INC_DIR} 
)

set( C_FLAGS "-march=armv7-a -mfpu=neon -fvisibility=default" )
set( CMAKE_C_FLAGS_DEBUG   "${CMAKE_C_FLAGS_DEBUG} -g ${C_FLAGS}" )
set( CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -ffast-math ${C_FLAGS}" )

set( CXX_FLAGS "-march=armv7-a -mfpu=neon -fvisibility=default" )
set( CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11 -g -fexceptions ${CXX_FLAGS}" )
set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++11 -fexceptions -O3 -ffast-math ${CXX_FLAGS}" )

set( CINDER_C_SRC_FILES
    ${CINDER_SRC_DIR}/cinder/android/android_native_app_glue.c

    ${CINDER_SRC_DIR}/linebreak/linebreak.c
    ${CINDER_SRC_DIR}/linebreak/linebreakdata.c
    ${CINDER_SRC_DIR}/linebreak/linebreakdef.c

    ${CINDER_SRC_DIR}/libtess2/bucketalloc.c
    ${CINDER_SRC_DIR}/libtess2/dict.c
    ${CINDER_SRC_DIR}/libtess2/geom.c
    ${CINDER_SRC_DIR}/libtess2/mesh.c
    ${CINDER_SRC_DIR}/libtess2/priorityq.c
    ${CINDER_SRC_DIR}/libtess2/sweep.c
    ${CINDER_SRC_DIR}/libtess2/tess.c
)

set( CINDER_CXX_SRC_FILES
    ${CINDER_SRC_DIR}/cinder/android/LogCatStream.cpp

    ${CINDER_SRC_DIR}/cinder/app/android/AppAndroid.cpp
    ${CINDER_SRC_DIR}/cinder/app/android/AppImplAndroid.cpp
    ${CINDER_SRC_DIR}/cinder/app/android/AssetFileSystem.cpp
    ${CINDER_SRC_DIR}/cinder/app/android/EventManagerAndroid.cpp
    ${CINDER_SRC_DIR}/cinder/app/android/PlatformAndroid.cpp
    ${CINDER_SRC_DIR}/cinder/app/android/Renderer2dAndroid.cpp
    ${CINDER_SRC_DIR}/cinder/app/android/RendererGlAndroid.cpp
    ${CINDER_SRC_DIR}/cinder/app/android/WindowImplAndroid.cpp

    ${CINDER_SRC_DIR}/cinder/app/AppBase.cpp
    ${CINDER_SRC_DIR}/cinder/app/AppScreenSaver.cpp
    ${CINDER_SRC_DIR}/cinder/app/KeyEvent.cpp
    ${CINDER_SRC_DIR}/cinder/app/Platform.cpp
    ${CINDER_SRC_DIR}/cinder/app/Renderer.cpp
    ${CINDER_SRC_DIR}/cinder/app/RendererGl.cpp
    ${CINDER_SRC_DIR}/cinder/app/Window.cpp

    ${CINDER_SRC_DIR}/cinder/gl/gl.cpp
    ${CINDER_SRC_DIR}/cinder/gl/Batch.cpp
    ${CINDER_SRC_DIR}/cinder/gl/BufferObj.cpp
    ${CINDER_SRC_DIR}/cinder/gl/BufferTexture.cpp
    ${CINDER_SRC_DIR}/cinder/gl/ConstantStrings.cpp
    ${CINDER_SRC_DIR}/cinder/gl/Context.cpp
    ${CINDER_SRC_DIR}/cinder/gl/Environment.cpp
    ${CINDER_SRC_DIR}/cinder/gl/EnvironmentCore.cpp
    ${CINDER_SRC_DIR}/cinder/gl/EnvironmentEs.cpp
    ${CINDER_SRC_DIR}/cinder/gl/Fbo.cpp
    ${CINDER_SRC_DIR}/cinder/gl/GlslProg.cpp
    ${CINDER_SRC_DIR}/cinder/gl/Pbo.cpp
    ${CINDER_SRC_DIR}/cinder/gl/Query.cpp
    ${CINDER_SRC_DIR}/cinder/gl/Shader.cpp
    ${CINDER_SRC_DIR}/cinder/gl/Sync.cpp
    ${CINDER_SRC_DIR}/cinder/gl/Texture.cpp
    #${CINDER_SRC_DIR}/cinder/gl/TextureFont.cpp
    ${CINDER_SRC_DIR}/cinder/gl/TextureFormatParsers.cpp
    ${CINDER_SRC_DIR}/cinder/gl/TransformFeedbackObj.cpp
    ${CINDER_SRC_DIR}/cinder/gl/TransformFeedbackObjImplHardware.cpp
    ${CINDER_SRC_DIR}/cinder/gl/TransformFeedbackObjImplSoftware.cpp
    ${CINDER_SRC_DIR}/cinder/gl/Ubo.cpp
    ${CINDER_SRC_DIR}/cinder/gl/Vao.cpp
    ${CINDER_SRC_DIR}/cinder/gl/VaoImplCore.cpp
    ${CINDER_SRC_DIR}/cinder/gl/VaoImplEs.cpp
    ${CINDER_SRC_DIR}/cinder/gl/Vbo.cpp
    ${CINDER_SRC_DIR}/cinder/gl/VboMesh.cpp

    ${CINDER_SRC_DIR}/cinder/ip/Blend.cpp
    ${CINDER_SRC_DIR}/cinder/ip/EdgeDetect.cpp
    ${CINDER_SRC_DIR}/cinder/ip/Fill.cpp
    ${CINDER_SRC_DIR}/cinder/ip/Flip.cpp
    ${CINDER_SRC_DIR}/cinder/ip/Grayscale.cpp
    ${CINDER_SRC_DIR}/cinder/ip/Hdr.cpp
    ${CINDER_SRC_DIR}/cinder/ip/Premultiply.cpp
    ${CINDER_SRC_DIR}/cinder/ip/Resize.cpp
    ${CINDER_SRC_DIR}/cinder/ip/Threshold.cpp
    ${CINDER_SRC_DIR}/cinder/ip/Trim.cpp

    ${CINDER_SRC_DIR}/cinder/svg/Svg.cpp

    ${CINDER_SRC_DIR}/cinder/Area.cpp
    ${CINDER_SRC_DIR}/cinder/AxisAlignedBox.cpp
    ${CINDER_SRC_DIR}/cinder/BSpline.cpp
    ${CINDER_SRC_DIR}/cinder/BSplineFit.cpp
    ${CINDER_SRC_DIR}/cinder/Buffer.cpp
    ${CINDER_SRC_DIR}/cinder/Camera.cpp
    ${CINDER_SRC_DIR}/cinder/Channel.cpp
    ${CINDER_SRC_DIR}/cinder/CinderAssert.cpp
    ${CINDER_SRC_DIR}/cinder/CinderMath.cpp
    ${CINDER_SRC_DIR}/cinder/Color.cpp
    ${CINDER_SRC_DIR}/cinder/ConvexHull.cpp
    ${CINDER_SRC_DIR}/cinder/DataSource.cpp
    ${CINDER_SRC_DIR}/cinder/DataTarget.cpp
    ${CINDER_SRC_DIR}/cinder/Display.cpp
    ${CINDER_SRC_DIR}/cinder/Exception.cpp
    ${CINDER_SRC_DIR}/cinder/Frustum.cpp
    ${CINDER_SRC_DIR}/cinder/GeomIo.cpp
    ${CINDER_SRC_DIR}/cinder/ImageIo.cpp
    ${CINDER_SRC_DIR}/cinder/ImageSourceFileRadiance.cpp
    ${CINDER_SRC_DIR}/cinder/ImageSourceFileStbImage.cpp
    ${CINDER_SRC_DIR}/cinder/Log.cpp
    ${CINDER_SRC_DIR}/cinder/Matrix.cpp
    ${CINDER_SRC_DIR}/cinder/MatrixStack.cpp
    ${CINDER_SRC_DIR}/cinder/Path2d.cpp
    ${CINDER_SRC_DIR}/cinder/Perlin.cpp
    ${CINDER_SRC_DIR}/cinder/Plane.cpp
    ${CINDER_SRC_DIR}/cinder/PolyLine.cpp
    ${CINDER_SRC_DIR}/cinder/Rand.cpp
    ${CINDER_SRC_DIR}/cinder/Ray.cpp
    ${CINDER_SRC_DIR}/cinder/Rect.cpp
    ${CINDER_SRC_DIR}/cinder/Shape2d.cpp
    ${CINDER_SRC_DIR}/cinder/Signals.cpp
    ${CINDER_SRC_DIR}/cinder/Sphere.cpp
    ${CINDER_SRC_DIR}/cinder/Stream.cpp
    ${CINDER_SRC_DIR}/cinder/Surface.cpp
    ${CINDER_SRC_DIR}/cinder/System.cpp
    ${CINDER_SRC_DIR}/cinder/Timeline.cpp
    ${CINDER_SRC_DIR}/cinder/TimelineItem.cpp
    ${CINDER_SRC_DIR}/cinder/Timer.cpp
    ${CINDER_SRC_DIR}/cinder/Triangulate.cpp
    ${CINDER_SRC_DIR}/cinder/TriMesh.cpp
    ${CINDER_SRC_DIR}/cinder/Tween.cpp
    ${CINDER_SRC_DIR}/cinder/Unicode.cpp
    ${CINDER_SRC_DIR}/cinder/Url.cpp
    #${CINDER_SRC_DIR}/cinder/UrlImplCurl.cpp
    ${CINDER_SRC_DIR}/cinder/Utilities.cpp
    ${CINDER_SRC_DIR}/cinder/Xml.cpp

    ${CINDER_SRC_DIR}/jsoncpp/jsoncpp.cpp
)

#set_source_files_properties( ${CINDER_CXX_SRC_FILES} COMPILE_FLAGS "-std=c++11" )

set( CMAKE_DEBUG_POSTFIX "_d" )
add_library( cinder STATIC ${CINDER_C_SRC_FILES} ${CINDER_CXX_SRC_FILES} )


