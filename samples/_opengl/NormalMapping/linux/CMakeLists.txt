cmake_minimum_required( VERSION 3.1 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE on )

get_filename_component( CINDER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../.." ABSOLUTE )
include( ${CINDER_DIR}/tools/linux/Cinder.cmake )

project( NormalMapping )

get_filename_component( SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src" ABSOLUTE )
get_filename_component( INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../include" ABSOLUTE )

message( "${BoldGreen}CINDER_INC_DIR: ${CINDER_INC_DIR}${ColorReset}" )
message( "${BoldGreen}CINDER_SRC_DIR: ${CINDER_SRC_DIR}${ColorReset}" )
message( "" )
message( "${BoldGreen}SRC_DIR       : ${SRC_DIR}${ColorReset}" )
message( "" )

include_directories( 
    ${CINDER_INC_DIR} 
    ${CINDER_INC_DIR}/glfw
    ${BOOST_INC_DIR}  
    ${INC_DIR} 
)

set( SRC_FILES
  ${SRC_DIR}/NormalMappingApp.cpp
  ${SRC_DIR}/DebugMesh.cpp
)

set( CMAKE_DEBUG_POSTFIX "_d" )
add_executable( "${CMAKE_PROJECT_NAME}" ${SRC_FILES} )
target_link_libraries( "${CMAKE_PROJECT_NAME}" debug     ${CINDER_DIR}/lib/linux/libcinder_d.a )
target_link_libraries( "${CMAKE_PROJECT_NAME}" optimized ${CINDER_DIR}/lib/linux/libcinder.a )
target_link_libraries( "${CMAKE_PROJECT_NAME}" general ${CINDER_DIR}/lib/linux/libboost_filesystem.a )
target_link_libraries( "${CMAKE_PROJECT_NAME}" general ${CINDER_DIR}/lib/linux/libboost_system.a )
target_link_libraries( "${CMAKE_PROJECT_NAME}" c++ dl pthread z GL X11 Xcursor Xinerama Xrandr Xi )

