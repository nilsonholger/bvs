LOCAL_PATH := $(abspath $(call my-dir)/../../)

include $(CLEAR_VARS)

LOCAL_CPPFLAGS := -std=c++11 -pthread -frtti -fexceptions
APP_USE_CPP0X := true 
LOCAL_ARM_MODE := arm
LOCAL_MODULE    := BvsA


include $(call all-subdir-makefiles)
	
LOCAL_C_INCLUDES := $(LOCAL_PATH)/lib/include 
LOCAL_C_INCLUDES += /usr/local/include
LOCAL_SRC_FILES :=lib/src/barrier.cc
LOCAL_SRC_FILES +=lib/src/bvs.cc
LOCAL_SRC_FILES +=lib/src/config.cc
LOCAL_SRC_FILES +=lib/src/connector.cc
LOCAL_SRC_FILES +=lib/src/control.cc
LOCAL_SRC_FILES +=lib/src/info.cc
LOCAL_SRC_FILES +=lib/src/loader.cc
LOCAL_SRC_FILES +=lib/src/logger.cc
LOCAL_SRC_FILES +=lib/src/logsystem.cc
LOCAL_SRC_FILES +=lib/src/module.cc
LOCAL_SRC_FILES +=lib/src/utils.cc

LOCAL_SRC_FILES += android/jni/BvsA.cpp

LOCAL_LDLIBS  += -llog -ldl 
#LOCAL_LDLIBS += $(LOCAL_PATH)/../../opencvsdk/native/libs/armeabi-v7a/libopencv_core.a -L/home/bsigrist/local/lib/libopencv_core.so

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)


#OpenCV
#OPENCV_INSTALL_MODULES:=on
OPENCV_CAMERA_MODULES:=on
OPENCV_LIB_TYPE:=STATIC
include $(LOCAL_PATH)/../opencvsdk/native/jni/OpenCV.mk

LOCAL_C_INCLUDES += $(LOCAL_PATH)/lib/include 
LOCAL_CPPFLAGS := -std=c++11 -pthread -frtti -fexceptions
APP_USE_CPP0X := true 

LOCAL_ARM_MODE := arm

LOCAL_MODULE    := ExampleCV
LOCAL_SRC_FILES := modules/bvsModules/ExampleCV/ExampleCV.cc

LOCAL_SHARED_LIBRARIES := BvsA

include $(BUILD_SHARED_LIBRARY)

#AndroidCV
include $(CLEAR_VARS)

#OpenCV
OPENCV_LIB_TYPE:=STATIC
include $(LOCAL_PATH)/../opencvsdk/native/jni/OpenCV.mk

LOCAL_C_INCLUDES += $(LOCAL_PATH)/lib/include 
LOCAL_CPPFLAGS := -std=c++11 -pthread -frtti -fexceptions
APP_USE_CPP0X := true 

LOCAL_ARM_MODE := arm

LOCAL_MODULE    := AndroidCV
LOCAL_SRC_FILES := modules/bvsModules/AndroidCV/AndroidCV.cc
LOCAL_SHARED_LIBRARIES := BvsA

include $(BUILD_SHARED_LIBRARY)
