#include "vision/target_spotter.h"

#include <iostream>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "vision/shape_detector.h"

namespace vision {
namespace target_spotter {
TargetSpotter::TargetSpotter() : camera_(0) {}
void TargetSpotter::Quit() { run_ = false; }

// Thread that feeds frames from the camera to the shape_detector, and outputs
// an image outlining these contours.
void TargetSpotter::operator()() {
  while(run_) {
    ::cv::Mat frame;
    camera_ >> frame;

    ::cv::namedWindow("Original");
    ::cv::imshow("Original", frame);

    ::vision::shape_detector::ShapeDetector shape_detector;

    ::std::vector<::std::vector<::cv::Point>> shapes;
    shape_detector.ProcessImage(frame, shapes);

    ::cv::namedWindow("Output");
    ::cv::imshow("Output", frame);

    ::cv::waitKey(10);
  }
}

}  // namespace target_spotter
}  // namespace vision
