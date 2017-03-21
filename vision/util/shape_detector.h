#ifndef VISION_SHAPE_DETECTOR_H_
#define VISION_SHAPE_DETECTOR_H_

#include <iostream>
#include <vector>
#include <limits>
#include <iomanip>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

namespace vision {
namespace shape_detector {
class ShapeTemplate {
 public:
  ShapeTemplate(std::string filename, double similarity_thres);
  double FindSimilarity(std::vector<cv::Point> contour);
  std::string name();

 private:
  std::vector<cv::Point> template_contour_;
  std::string name_;
  double similarity_thres_;
};

class ShapeDetector {
 public:
  ShapeDetector();
  void ProcessImage(cv::Mat &frame,
                    std::vector<std::vector<cv::Point>> &shapes);

 private:
  void Threshold(cv::Mat &frame, cv::Mat *filtered_frames);
  void GenerateContours(std::vector<std::vector<cv::Point>> &shapes,
                        cv::Mat *filtered_frames);
  void FilterContours(cv::Mat &original_frame,
                      std::vector<std::vector<cv::Point>> &shapes);
  bool ApproveContour(std::vector<cv::Point> contour);
  void OutlineContours(cv::Mat &frame, cv::Mat original_frame,
                       std::vector<std::vector<cv::Point>> &contours,
                       cv::Scalar color);
};

}  // namespace shape_detector
}  // namespace vision

#endif  // VISION_SHAPE_DETECTOR_H_
