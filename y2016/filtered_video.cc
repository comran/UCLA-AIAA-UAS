#include <iostream>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"

void show_frame(const ::std::string window_name, ::cv::Mat &frame) {
  ::cv::namedWindow(window_name);
  ::cv::imshow(window_name, frame);
}

int main(int, char **) {
  ::cv::VideoCapture camera(0);
  while (true) {
    ::cv::Mat original_frame;
    camera >> original_frame;
    ::cv::Mat filtered_frame = ::cv::Mat::zeros(original_frame.rows,
                                                original_frame.cols, CV_8UC3);

    show_frame("Original frame", original_frame);

    // Convert the original image into grayscale and do thresholding.
    ::cv::cvtColor(original_frame, filtered_frame, CV_BGR2GRAY);
    show_frame("Black and white frame", filtered_frame);
    ::cv::threshold(filtered_frame, filtered_frame, 110, 255, CV_THRESH_BINARY);
    show_frame("Thresholded frame", filtered_frame);

    ::cv::waitKey(10);
  }

  return 0;
}
