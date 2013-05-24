Java JDK
sudo apt-get install openjdk-6-jre icedtea6-plugin 
sudo apt-get install openjdk-6-jdk openjdk-6-source openjdk-6-jre-headless openjdk-6-jre-lib 
sudo apt-get install ant

If you are running on 64 Bit system:
sudo apt-get install ia32-libs


Download SDK Tools only
http://dl.google.com/android/android-sdk_r22-linux.tgz

Download NDK
http://dl.google.com/android/ndk/android-ndk-r8e-linux-x86_64.tar.bz2

Download OpenCV for Android
http://sourceforge.net/projects/opencvlibrary/files/opencv-android/2.4.5/OpenCV-2.4.5-android-sdk.zip/download

Download Framework from GitHub
Download modules

go to downloadlocation, usually ~/Downloads
chmod 777 *
tar -xf android-sdk_r22-linux.tgz
tar -xf android-ndk-r8e-linux-x86_64.tar.bz2
unzip OpenCV-2.4.5-android-sdk.zip
unzip bvs.zip

# Create direcory for android development, for example in home directory

mkdir ~/androidDev

cp -r android-sdk-linux/ ~/androiDev/sdk
cp -r android-ndk-r8e/ ~/androiDev/ndk
cp -r OpenCV-2.4.5-android-sdk/ ~/android/opencv



#create workspace folder, for example in home directory
mkdir ~/workspace
cp -r bvs ~/workspace/bvs

#add OPENCV path

vi ~/workspace/bvs/android/local.opencv.mk
add
LOCAL_OPENCV=~/androidDev/opencv/sdk/native/jni/OpenCV.mk


add sdk toools, ndk build tools and sdk platformtools to PATH
platform tools not yet available, but will be installed on next step
export PATH=$PATH:$HOME/androidDev/sdk/tools:$HOME/androidDev/sdk/platform-tools
:$HOME/androidDev/ndk

Edit the file $(ndk_dir)build/core/build-binary.mk. Change the line
$(cleantarget): PRIVATE_CLEAN_FILES := ($(my)OBJS)
to
$(cleantarget): PRIVATE_CLEAN_FILES := $($(my)OBJS)


start Android SDK Manager
android sdk

Now, you have to check your android version on your device and install

Android SDK Platform-tools
Android SDK Build-tools

And for your android version or newest if running on emulator
SDK Platform
ARM EABI v7a System IMage

Extras
Android support library

connect your device, or create emulator:
device: 
got to settings and activate developer mode

emulator
android avd 
New
type a name
Device: for example 4.0" WVGA 480x800
Back Camera: Webcam 
ok

start new created emulator
emulator -avd nameOfEmulator


Install OpenCV Manager on emulator
go to opencv folder
cd ~/androidDev/opencv/apk/
adb install OpenCV_2.4.5_Manager_2.7_armeabi.apk


build open java
cd ~/androidDev/opencv/sdk/java
android update project --target 1 --path .


vi ~/workspace/bvs/android/project.properties
add android.library.reference.1=../../../opencv/sdk/java
no shortcut like $HOME or ~  allowed, PATH HAS TO BE RELATIVE







