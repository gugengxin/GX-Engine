LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := gx_freetype
LOCAL_MODULE_FILENAME := freetype
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/freetype.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../include
include $(PREBUILT_STATIC_LIBRARY)