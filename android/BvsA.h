#include <jni.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>
 #include <opencv2/core/core.hpp>
 #include <opencv2/imgproc/imgproc.hpp>
#include "lib/include/bvs/bvs.h"

#define LOG_TAG "BvsA/"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))

using namespace cv;

Mat * javaMat;
jobject  javaObj;



	
BVS::BVS* bvs;




/** Shutdown function, called by BVS when shutdown requested. */
void shutdownFunction();

extern "C" void drawToAndroid();

Mat* getNativeAddress();