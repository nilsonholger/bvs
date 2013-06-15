# BVS on Android


## REQUIREMENTS

### Java JDK

	sudo apt-get install openjdk-6-jre icedtea6-plugin
	sudo apt-get install openjdk-6-jdk openjdk-6-source openjdk-6-jre-headless openjdk-6-jre-lib
	sudo apt-get install ant


### ia32 shared libraries for use on amd64 and ia64 systems
	sudo apt-get install ia32-libs

### Android Development 
We do not use eclipse so download only

* SDK Tools `wget http://dl.google.com/android/android-sdk_r22-linux.tgz`

* NDK `wget http://dl.google.com/android/ndk/android-ndk-r8e-linux-x86_64.tar.bz2`

* OpenCV for Android http://sourceforge.net/projects/opencvlibrary/files/opencv-android/2.4.5/OpenCV-2.4.5-android-sdk.zip/download

Unpack downloaded file (they have to be executable, so do `chmod 777 filename` if not)
	tar -xf android-sdk_r22-linux.tgz
	tar -xf android-ndk-r8e-linux-x86_64.tar.bz2
	unzip OpenCV-2.4.5-android-sdk.zip

a little clean up
create direcory for android development, for example in home directory and copy android stuff

	mkdir ~/androidDev
	cp -r android-sdk-linux/ ~/androiDev/sdk
	cp -r android-ndk-r8e/ ~/androiDev/ndk
	cp -r OpenCV-2.4.5-android-sdk/ ~/android/opencv


## BVS SETUP
### DOWNLOAD
* BVS Framework from GitHub `wget https://github.com/nilsonholger/bvs/archive/master.zip`

	unzip master.zip
	rm master.zip

* if needed modules `wget https://github.com/nilsonholger/bvs-modules/archive/master.zip`

	unzip master.zip
	rm master.zip


* create workspace folder, for example in home directory

	mkdir ~/workspace
	mv -r bvs-master ~/workspace/bvs
	mv -r bvs-modules-master ~/workspace/bvs/modules/bvs-modules


### CONFIGURATION
The following paths depends on the paths you used before

#### PATH VARIABLE 

sdk toools, ndk build tools and sdk platformtools to PATH 
platform tools not yet available, but will be installed on next step

	export PATH=$PATH:$HOME/androidDev/sdk/tools:$HOME/androidDev/sdk/platform-tools:$HOME/androidDev/ndk


#### OPENCV PATH 
create and open ~/workspace/bvs/android/local.opencv.mk and add

	LOCAL_OPENCV=~/androidDev/opencv/sdk/native/jni/OpenCV.mk


#### INSTALL ANDROID SDK ADDONS

start Android SDK Manager `android sdk`
Now, you have to check your android version on your device and install

###### TOOLS
* Android SDK Platform-tools
* Android SDK Build-tools

And for your android version or use newest version if running on emulator,
for example

###### Android 4.2.2
* SDK Platform
* ARM EABI v7a System IMage

###### Extras
* Android support library

close Android SDK Manager

connect your device, or create emulator:
on device: got to settings and activate developer mode
or
start Android Virtual Device Mangager: `android avd`
	
	New
	type a name
	Device: for example 4.0 WVGA 480x800
	Back Camera: Webcam 

close Android Virtual Device Manager

start new created emulator `emulator -avd nameOfEmulator &`

#### OPENCV LOCATION FOR JAVA

create and open ~/workspace/bvs/android/project.properties
add to `target` your android version, for which you have installed the SDK Platform before, for example `android-17`

	target=android-17
	android.library.reference.1=../../../opencv/sdk/java`
	
ATTENTION!: no shortcut like $HOME or ~  allowed, PATH HAS TO BE RELATIVE


#### OPENCV JAVA ANDROID PROJECT

again add to `target` your android version, for example `android-17`

	cd ~/androidDev/opencv/sdk/java
	android	update project --target android-17 --path .

#### Install OpenCV Manager on device / emulator

	cd ~/androidDev/opencv/apk/
	adb install OpenCV_2.4.5_Manager_2.7_armeabi.apk

#### ATTENTION!
There is a bug in the android-ndk-r8e 
with the path above, our ndk_dir is ~/androidDev/ndk/

Edit the file $(ndk_dir)build/core/build-binary.mk. Change the line
$(cleantarget): PRIVATE_CLEAN_FILES := ($(my)OBJS)
to
$(cleantarget): PRIVATE_CLEAN_FILES := $($(my)OBJS)



Configure CMake and build, e.g.:
cmake -DANDROID_NDK=$HOME/android/ndk -DCMAKE_TOOLCHAIN_FILE=../android/android.toolchain.cmake -DBVS_ANDROID_APP=ON ..
make

additional cmake build targets:
android-app
android-install
android-debug
android-clean
