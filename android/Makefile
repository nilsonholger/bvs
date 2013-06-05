all:
	ndk-build V=1
	android update project --target android-17 --path . --name BvsA
	ant debug

install: all
	adb shell am force-stop com.cvhci.bvsa
	adb install -r bin/BvsA-debug.apk
#	adb shell am start -n com.cvhci.bvsa/com.cvhci.bvsa.MainActivity

debug: install
	bash -c "trap 'adb shell am force-stop com.cvhci.bvsa' INT TERM;adb shell am start -n com.cvhci.bvsa/com.cvhci.bvsa.MainActivity;adb logcat"
	
clean:
	-ndk-build clean
	ndk-build clean
