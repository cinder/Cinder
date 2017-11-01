cmake_minimum_required(VERSION 3.4.1)

add_subdirectory(${CINDER_DIR}/proj/android ${CINDER_DIR}/buildCinderForAndroid)

add_library( ImmediateMode SHARED
		../../../src/ImmediateModeApp.cpp
)

target_include_directories( ImmediateMode PRIVATE
    ${CINDER_DIR}/include
    ../../../include
)


target_link_libraries( ImmediateMode
    cinder
    ${CINDER_DIR}/lib/android/android-${NDK_PLATFORM}/${ANDROID_ABI}/libboost_system.a
    ${CINDER_DIR}/lib/android/android-${NDK_PLATFORM}/${ANDROID_ABI}/libboost_filesystem.a
    log android EGL GLESv3 OpenSLES z atomic
)
