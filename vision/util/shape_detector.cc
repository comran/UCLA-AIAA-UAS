#include "vision/util/shape_detector.h"

namespace vision {
namespace shape_detector {

ShapeDetector::ShapeDetector() {}

// Find shapes in the given image and trace them out in the frame.
void ShapeDetector::ProcessImage(
    ::cv::Mat &frame, ::std::vector<::std::vector<::cv::Point>> &shapes) {
  ::cv::Mat filtered_frames[3];
  Threshold(frame, filtered_frames);

  for (int i = 0; i < 3; i++) {
    ::std::vector<::std::vector<::cv::Point>> channel_contours;
    ::std::vector<::cv::Vec4i> channel_hierarchy;
    ::cv::findContours(filtered_frames[i], channel_contours, channel_hierarchy,
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

// Trace out the edges in a colored image for RGB channels.
// Puts the output edges in filtered_frames, with indexes as follows:
// red = 0, green = 1, blue = 2
void ShapeDetector::Threshold(::cv::Mat &frame, ::cv::Mat *filtered_frames) {
  const char *channels = "RGB";

  for (int i = 0; i < 3; i++) {
    ::cv::extractChannel(frame, filtered_frames[i], 2 - i /* to convert BGR */);

    ::cv::blur(filtered_frames[i], filtered_frames[i], ::cv::Size(3, 3));
    ::cv::Canny(filtered_frames[i], filtered_frames[i], 50, 100, 3);

    ::std::string window_name = "Threshold channel ";
    window_name += channels[i];
    ::cv::namedWindow(window_name);
    ::cv::imshow(window_name, filtered_frames[i]);
  }
}

// Returns whether a contour is good or not based off some sort of filter.
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
