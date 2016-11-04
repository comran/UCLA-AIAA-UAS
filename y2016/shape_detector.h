#ifndef Y2016_SHAPE_DETECTOR_H_
#define Y2016_SHAPE_DETECTOR_H_

#include <iostream>
#include <vector>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

namespace y2016 {
namespace shape_detector {
class ShapeDetector {
 public:
  ShapeDetector();
  ::std::vector<::std::vector<::cv::Point>> ProcessImage(::cv::Mat frame);

 private:
  ::cv::Mat Threshold(::cv::Mat frame, int channel);
};

}  // namespace shape_detector
}  // namespace y2016

#endif  // Y2016_SHAPE_DETECTOR_H_
