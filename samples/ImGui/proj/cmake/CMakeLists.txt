cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
set(CMAKE_VERBOSE_MAKEFILE ON)

project(ImGui)

# Set path to Cinder relative to this sample
get_filename_component(CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../.." ABSOLUTE)
get_filename_component(APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)

# Include Cinder's CMake configuration
include("${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake")

# Set source files
set(SRC_FILES
    ${APP_PATH}/src/ImGuiApp.cpp
)

# Create the executable
ci_make_app(
    SOURCES     ${SRC_FILES}
    CINDER_PATH ${CINDER_PATH}
)