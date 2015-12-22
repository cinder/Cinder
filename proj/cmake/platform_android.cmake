cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

set( CINDER_PLATFORM "Android" )

set( CINDER_ANDROID_NDK_PLATFORM 21 )
set( CINDER_ANDROID_NDK_ARCH "armeabi-v7a" )

set( CINDER_TARGET_SUBFOLDER "android-${CINDER_ANDROID_NDK_PLATFORM/CINDER_ANDROID_NDK_ARCH" )

include( ${CINDER_CMAKE_DIR}/libcinder_configure_build.cmake )
include( ${CINDER_CMAKE_DIR}/libcinder_source_files.cmake )

list( APPEND SRC_SET_CINDER_ANDROID
	${CINDER_SRC_DIR}/cinder/CaptureImplJni.cpp
	${CINDER_SRC_DIR}/cinder/UrlImplJni.cpp
)

list( APPEND SRC_SET_CINDER_APP_ANDROID
	${CINDER_SRC_DIR}/cinder/app/android/android_native_app_glue.c
	${CINDER_SRC_DIR}/cinder/app/android/AppAndroid.cpp
	${CINDER_SRC_DIR}/cinder/app/android/AppImplAndroid.cpp
	${CINDER_SRC_DIR}/cinder/app/android/AssetFileSystem.cpp
	${CINDER_SRC_DIR}/cinder/app/android/EventManagerAndroid.cpp
	${CINDER_SRC_DIR}/cinder/app/android/PlatformAndroid.cpp
	${CINDER_SRC_DIR}/cinder/app/android/Renderer2dAndroid.cpp
	${CINDER_SRC_DIR}/cinder/app/android/RendererGlAndroid.cpp
	${CINDER_SRC_DIR}/cinder/app/android/WindowImplAndroid.cpp
)

list( APPEND CINDER_SRC_FILES
	${SRC_SET_CINDER_ANDROID}
	${SRC_SET_CINDER_APP_ANDROID}
)

source_group( "cinder\\android"         FILES ${SRC_SET_CINDER_ANDROID} )
source_group( "cinder\\app\\android"    FILES ${SRC_SET_CINDER_APP_ANDROID} )

include( ${CINDER_CMAKE_DIR}/libcinder_target.cmake )