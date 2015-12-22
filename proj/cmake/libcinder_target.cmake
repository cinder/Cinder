cmake_minimum_required( VERSION 3.0 FATAL_ERROR )

# note: CINDER_TARGET_SUBFOLDER is defined by each platform config, to be a folder that lives in cinder/lib/*
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/${CINDER_TARGET_SUBFOLDER}/${CMAKE_BUILD_TYPE} )

message( "CMAKE_ARCHIVE_OUTPUT_DIRECTORY: ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}" )

add_library(
    cinder STATIC
    ${CINDER_SRC_FILES}
)

target_include_directories( cinder BEFORE PUBLIC ${CINDER_INCLUDE_USER} )
target_include_directories( cinder SYSTEM BEFORE PUBLIC ${CINDER_INCLUDE_SYSTEM} )

target_compile_definitions( cinder PUBLIC ${CINDER_DEFINES} )
