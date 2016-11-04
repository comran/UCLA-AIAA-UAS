#include <iostream>

#include "shape_detector.h"

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

void show_frame(const ::std::string window_name, ::cv::Mat &frame) {
  ::cv::namedWindow(window_name);
  ::cv::imshow(window_name, frame);
}

void test_image() {
  ::cv::Mat original_frame = ::cv::imread(
      "./sample_terrain/target_collection_1.png", CV_LOAD_IMAGE_COLOR);

  show_frame("Original frame", original_frame);

  ::y2016::shape_detector::ShapeDetector shape_detector;

  ::std::vector<::std::vector<::cv::Point>> shapes =
      shape_detector.ProcessImage(original_frame);

  for (size_t i = 0; i < shapes.size(); i++) {
    ::cv::Scalar color(::std::rand() % 255, ::std::rand() % 255, ::std::rand() % 255);

    for (size_t j = 0; j < shapes.at(i).size(); j++) {
      ::cv::Point from = shapes.at(i).at(j);
      ::cv::Point to = shapes.at(i).at((j + 1) % shapes.at(i).size());

      ::cv::line(original_frame, from, to, color, 4);
    }
  }

  show_frame("Output frame", original_frame);

  ::cv::waitKey(0);
}

void stream() {
  ::cv::VideoCapture camera(0);

  while (true) {
    ::cv::Mat original_frame;
    camera >> original_frame;

    show_frame("Original frame", original_frame);

    ::y2016::shape_detector::ShapeDetector shape_detector;

    ::std::vector<::std::vector<::cv::Point>> shapes =
        shape_detector.ProcessImage(original_frame);

    for (size_t i = 0; i < shapes.size(); i++) {
      ::cv::Scalar color(0, 0, 255);

      for (size_t j = 0; j < shapes.at(i).size(); j++) {
        ::cv::Point from = shapes.at(i).at(j);
        ::cv::Point to = shapes.at(i).at((j + 1) % shapes.at(i).size());

        ::cv::line(original_frame, from, to, color, 4);
      }
    }

    show_frame("Output frame", original_frame);

    ::cv::waitKey(10);
  }
}

int main(int, char **) {
  test_image();
  //stream();
  return 0;
}
