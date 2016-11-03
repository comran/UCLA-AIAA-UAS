#include <iostream>

#include "shape_detector.h"

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

void show_frame(const ::std::string window_name, ::cv::Mat &frame) {
  ::cv::namedWindow(window_name);
  ::cv::imshow(window_name, frame);
}

int main(int, char **) {
  ::cv::VideoCapture camera(0);

  while (true) {
    ::cv::Mat original_frame;
    camera >> original_frame;

    show_frame("Original frame", original_frame);

    ::y2016::shape_detector::ShapeDetector shape_detector;

    ::std::vector<::std::vector<::cv::Point>> shapes =
        shape_detector.ProcessImage(original_frame);

    for (size_t i = 0; i < shapes.size(); i++) {
      double area = ::cv::contourArea(shapes.at(i), false);
      if (area < 800 || area > 5000) continue;

      ::cv::Scalar color(0, 0, 255);

      ::std::cout << "size: " << shapes.size() << ::std::endl;
      for (size_t j = 0;j < shapes.at(i).size();j++) {
        ::cv::Point from = shapes.at(i).at(j);
        ::cv::Point to = shapes.at(i).at((j + 1) % shapes.at(i).size());

        ::cv::line(original_frame, from, to, color, 4);
      }
    }

    show_frame("Output frame", original_frame);

    ::cv::waitKey(10);
  }

  return 0;
}
