cmake_minimum_required( VERSION 2.8 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( UnitTests )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../.." ABSOLUTE )
get_filename_component( UNIT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

set( SOURCES
	${UNIT_DIR}/src/Base64Test.cpp
	${UNIT_DIR}/src/FileWatcherTest.cpp
	${UNIT_DIR}/src/JsonTest.cpp
	${UNIT_DIR}/src/ObjLoaderTest.cpp
	${UNIT_DIR}/src/RandTest.cpp
	${UNIT_DIR}/src/SystemTest.cpp
	${UNIT_DIR}/src/ShaderPreprocessorTest.cpp
	${UNIT_DIR}/src/TestMain.cpp
	${UNIT_DIR}/src/UnicodeTest.cpp
	${UNIT_DIR}/src/Utilities.cpp
	${UNIT_DIR}/src/Path2dTest.cpp
	${UNIT_DIR}/src/PolyLineTest.cpp
	${UNIT_DIR}/src/audio/BufferUnit.cpp
	${UNIT_DIR}/src/audio/FftUnit.cpp
	${UNIT_DIR}/src/audio/RingBufferUnit.cpp
	${UNIT_DIR}/src/signals/SignalsTest.cpp
)

ci_make_app(
	SOURCES     ${SOURCES}
	CINDER_PATH ${CINDER_PATH}
	INCLUDES    "${UNIT_DIR}/src"    # for catch.hpp
)

if( APPLE )
	set( TESTBINDIR ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/UnitTests/UnitTests.app/Contents/MacOS )
else()
	set( TESTBINDIR ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/UnitTests )
endif()

add_test(
	NAME UnitTests
	COMMAND ${TESTBINDIR}/UnitTests
)
