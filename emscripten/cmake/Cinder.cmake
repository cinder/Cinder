
# Assume Debug if a build type isn't specified
if( "" STREQUAL "${CMAKE_BUILD_TYPE}" )
    set( CMAKE_BUILD_TYPE "Debug" CACHE FILEPATH "" FORCE )
endif()

### Force some Emscripten CMake variables. CMake seems to be really particular
### about when these vars show up. They need to come after project.
set( CMAKE_AR 							"emcc" CACHE FILEPATH "" FORCE)
set( CMAKE_STATIC_LIBRARY_SUFFIX 		".bc" )
set( CMAKE_C_CREATE_STATIC_LIBRARY 		"<CMAKE_AR> -o <TARGET> <LINK_FLAGS> <OBJECTS>" )
set( CMAKE_CXX_CREATE_STATIC_LIBRARY 	"<CMAKE_AR> -o <TARGET> <LINK_FLAGS> <OBJECTS>" )

# set basic flags for Emscripten
set( CINDER_GL_ES_FLAGS 	"-DCINDER_GL_ES_3 -s USE_WEBGL2=1 -s FULL_ES3=1" )

# C Flags
set( C_FLAGS "-fvisibility=default -D_UNIX ${CINDER_GL_ES_FLAGS} -s USE_GLFW=3 -s NO_EXIT_RUNTIME=1 -s DISABLE_EXCEPTION_CATCHING=0" )
set( CMAKE_C_FLAGS_DEBUG   "${C_FLAGS} -g -O0" )
set( CMAKE_C_FLAGS_RELEASE "${C_FLAGS} -Os" )

# CPP Flags
set( CXX_FLAGS "-stdlib=libc++ -std=c++11 -fvisibility=default -D_UNIX ${CINDER_GL_ES_FLAGS} -s USE_GLFW=3 -s DEMANGLE_SUPPORT=1 -s DISABLE_EXCEPTION_CATCHING=0" )
set( CMAKE_CXX_FLAGS_DEBUG    "${CXX_FLAGS} -g -O0 -fexceptions -frtti" )
set( CMAKE_CXX_FLAGS_RELEASE  "${CXX_FLAGS} -Os -fexceptions -frtti")

get_filename_component( CINDER_INC_DIR "${CINDER_DIR}/include" ABSOLUTE )
get_filename_component( CINDER_LIB_DIR "${CINDER_DIR}/lib"     ABSOLUTE )

# we need an externs file to get around closure compiler mangling names(or is it UglifyJS ? I see references to both!¯\_(ツ)_/¯ )
list(APPEND EMCC_CLOSURE_ARGS "--externs ${CINDER_INC_DIR}/cinder/emscripten/externs.js")
################### HELPER VARIABLES ##################################

set(ALLOW_MEMORY_GROWTH "-s ALLOW_MEMORY_GROWTH=1")

# set variable for helper file when handling DOM related stuff. This should be a part of the --pre-js Emscripten flag
set( CINDER_JS_HELPERS "--pre-js ${CINDER_INC_DIR}/cinder/emscripten/helpers.js")

set( CINDER_VIDEO "${CINDER_JS_HELPERS} ${CINDER_USE_EMBIND}")

# add to your project flags to build your file as a WebWorker.
# note that you apparently need the --bind flag when building workers
set( BUILD_AS_WORKER "-s BUILD_AS_WORKER=1")

# the flag to set when you want to include an assets folder and it's contents.
# this assumes that your assets directory is one directory behind. Note that
# this may not work for samples
set( INCLUDE_RESOURCES_FOLDER "--preload-file ../resources@")

# simple function to allow you to re-set the resources path - just pass in a new path relative to your build directory
function(SET_RESOURCES_PATH path)
set( INCLUDE_RESOURCES_FOLDER "--preload-file ${path}@" PARENT_SCOPE)
endfunction()

# this flag tells Emscripten to use the browser to decode media assets whenever possible, when used
# in conjunction with a related API function.
set( USE_BROWSER_FOR_DECODING "--use-preload-plugins")

# adds optimizations to the final output
set( ADD_OPTIMIZATIONS "-s WASM=1 -Os -g0 -closure 1")

# enables use of pthreads
set(USE_THREADS "-s USE_PTHREADS=1")


# note - release command would be
# emcmake cmake .. -DCMAKE_BUILD_TYPE=Release
if( "Debug" STREQUAL "${CMAKE_BUILD_TYPE}" )
    set( CINDER_LIB_SUFFIX "_d" CACHE FILEPATH "" FORCE )
endif()
