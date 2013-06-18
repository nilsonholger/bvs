cmake_minimum_required(VERSION 2.8)
project(BvsA)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/config.cmake")
	include("${CMAKE_CURRENT_SOURCE_DIR}/config.cmake")
else()
	message(FATAL_ERROR "\nconfig.cmake NOT FOUND, please copy from config.cmake.sample and modify accordingly!")
endif()

set(ANDROID_STL gnustl_shared CACHE STRING "C++ Runtime" FORCE)
set(ANDROID_STL_FORCE_FEATURES ON)
set(APP_USE_CPP0X true)
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic --export-dynamic" )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frtti -fexceptions -Weffc++ -DBVS_LOG_SYSTEM")

#test for ANDROID and look for stuff not relevant to android.
if(NOT ANDROID)
	#jni is available by default on android
	find_package(JNI REQUIRED)
	include_directories(${JNI_INCLUDE_DIRS})
endif()

add_subdir_lib(../lib/src BvsA SHARED barrier.cc bvs.cc config.cc connector.cc control.cc droid.cc info.cc loader.cc logger.cc logsystem.cc module.cc utils.cc ../../android/jni/BvsA.cpp)
target_link_libraries(BvsA dl log)

add_library(bvs_modules SHARED .)
target_link_libraries(bvs_modules BvsA dl log -Wl,--whole-archive $ENV{BVS_STATIC_MODULES} -Wl,--no-whole-archive -L${OPENCV_LIBRARY_PATH})
set_target_properties(bvs_modules PROPERTIES LINKER_LANGUAGE CXX) 



add_custom_target(android-app
	COMMAND mkdir -p ${CMAKE_CURRENT_SOURCE_DIR}/libs/${ANDROID_ABI}
	COMMAND cp ${LIBRARY_OUTPUT_PATH}/libBvsA.so ${LIBRARY_OUTPUT_PATH}/libbvs_modules.so ${CMAKE_CURRENT_SOURCE_DIR}/libs/${ANDROID_ABI}
	COMMAND cp ${LIBRARY_OUTPUT_PATH}/libgnustl_shared.so ${CMAKE_CURRENT_SOURCE_DIR}/libs/${ANDROID_ABI}
	COMMAND android update project --target android-17 --path . --name BvsA
	COMMAND ant debug
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "Build Android App!" VERBATIM)
add_dependencies(android-app BvsA bvs_modules)

add_custom_target(android-install
	COMMAND adb shell am force-stop com.cvhci.bvsa
	COMMAND adb install -r bin/BvsA-debug.apk
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "Install Android App!" VERBATIM)
add_dependencies(android-install android-app)

add_custom_target(android-debug
	COMMAND adb logcat -c
	COMMAND bash -c "trap 'adb shell am force-stop com.cvhci.bvsa' INT TERM;adb shell am start -n com.cvhci.bvsa/com.cvhci.bvsa.MainActivity;adb logcat"
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "Debug Android App!" VERBATIM)
add_dependencies(android-debug android-install)

add_custom_target(android-clean
	COMMAND ant clean
	COMMAND rm -rf libs build.xml local.properties
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "Clean Android directory!" VERBATIM)
