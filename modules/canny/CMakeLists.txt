project(CANNY)

add_library(canny MODULE canny.cc)
set(OpenCV_LIBS opencv_core opencv_imgproc opencv_calib3d opencv_video opencv_features2d opencv_ml opencv_highgui opencv_objdetect opencv_contrib opencv_legacy opencv_gpu)
target_link_libraries(canny bvs ${OpenCV_LIBS})
