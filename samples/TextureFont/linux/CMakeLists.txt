cmake_minimum_required( VERSION 3.1 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

get_filename_component( CINDER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE )
include( ${CINDER_DIR}/tools/linux/Cinder.cmake )

project( TextureFont )

get_filename_component( SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src" ABSOLUTE )

message( "${BoldGreen}CINDER_INC_DIR: ${CINDER_INC_DIR}${ColorReset}" )
message( "${BoldGreen}CINDER_SRC_DIR: ${CINDER_SRC_DIR}${ColorReset}" )
message( "" )
message( "${BoldGreen}SRC_DIR       : ${SRC_DIR}${ColorReset}" )
message( "" )

include_directories( 
    ${CINDER_INC_DIR} 
    ${CINDER_INC_DIR}/glfw
    ${BOOST_INC_DIR}  
)

if( CINDER_LINUX_EGL_RPI2 )
	include_directories( /opt/vc/include )
	link_directories( /opt/vc/lib )
endif()

set( SRC_FILES
  ${SRC_DIR}/TextureFontApp.cpp
)

set( EXE_NAME "${CMAKE_PROJECT_NAME}" )
if( CINDER_GL_ES_2 )
	set( EXE_NAME "${CMAKE_PROJECT_NAME}-es2" )
endif()

add_executable( "${EXE_NAME}" ${SRC_FILES} )
target_link_libraries( "${EXE_NAME}" debug     ${CINDER_LIB_DIR}/libcinder${CINDER_LIB_SUFFIX}_d.a )
target_link_libraries( "${EXE_NAME}" optimized ${CINDER_LIB_DIR}/libcinder${CINDER_LIB_SUFFIX}.a )
target_link_libraries( "${EXE_NAME}" general   ${CINDER_LIB_DIR}/libboost_filesystem.a )
target_link_libraries( "${EXE_NAME}" general   ${CINDER_LIB_DIR}/libboost_system.a )
if( CINDER_LINUX_EGL_RPI2 )
	target_link_libraries( "${EXE_NAME}" stdc++ dl pthread z ${CINDER_GL_LIBS} fontconfig bcm_host )
else()
	target_link_libraries( "${EXE_NAME}" stdc++ dl pthread z ${CINDER_GL_LIBS} X11 Xcursor Xinerama Xrandr Xi fontconfig )
endif()

