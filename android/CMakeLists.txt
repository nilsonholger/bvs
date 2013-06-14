cmake_minimum_required(VERSION 2.8)
project(BvsA)

set(ANDROID_TOOLCHAIN_NAME arm-linux-androideabi-4.7)
set(ANDROID_STL gnustl_shared)
set(ANDROID_ABI armeabi-v7a)
set(ANDROID_NATIVE_API_LEVEL android-14)
set(ANDROID_STL_FORCE_FEATURES ON)

set(BVS_ROOT_PATH ${CMAKE_BINARY_DIR}/../)
message(STATUS "BVS_ROOT_PATH:"${BVS_ROOT_PATH})
set(APP_USE_CPP0X true)
#set(LOCAL_ARM_MODE arm)
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic --export-dynamic" )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frtti -fexceptions -DBVS_LOG_SYSTEM")
#test for ANDROID and look for stuff not relevant to android.
if(NOT ANDROID)
#jni is available by default on android
find_package(JNI REQUIRED)
include_directories(${JNI_INCLUDE_DIRS})
endif()

add_subdir_lib(${BVS_ROOT_PATH}/lib/src BvsA SHARED barrier.cc bvs.cc config.cc connector.cc control.cc droid.cc info.cc loader.cc logger.cc logsystem.cc module.cc utils.cc ../../android/jni/BvsA.cpp)
target_link_libraries(BvsA dl log ${CMAKE_BINARY_DIR}/bin/libgnustl_shared.so)

add_library(bvs_modules SHARED .)
target_link_libraries(bvs_modules BvsA dl log -Wl,--whole-archive $ENV{BVS_STATIC_MODULES} -Wl,--no-whole-archive)



# have to copy the .so file from bvs/bin to bvs/android/libs/armeabi-v7a/
