cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

add_library(
    cinder STATIC
    ${CINDER_SRC_FILES}
)