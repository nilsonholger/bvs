BVS_OPTIONS:=-DBVS_LOG_SYSTEM
#LOCAL_PATH variable is needed from buildsystem
LOCAL_PATH := $(abspath $(call my-dir)/../../)
BVS_ROOT_PATH := $(LOCAL_PATH)
#get location of local OPENCV.MK from seperated mk
include $(BVS_ROOT_PATH)/android/local.opencv.mk
OPENCVMK:=$(LOCAL_OPENCV)
#BvsA.h need opencv, but we cant link against it, because ...
#so save the LOCAL_C_INCLUDES to a variable and CLEAR all LOCAL_Variable
include $(OPENCVMK)
OPENCV_C_INCLUDES:=$(LOCAL_C_INCULDES)

include $(CLEAR_VARS)

LOCAL_CPPFLAGS := -std=c++11 -pthread -frtti -fexceptions $(BVS_OPTIONS)
APP_USE_CPP0X := true 
LOCAL_ARM_MODE := arm
LOCAL_MODULE    := BvsA


include $(call all-subdir-makefiles)
	
LOCAL_C_INCLUDES :=$(BVS_ROOT_PATH)/lib/include 
LOCAL_C_INCLUDES += /usr/local/include
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

include $(CLEAR_VARS)


#OpenCV
##### Following lines not needed if OpenCV_Manager available on device
#OPENCV_INSTALL_MODULES:=on
#OPENCV_CAMERA_MODULES:=on
#OPENCV_LIB_TYPE:=STATIC
include $(LOCAL_OPENCV)

LOCAL_C_INCLUDES += $(BVS_ROOT_PATH)/lib/include 
LOCAL_CPPFLAGS := -std=c++11 -pthread -frtti -fexceptions $(BVS_OPTIONS)
APP_USE_CPP0X := true 

LOCAL_ARM_MODE := arm
# --llog for logging
LOCAL_LDLIBS  += -llog 
LOCAL_MODULE    := ExampleCV
LOCAL_SRC_FILES := modules/bvs-modules/ExampleCV/ExampleCV.cc

LOCAL_SHARED_LIBRARIES := BvsA

include $(BUILD_SHARED_LIBRARY)

#AndroidCV
include $(CLEAR_VARS)

#OpenCV
##### Following line not needed if OpenCV_Manager available on device
#OPENCV_LIB_TYPE:=STATIC
include $(LOCAL_OPENCV)

LOCAL_C_INCLUDES += $(BVS_ROOT_PATH)/lib/include 
LOCAL_C_INCLUDES += $(BVS_ROOT_PATH)/android
LOCAL_CPPFLAGS := -std=c++11 -pthread -frtti -fexceptions $(BVS_OPTIONS)
APP_USE_CPP0X := true 

LOCAL_ARM_MODE := arm

LOCAL_LDLIBS  += -llog 
LOCAL_MODULE    := AndroidCV
LOCAL_SRC_FILES := modules/bvs-modules/AndroidCV/AndroidCV.cc
LOCAL_SHARED_LIBRARIES := BvsA

include $(BUILD_SHARED_LIBRARY)
