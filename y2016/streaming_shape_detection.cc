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

    ::std::vector<::std::vector<::cv::Point>> contours;
    ::std::vector<::cv::Vec4i> hierarchy;

    ::cv::findContours(filtered_frame, contours, hierarchy, CV_RETR_CCOMP,
                       CV_CHAIN_APPROX_SIMPLE);
    // Iterate through each contour and draw them out if they are good.
    for (size_t i = 0; i < contours.size(); i++) {
      double perimeter = ::cv::arcLength(contours.at(i), true);
      ::std::vector<::cv::Point> contour_estimate;
      ::cv::approxPolyDP(contours.at(i), contour_estimate, perimeter * 0.02,
                         true);

      bool known_shape = true;
      double area = ::cv::contourArea(contours.at(i), false);

      // Filter out the small contours.
      if (area < 1000) {
        known_shape = false;
      }

      short color[] = {0, 0, 0};
      if (area < 10) {
        color[0] = 10;
      } else if (area < 100) {
        color[0] = 50;
      } else if (area < 1000) {
        color[0] = 150;
      } else {
        color[0] = 255;
      }

      if (known_shape) {
        // Iterate through each point.
        for (size_t i = 0; i < contour_estimate.size(); i++) {
          ::cv::Point from = contour_estimate.at(i);
          ::cv::Point to = contour_estimate.at((i + 1) % contour_estimate.size());
          ::cv::line(original_frame, from, to,
                     ::cv::Scalar(color[0], color[1], color[2]), 4);
        }
      }
    }

    show_frame("Output frame", original_frame);

    ::cv::waitKey(10);
  }

  return 0;
}
