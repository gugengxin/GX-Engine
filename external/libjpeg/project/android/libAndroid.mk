LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := gx_libjpeg
LOCAL_MODULE_FILENAME := libjpeg
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libjpeg.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../include $(LOCAL_PATH)/../../include/ard
include $(PREBUILT_STATIC_LIBRARY)