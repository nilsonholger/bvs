#include <jni.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>
 #include <opencv2/core/core.hpp>
 #include <opencv2/imgproc/imgproc.hpp>
#include "lib/include/bvs/bvs.h"
cv::Mat * javaMat;
jobject  javaObj;



	
BVS::BVS* bvs;




/** Shutdown function, called by BVS when shutdown requested. */
void shutdownFunction();

extern "C" void drawToAndroid();

cv::Mat* getNativeAddress();
