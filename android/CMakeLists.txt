#
# Environmental variables:
#     NDK_ROOT - should point NDK
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

set( CMAKE_C_COMPILER   $ENV{NDK_ROOT}/bin/arm-linux-androideabi-gcc )
set( CMAKE_CXX_COMPILER $ENV{NDK_ROOT}/bin/arm-linux-androideabi-g++ )
#set( CMAKE_C_COMPILER   $ENV{NDK_ROOT}/bin/clang )
#set( CMAKE_CXX_COMPILER $ENV{NDK_ROOT}/bin/clang++ )

project( cinder )

get_filename_component( CINDER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE )
set( CINDER_INC_DIR ${CINDER_DIR}/include )
set( CINDER_SRC_DIR ${CINDER_DIR}/src )
set( BOOST_INC_DIR ${CINDER_DIR}/boost )


message( "${BoldGreen}CINDER_INC_DIR: ${CINDER_INC_DIR}${ColorReset}" )
message( "${BoldGreen}CINDER_SRC_DIR: ${CINDER_SRC_DIR}${ColorReset}" )

include_directories( ${CINDER_INC_DIR} ${BOOST_INC_DIR} )


set( CINDER_SRC_FILES
    ${CINDER_SRC_DIR}/cinder/app/AppBase.cpp
    ${CINDER_SRC_DIR}/cinder/app/AppBasic.cpp
    ${CINDER_SRC_DIR}/cinder/app/Renderer.cpp
    ${CINDER_SRC_DIR}/cinder/app/RendererGl.cpp
    ${CINDER_SRC_DIR}/cinder/app/Window.cpp
    ${CINDER_SRC_DIR}/cinder/app/AppImplAndroid.cpp
    ${CINDER_SRC_DIR}/cinder/app/AppBasicAndroid.cpp
    ${CINDER_SRC_DIR}/cinder/gl/Environment.cpp

    ${CINDER_SRC_DIR}/cinder/Area.cpp
    ${CINDER_SRC_DIR}/cinder/AxisAlignedBox.cpp
    ${CINDER_SRC_DIR}/cinder/BSpline.cpp
    ${CINDER_SRC_DIR}/cinder/BSplineFit.cpp
    ${CINDER_SRC_DIR}/cinder/Buffer.cpp
    ${CINDER_SRC_DIR}/cinder/Camera.cpp
    ${CINDER_SRC_DIR}/cinder/Channel.cpp
    ${CINDER_SRC_DIR}/cinder/CinderAssert.cpp
    ${CINDER_SRC_DIR}/cinder/CinderMath.cpp
    ${CINDER_SRC_DIR}/cinder/Display.cpp
    ${CINDER_SRC_DIR}/cinder/Exception.cpp
    ${CINDER_SRC_DIR}/cinder/Frustum.cpp
    ${CINDER_SRC_DIR}/cinder/Surface.cpp
)

set_source_files_properties( ${CINDER_SRC_FILES} COMPILE_FLAGS "-std=c++11" )

add_library( cinder STATIC ${CINDER_SRC_FILES} )


