#ifndef BvsA_H
#define BvsA_H

#include <jni.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
static cv::Mat* javaMat;



/** Shutdown function, called by BVS when shutdown requested. */
void shutdownFunction();

extern "C" void drawToAndroid();

cv::Mat* getNativeAddress();
#endif //BvsA_H
