cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( TestMain )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../.." ABSOLUTE )
get_filename_component( UNIT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

SET(SOURCES
	${UNIT_DIR}/src/Base64Test.cpp
	${UNIT_DIR}/src/JsonTest.cpp
	${UNIT_DIR}/src/ObjLoaderTest.cpp
	${UNIT_DIR}/src/RandTest.cpp
	${UNIT_DIR}/src/SystemTest.cpp
	${UNIT_DIR}/src/TestMain.cpp
	${UNIT_DIR}/src/UnicodeTest.cpp
	${UNIT_DIR}/src/audio/BufferUnit.cpp
	${UNIT_DIR}/src/audio/FftUnit.cpp
	${UNIT_DIR}/src/audio/RingBufferUnit.cpp
	${UNIT_DIR}/src/signals/SignalsTest.cpp
)

ci_make_app(
	SOURCES     ${SOURCES}
	CINDER_PATH ${CINDER_PATH}
	INCLUDES "${UNIT_DIR}/src"    # for catch.hpp
)
TARGET_COMPILE_DEFINITIONS(
	TestMain
	PRIVATE -DUNIT_DIR_ASSETS="${UNIT_DIR}/assets"
)

IF (APPLE)
	SET(TESTBINDIR ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/TestMain.app/Contents/MacOS)
ELSE()
	SET(TESTBINDIR ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
ENDIF()

ADD_TEST(
	NAME TestMain
	COMMAND ${TESTBINDIR}/TestMain
)
