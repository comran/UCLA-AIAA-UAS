#include "shape_detector.h"

#include <iostream>
#include <vector>
#include <cmath>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

namespace y2016 {
namespace shape_detector {

void show_frame(const ::std::string window_name, ::cv::Mat &frame) {
  ::cv::namedWindow(window_name);
  ::cv::imshow(window_name, frame);
}

ShapeDetector::ShapeDetector() {}

::std::vector<::std::vector<::cv::Point>> ShapeDetector::ProcessImage(
    ::cv::Mat frame) {
  ::cv::Mat filtered_frame[3];
  ::std::vector<::std::vector<::cv::Point>> contours;
  ::std::vector<::cv::Vec4i> hierarchy;

  ::std::vector<::std::vector<::cv::Point>> contour_estimates;

  for (int i = 0; i < 3; i++) {
    filtered_frame[i] = Threshold(frame, i);
    ::cv::findContours(filtered_frame[i], contours, hierarchy, CV_RETR_CCOMP,
                       CV_CHAIN_APPROX_SIMPLE);

    for (size_t i = 0; i < contours.size(); i++) {
      ::std::vector<::cv::Point> contour_estimate = contours.at(i);

      ::cv::approxPolyDP(contour_estimate, contour_estimate, 2, true);

      /*double area = ::cv::contourArea(contour_estimate, false);
      if (area < 80 || area > 10000) continue;*/

      contour_estimates.push_back(contour_estimate);
    }
  }

  return contour_estimates;
}

::cv::Mat ShapeDetector::Threshold(::cv::Mat frame, int channel) {
  ::cv::Mat filtered_frame = ::cv::Mat::zeros(frame.rows, frame.cols, CV_8UC3);
  ::cv::bilateralFilter(frame, filtered_frame, 3, 75, 75);
  ::cv::extractChannel(frame, filtered_frame, channel);
  show_frame("Filtered #" + ::std::to_string(channel), filtered_frame);

  ::cv::blur(filtered_frame, filtered_frame, ::cv::Size(3, 3));
  ::cv::Canny(filtered_frame, filtered_frame, 50, 100, 3);

  return filtered_frame;
}

}  // namespace shape_detector
}  // namespace y2016
