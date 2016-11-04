#ifndef VISION_TARGET_SPOTTER_H_
#define VISION_TARGET_SPOTTER_H_

#include <iostream>
#include <vector>
#include <atomic>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

namespace vision {
namespace target_spotter {
class TargetSpotter {
 public:
  TargetSpotter();

  void operator()();
  void Quit();
 private:
  ::std::atomic<bool> run_{true};
  ::cv::VideoCapture camera_;
};

}  // namespace target_spotter
}  // namespace vision

#endif  // VISION_TARGET_SPOTTER_H_
