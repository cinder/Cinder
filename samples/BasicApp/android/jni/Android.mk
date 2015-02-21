LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libcinder_d
LOCAL_SRC_FILES := ../../../../lib/libcinder_d.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libboost_date_time
LOCAL_SRC_FILES := ../../../../lib/android/libboost_date_time.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libboost_filesystem
LOCAL_SRC_FILES := ../../../../lib/android/libboost_filesystem.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libboost_system
LOCAL_SRC_FILES := ../../../../lib/android/libboost_system.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE           := BasicApp 
LOCAL_SRC_FILES        := ../../src/basicApp.cpp
LOCAL_C_INCLUDES       := ../../../../include ../../../../boost
LOCAL_CPPFLAGS         += -std=c++11 -fexceptions -g -mfpu=neon
LOCAL_LDLIBS           := -llog -landroid -lEGL -lGLESv3 -lz
LOCAL_STATIC_LIBRARIES := libcinder_d libboost_date_time libboost_filesystem libboost_system
include $(BUILD_SHARED_LIBRARY)

#$(call import-module,ffmpeg/android/arm)
#$(call import-module,android/native_app_glue)

