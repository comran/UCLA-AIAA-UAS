#include "vision/util/shape_detector.h"

namespace vision {
namespace shape_detector {

ShapeDetector::ShapeDetector() {}

// Find shapes in the given image and trace them out in the frame.
void ShapeDetector::ProcessImage(
    ::cv::Mat &frame, ::std::vector<::std::vector<::cv::Point>> &shapes) {
  ::cv::Mat hexagon_mat = ::cv::imread("./shapes/hexagon.jpg");
  ::cv::Mat hexagon_filtered_frames[3];
  Threshold(hexagon_mat, hexagon_filtered_frames);

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

  ::std::vector<::std::vector<::cv::Point>> hexagon_contours;
  ::std::vector<::cv::Vec4i> hexagon_hierarchy;
  ::cv::findContours(hexagon_filtered_frames[0], hexagon_contours,
                     hexagon_hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

  ::std::cout << shapes.size() << " shapes\n";
  for (size_t i = 0; i < shapes.size(); i++) {
    double area = ::cv::contourArea(shapes[i]);
    if(area < 5) {
      shapes.erase(shapes.begin() + i);
      i--;

      continue;
    }
    double similarity =
        ::cv::matchShapes(shapes[i], hexagon_contours[0], 1, 0.0);

    if (similarity > 0.01) {
      shapes.erase(shapes.begin() + i);
      i--;
    } else {
      ::std::cout << "i: " << i << " area: " << area
                  << " similarity: " << similarity << ::std::endl;
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

    // Blur channel to deal with camera noise.
    ::cv::GaussianBlur(filtered_frames[i], filtered_frames[i], ::cv::Size(3, 3),
                       0, 0);

    // Dynamic canny edge filter with automatic threshold values.
    ::cv::Mat temp_frame;  // Required for ::cv::threshold to work.
    double high_canny_threshold =
        ::cv::threshold(filtered_frames[i], temp_frame, 0, 255,
                        CV_THRESH_BINARY | CV_THRESH_OTSU);
    temp_frame.release();
    ::cv::Canny(filtered_frames[i], filtered_frames[i], high_canny_threshold,
                0.5 * high_canny_threshold);
    ::cv::dilate(filtered_frames[i], filtered_frames[i], ::cv::Mat(),
                 ::cv::Point(-1, -1));

    // Display the channel edges for debugging.
    ::std::string window_name = "Threshold channel ";
    window_name += channels[i];
    ::cv::namedWindow(window_name);
    ::cv::moveWindow(window_name, filtered_frames[0].cols,
                     filtered_frames[0].rows * i);
    ::cv::imshow(window_name, filtered_frames[i]);
  }
}

// Returns whether a contour is good or not based off some sort of filter.
bool ShapeDetector::ApproveContour(::std::vector<::cv::Point> contour) {
  return true;  // Show all contours for debugging purposes.

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
