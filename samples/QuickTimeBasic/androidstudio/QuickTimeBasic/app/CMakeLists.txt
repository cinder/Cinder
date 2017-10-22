cmake_minimum_required(VERSION 3.4.1)

add_subdirectory(${CINDER_DIR}/proj/android ./buildCinder)

add_library( QuickTimeBasic SHARED
    ../../../src/QuickTimeBasicApp.cpp
)

target_include_directories( QuickTimeBasic PRIVATE
    ${CINDER_DIR}/include
    ${CINDER_DIR}/include
    ../../../include
)


target_link_libraries( QuickTimeBasic
    cinder
    ${CINDER_DIR}/lib/android/android-${NDK_PLATFORM}/${ANDROID_ABI}/libboost_system.a
    ${CINDER_DIR}/lib/android/android-${NDK_PLATFORM}/${ANDROID_ABI}/libboost_filesystem.a
    log android EGL GLESv3 OpenSLES z atomic
)
