cmake_minimum_required( VERSION 3.1 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE on )

get_filename_component( CINDER_DIR "${CMAKE_CURRENT_SOURCE_DIR}../../../../../../" ABSOLUTE )

set(ASYNC_FUNCTIONS true)

# For Emscripten we need to do this again since project() resets some vars .
include( ${CINDER_DIR}/emscripten/cmake/Cinder.cmake )


project( WebAudioApp )

set( SRC_FILES
	../src/WebAudioApp.cpp
	../../EmscriptenAudio/ContextWebAudio.cpp
	../../EmscriptenAudio/Voice.cpp
)

get_filename_component( CINDER_INC_DIR "${CINDER_DIR}/include" ABSOLUTE )

set( EXE_NAME ${PROJECT_NAME} )

# For Emscripten we need to do this again since project() resets some vars .
include( ${CINDER_DIR}/emscripten/cmake/Cinder.cmake )

set( CMAKE_EXECUTABLE_SUFFIX ".html" )

# set all other Emscripten flags you might want besides the one's specified in Cinder.cmake
# If dealing with video, remember to copy your video to the output directory.
# The CINDER_VIDEO variable is coming from the Cinder.camke file in <Cinder root>/emscripten/cmake
#set( CMAKE_EXE_LINKER_FLAGS  "${CINDER_VIDEO} ${INCLUDE_AUDIO} ${CINDER_ASYNC} -s EMTERPRETIFY_WHITELIST=@../../src/whitelist.json -s ASSERTIONS=2 -Os --js-library ../../src/library.js --bind " )
#set( CMAKE_EXE_LINKER_FLAGS  "${CINDER_ASYNC} ${CINDER_JS_HELPERS} -s ASSERTIONS=2 -profiling --js-library ../../src/library.js --bind -s ABORTING_MALLOC=0" )
set( CMAKE_EXE_LINKER_FLAGS  "${CINDER_JS_HELPERS} -s ASSERTIONS=2 -profiling ${CINDER_JS_HELPERS} --bind -s ABORTING_MALLOC=0" )

# note - setting target name to "index" so Cmake will spit out "index.html" and we
# can then just navigate directly to the page.
add_executable(index ${SRC_FILES} )

target_include_directories(
  index
	PUBLIC ${CINDER_INC_DIR}
	PUBLIC ../../EmscriptenAudio
)

target_link_libraries(
  index
	${CINDER_DIR}/lib/emscripten/libboost_filesystem.bc
	${CINDER_DIR}/lib/emscripten/libboost_system.bc
	${CINDER_DIR}/lib/emscripten/libcinder_d.bc
)
