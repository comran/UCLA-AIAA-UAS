#include "vision/target_spotter.h"

#include <thread>

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
    ::cv::moveWindow("Output", 0, frame.rows);

    const int frames_per_second = 30;
    ::cv::waitKey(1000 / frames_per_second);
  }
}

}  // namespace target_spotter
}  // namespace vision

int main(int argc, char** argv) {
  // Handle processing targets in an image file.
  if(argc > 1) {
    ::cv::Mat frame = ::cv::imread(argv[1]);

    ::cv::namedWindow("Original");
    ::cv::imshow("Original", frame);

    ::vision::shape_detector::ShapeDetector shape_detector;

    ::std::vector<::std::vector<::cv::Point>> shapes;
    shape_detector.ProcessImage(frame, shapes);

    ::cv::namedWindow("Output");
    ::cv::imshow("Output", frame);
    ::cv::moveWindow("Output", 0, frame.rows);

    ::cv::waitKey(0);

    return 0;
  }

  // Feed in stream from camera.
  ::vision::target_spotter::TargetSpotter target_spotter;
  ::std::thread target_spotter_thread(::std::ref(target_spotter));

  // Wait forever.
  while (true) {
    const int r = select(0, nullptr, nullptr, nullptr, nullptr);
    (void) r;
  }

  target_spotter.Quit();
  target_spotter_thread.join();

  return 0;
}
