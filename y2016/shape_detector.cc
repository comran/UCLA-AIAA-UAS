#include "shape_detector.h"

#include <iostream>
#include <vector>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

namespace y2016 {
namespace shape_detector {
ShapeDetector::ShapeDetector() {}

::std::vector<::std::vector<::cv::Point>> ShapeDetector::ProcessImage(
    ::cv::Mat frame) {
  ::cv::Mat filtered_frame = Threshold(frame);

  ::std::vector<::std::vector<::cv::Point>> contours;
  ::std::vector<::cv::Vec4i> hierarchy;

  ::cv::findContours(filtered_frame, contours, hierarchy, CV_RETR_CCOMP,
                     CV_CHAIN_APPROX_SIMPLE);

  ::std::vector<::std::vector<::cv::Point>> contour_estimates;

  for (size_t i = 0; i < contours.size(); i++) {
    double perimeter = ::cv::arcLength(contours.at(i), true);
    ::std::vector<::cv::Point> contour_estimate;

    ::cv::approxPolyDP(contours.at(i), contour_estimate, perimeter * 0.02,
                       true);
    contour_estimates.push_back(contour_estimate);
  }

  return contour_estimates;
}

::cv::Mat ShapeDetector::Threshold(::cv::Mat frame) {
  ::cv::Mat filtered_frame = ::cv::Mat::zeros(frame.rows, frame.cols, CV_8UC3);
  ::cv::extractChannel(frame, filtered_frame, 0);
  ::cv::Canny(filtered_frame, filtered_frame, 100, 100 * 2, 3);

  return filtered_frame;
}

}  // namespace shape_detector
}  // namespace y2016
