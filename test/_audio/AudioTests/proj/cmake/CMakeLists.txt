cmake_minimum_required( VERSION 3.16 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( AudioTests )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../.." ABSOLUTE )
get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

set( SRC_FILES
	${APP_PATH}/src/AudioTestsApp.cpp
	${APP_PATH}/src/DeviceTest.cpp
	${APP_PATH}/src/DeviceTest.h
	${APP_PATH}/src/NodeBasicTest.cpp
	${APP_PATH}/src/NodeBasicTest.h
	${APP_PATH}/src/NodeEffectsTest.cpp
	${APP_PATH}/src/NodeEffectsTest.h
	${APP_PATH}/src/ParamTest.cpp
	${APP_PATH}/src/ParamTest.h
	${APP_PATH}/src/SamplePlayerTest.cpp
	${APP_PATH}/src/SamplePlayerTest.h
	${APP_PATH}/src/SpectralTest.cpp
	${APP_PATH}/src/SpectralTest.h
	${APP_PATH}/src/StressTest.cpp
	${APP_PATH}/src/StressTest.h
	${APP_PATH}/src/VoiceTest.cpp
	${APP_PATH}/src/VoiceTest.h
	${APP_PATH}/src/WaveTableTest.cpp
	${APP_PATH}/src/WaveTableTest.h
	${CINDER_PATH}/samples/_audio/common/AudioDrawUtils.cpp
	${CINDER_PATH}/samples/_audio/common/AudioDrawUtils.h
)

ci_make_app(
	APP_NAME    "AudioTests"
	CINDER_PATH ${CINDER_PATH}
	SOURCES     ${SRC_FILES}
)
