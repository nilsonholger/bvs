all:
	ndk-build V=1
	android update project --target 2 --path . --name BvsA
	ant debug
	adb shell am force-stop com.cvhci.bvsa
	adb install -r bin/BvsA-debug.apk
#	adb shell am start -n com.cvhci.bvsa/com.cvhci.bvsa.MainActivity
debug:	all
	bash -c "trap 'adb shell am force-stop com.cvhci.bvsa' INT TERM;adb shell am start -n com.cvhci.bvsa/com.cvhci.bvsa.MainActivity;adb logcat"
	
