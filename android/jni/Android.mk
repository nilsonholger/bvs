#DEBUG WITH $(warning ...)
BVS_OPTIONS:=-DBVS_LOG_SYSTEM
#LOCAL_PATH variable is needed from buildsystem
LOCAL_PATH := $(abspath $(call my-dir)/../../)
BVS_ROOT_PATH := $(LOCAL_PATH)
BVS_MODULES_PATH:= $(BVS_ROOT_PATH)/modules/bvs-modules
$(warning $(BVS_MODULES_PATH)...)

#get location of local OPENCV.MK from seperated mk in variable LOCAL_OPENCV
include $(BVS_ROOT_PATH)/android/local.opencv.mk
#BvsA.h need opencv, but we cant link against it, linked against it twice
#so save the LOCAL_C_INCLUDES to OPENCV_C_INCLUDES  and CLEAR_VARS
include $(LOCAL_OPENCV)
#OPENCV_C_INCLUDES:=$(LOCAL_C_INCLUDES)

OPENCV_C_INCLUDES:=$(OPENCV_LOCAL_C_INCLUDES)
include $(CLEAR_VARS)

LOCAL_CPPFLAGS := -std=c++11 -pthread -frtti -fexceptions $(BVS_OPTIONS)
APP_USE_CPP0X := true 
LOCAL_ARM_MODE := arm
LOCAL_MODULE    := BvsA


include $(call all-subdir-makefiles)
	
LOCAL_C_INCLUDES :=$(BVS_ROOT_PATH)/lib/include 
LOCAL_C_INCLUDES += /usr/local/include
LOCAL_C_INCLUDES += $(OPENCV_C_INCLUDES)
LOCAL_SRC_FILES :=lib/src/barrier.cc
LOCAL_SRC_FILES +=lib/src/bvs.cc
LOCAL_SRC_FILES +=lib/src/config.cc
LOCAL_SRC_FILES +=lib/src/connector.cc
LOCAL_SRC_FILES +=lib/src/control.cc
LOCAL_SRC_FILES +=lib/src/loader.cc
LOCAL_SRC_FILES +=lib/src/droid.cc
LOCAL_SRC_FILES +=lib/src/info.cc
LOCAL_SRC_FILES +=lib/src/logger.cc
LOCAL_SRC_FILES +=lib/src/logsystem.cc
LOCAL_SRC_FILES +=lib/src/module.cc
LOCAL_SRC_FILES +=lib/src/utils.cc

LOCAL_SRC_FILES += android/jni/BvsA.cpp
# --llog for logging
LOCAL_LDLIBS  += -llog 

include $(BUILD_SHARED_LIBRARY)

#ANDROIDCV MODULE
include $(CLEAR_VARS)
include $($(BVS_MODULES_PATH)/AndroidCV/Android.mk)
LOCAL_MODULE    := AndroidCV
include $(BUILD_SHARED_LIBRARY)

#AndroidCV MODULE
include $(CLEAR_VARS)
include $($(BVS_MODULES_PATH)/ExampleCV/ExampleCV.mk)
LOCAL_MODULE    := ExampleCV
include $(BUILD_SHARED_LIBRARY)
