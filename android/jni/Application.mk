# In NDK r8e, we have to use GCC 4.7 for C++11 features like thread, mutex, etc. 
#  The default GCC is 4.6, which does NOT define macro _GLIBCXX_HAS_GTHREADS, 
#  so you’ll get errors like "mutex in namespace std does not name a type" etc. 
NDK_TOOLCHAIN_VERSION := 4.7

# GNU STL implements most C++11 features. Use either gnustl_static or gnustl_shared 
#  Without this your C++ code will not be able to access headers like <thread>, <mutex> 

APP_STL := gnustl_shared

# Use advanced ARM ABI 
APP_ABI := armeabi-v7a

APP_CPPFLAGS := -frtti -fexceptions
APP_PLATFORM := android-8
