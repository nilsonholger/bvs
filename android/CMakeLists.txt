cmake_minimum_required(VERSION 2.8.6)

project(BVSDROID)

# ANDROID MAIN TARGET (OUTPUT LOCATED IN $SOURCE_DIR)
add_custom_target(android
	COMMAND ndk-build V=1
	COMMAND android update project --target 2 --path . --name BvsA
	COMMAND ant debug
	COMMAND adb shell am force-stop com.cvhci.bvsa
	COMMAND adb install -r bin/BvsA-debug.apk
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "Building Android App!" VERBATIM)

# ANDROID DEBUG TARGET (BUILDS AND RUNS APP IN DEBUG MODE ON DEVICE OR EMULATOR)
add_custom_target(android-debug
	COMMAND bash -c "trap 'adb shell am force-stop com.cvhci.bvsa' INT TERM;adb shell am start -n com.cvhci.bvsa/com.cvhci.bvsa.MainActivity;adb logcat"
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "Debugging Android App!" VERBATIM)
add_dependencies(android-debug android)

# ANDROID CLEAN TARGET
add_custom_target(android-clean
	COMMAND ndk-build clean
	COMMAND ndk-build clean
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "Cleaning Android App!" VERBATIM)
