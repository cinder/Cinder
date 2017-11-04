 

cmake_minimum_required(VERSION 3.4.1) 
 
add_subdirectory(${CINDER_DIR}/proj/android ${CINDER_DIR}/buildCinderForAndroid) 
 
add_library( ShadowMapping SHARED 
		../../../src/ShadowMappingApp.cpp

) 
 
target_include_directories( ShadowMapping PRIVATE 
    ${CINDER_DIR}/include 
    ../../../include 
) 
 
 
target_link_libraries( ShadowMapping 
    cinder 
    ${CINDER_DIR}/lib/android/android-${NDK_PLATFORM}/${ANDROID_ABI}/libboost_system.a 
    ${CINDER_DIR}/lib/android/android-${NDK_PLATFORM}/${ANDROID_ABI}/libboost_filesystem.a 
    log android EGL GLESv3 OpenSLES z atomic 
) 

