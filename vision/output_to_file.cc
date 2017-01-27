#include "output_to_file.h"

int main(int, char** argv) {
  ::cv::Mat frame = ::cv::imread(argv[1]);

  ::vision::shape_detector::ShapeDetector shape_detector;

  ::std::vector<::std::vector<::cv::Point>> shapes;
  shape_detector.ProcessImage(frame, shapes);

  imwrite("./output.jpg", frame);

  return 0;
}
