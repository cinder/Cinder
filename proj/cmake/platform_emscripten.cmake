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

list( APPEND CINDER_EMSCRIPTEN_LINK_FLAGS "-s USE_WEBGL2=1 -s FULL_ES3=1 -s USE_GLFW=3 -s DISABLE_EXCEPTION_CATCHING=0 -s DEMANGLE_SUPPORT=1" )
list( APPEND CINDER_CXX_FLAGS -stdlib=c++ ) 
list( APPEND CINDER_DEFINES "-DGLFW_INCLUDE_ES3 -DCINDER_EMSCRIPTEN" )

list( APPEND CINDER_INCLUDE_SYSTEM_PRIVATE
	${CINDER_INC_DIR}/msw/zlib
)
