cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

set( CINDER_PLATFORM "Emscripten" )

list( APPEND SRC_SET_CINDER_APP_EMSCRIPTEN
	${CINDER_SRC_DIR}/cinder/app/emscripten/AppEmscripten.cpp
	${CINDER_SRC_DIR}/cinder/app/emscripten/AppImplEmscripten.cpp
	${CINDER_SRC_DIR}/cinder/app/emscripten/PlatformEmscripten.cpp
)

list( APPEND SRC_SET_CINDER_EMSCRIPTEN
	${CINDER_SRC_DIR}/cinder/linux/gl_es_load.cpp
	${CINDER_SRC_DIR}/cinder/app/emscripten/RendererImplGlEmscripten.cpp
	${CINDER_SRC_DIR}/cinder/app/emscripten/WindowImplEmscripten.cpp
	${CINDER_SRC_DIR}/cinder/emscripten/EmscriptenVideo.cpp
	#${CINDER_SRC_DIR}/cinder/audio/emscripten/ContextWebAudio.cpp
	${CINDER_SRC_DIR}/cinder/emscripten/globalbindings.cpp
)

list( APPEND CINDER_SRC_FILES
	${SRC_SET_CINDER_APP_EMSCRIPTEN}
	${SRC_SET_CINDER_EMSCRIPTEN}
)

set( CINDER_EMSCRIPTEN_LINK_FLAGS "-s ERROR_ON_UNDEFINED_SYMBOLS=0 -s USE_WEBGL2=1 -s FULL_ES3=1 -s USE_GLFW=3 -s DISABLE_EXCEPTION_CATCHING=0 -s DEMANGLE_SUPPORT=1" )
list( APPEND CINDER_CXX_FLAGS -stdlib=c++ ) 
list( APPEND CINDER_DEFINES "-DGLFW_INCLUDE_ES3;-DCINDER_EMSCRIPTEN" )

list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE
	${CINDER_SRC_DIR}/zlib
)

set( EMSCRIPTEN_LIB_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../../lib/emscripten/" )

################### HELPER VARIABLES ##################################
set(ALLOW_MEMORY_GROWTH "-s ALLOW_MEMORY_GROWTH=1" )

# add to your project flags to build your file as a WebWorker.
# note that you apparently need the --bind flag when building workers
set( BUILD_AS_WORKER "-s BUILD_AS_WORKER=1" )

# the flag to set when you want to include an assets folder and it's contents.
# this assumes that your assets directory is one directory behind. Note that
# this may not work for samples
set( INCLUDE_RESOURCES_FOLDER "--preload-file ../resources@" )

# simple function to allow you to re-set the resources path - just pass in a new path relative to your build directory
function(SET_RESOURCES_PATH path )
set( INCLUDE_RESOURCES_FOLDER "--preload-file ${path}@" PARENT_SCOPE )
endfunction()

# this flag tells Emscripten to use the browser to decode media assets whenever possible, when used
# in conjunction with a related API function.
set( USE_BROWSER_FOR_DECODING "--use-preload-plugins" )

# adds optimizations to the final output
set( ADD_OPTIMIZATIONS "-s WASM=1 -Os -g0 --closure 1" )

# enables use of pthreads
set( USE_THREADS "-s USE_PTHREADS=1" )
