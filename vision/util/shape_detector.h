#ifndef VISION_SHAPE_DETECTOR_H_
#define VISION_SHAPE_DETECTOR_H_

#include <iostream>
#include <vector>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

namespace vision {
namespace shape_detector {
class ShapeDetector {
 public:
  ShapeDetector();
  void ProcessImage(::cv::Mat frame,
                    ::std::vector<::std::vector<::cv::Point>> &shapes);

 private:
  ::cv::Mat Threshold(::cv::Mat frame, int channel);
  bool ApproveContour(::std::vector<::cv::Point> contour);
  void OutlineContours(::cv::Mat &frame,
                       ::std::vector<::std::vector<::cv::Point>> &contours);
};

}  // namespace shape_detector
}  // namespace vision

#endif  // VISION_SHAPE_DETECTOR_H_
