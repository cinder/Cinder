cmake_minimum_required( VERSION 3.0 FATAL_ERROR )

# TODO: deposit lib files in lib/target/arch folder, this is especially necessary on android
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/${CINDER_TARGET}/${CMAKE_BUILD_TYPE} )

message( "CMAKE_ARCHIVE_OUTPUT_DIRECTORY: ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}" )

add_library(
    cinder STATIC
    ${CINDER_SRC_FILES}
)

target_compile_definitions( cinder PUBLIC ${CINDER_DEFINES} )
