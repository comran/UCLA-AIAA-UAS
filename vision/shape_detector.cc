#include "shape_detector.h"

#include <iostream>
#include <vector>
#include <cmath>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

namespace vision {
namespace shape_detector {

ShapeDetector::ShapeDetector() {}

// Find shapes in the given image and trace them out in the frame.
void ShapeDetector::ProcessImage(
    ::cv::Mat frame, ::std::vector<::std::vector<::cv::Point>> &shapes) {
  ::cv::Mat filtered_frame[3];

  for (int i = 0; i < 3; i++) {
    filtered_frame[i] = Threshold(frame, i);

    ::std::vector<::std::vector<::cv::Point>> channel_contours;
    ::std::vector<::cv::Vec4i> channel_hierarchy;
    ::cv::findContours(filtered_frame[i], channel_contours, channel_hierarchy,
                       CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

    for (size_t i = 0; i < channel_contours.size(); i++) {
      ::std::vector<::cv::Point> contour = channel_contours.at(i);

      if (!ApproveContour(contour)) continue;

      ::cv::approxPolyDP(contour, contour, 2, true);
      shapes.push_back(contour);
    }
  }

  OutlineContours(frame, shapes);
}

// Trace out the edges in a colored image for a specific channel.
// blue = 0, green = 1, red = 2
::cv::Mat ShapeDetector::Threshold(::cv::Mat frame, int channel) {
  ::cv::Mat filtered_frame = ::cv::Mat::zeros(frame.rows, frame.cols, CV_8UC3);
  ::cv::bilateralFilter(frame, filtered_frame, 3, 75, 75);
  ::cv::extractChannel(frame, filtered_frame, channel);

  ::cv::blur(filtered_frame, filtered_frame, ::cv::Size(3, 3));
  ::cv::Canny(filtered_frame, filtered_frame, 50, 100, 3);

  return filtered_frame;
}

// Returns whether a contours is good or not based off some sort of filter.
bool ShapeDetector::ApproveContour(::std::vector<::cv::Point> contour) {
  double area = ::cv::contourArea(contour, false);

  return !(area < 80 || area > 10000);
}

// Trace out contours on the given image.
void ShapeDetector::OutlineContours(
    ::cv::Mat &frame, ::std::vector<::std::vector<::cv::Point>> &contours) {
  for (size_t i = 0; i < contours.size(); i++) {
    ::cv::Scalar color(0, 0, 255);

    for (size_t j = 0; j < contours.at(i).size(); j++) {
      ::cv::Point from = contours.at(i).at(j);
      ::cv::Point to = contours.at(i).at((j + 1) % contours.at(i).size());

      ::cv::line(frame, from, to, color, 4);
    }
  }
}

}  // namespace shape_detector
}  // namespace vision
