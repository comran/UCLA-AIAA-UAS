#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

int main(int, char **) {
  ::cv::VideoCapture camera(0);
  ::cv::Mat frame;

  for(int i = 0;i < 20;i++) {
    camera >> frame;
    ::std::string output = "./output" + ::std::to_string(i) + ".jpg";
    ::cv::imwrite(output, frame);
    ::std::cout << "wrote" << i << ::std::endl;
  }

  return 0;
}
