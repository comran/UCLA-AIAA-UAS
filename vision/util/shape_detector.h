#ifndef VISION_SHAPE_DETECTOR_H_
#define VISION_SHAPE_DETECTOR_H_

#include <iostream>
#include <vector>
#include <limits>
#include <iomanip>
#include <chrono>
#include <mutex>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

namespace vision {
namespace shape_detector {

// Class to track down slow algorithms and sections of the code.
class Timer {
 public:
  Timer();
  void tick();  // Print time since last tick or Timer construction.
  void reset();  // Reset number printed before time since last tick.

 private:
  std::chrono::time_point<std::chrono::high_resolution_clock> start_;
  int count_;
};

class ShapeTemplate {
 public:
  ShapeTemplate(std::string filename);
  double FindSimilarity(std::vector<cv::Point> contour);
  std::string name();

 private:
  std::vector<cv::Point> template_contour_;
  std::string name_;
};

// Multithreaded filter to run some rough tests on all the contours to narrow
// down on the good ones.
class ContourFilter {
 public:
  ContourFilter(cv::Mat &original_frame,
                std::vector<std::vector<cv::Point>> &shapes,
                std::vector<std::vector<cv::Point>> &good_shapes,
                std::mutex &good_shapes_mutex,
                size_t start_index, size_t end_index);
  void operator()();

  void ContourHistogram(cv::Mat &original_frame,
                        std::vector<cv::Point> &contour, cv::Mat *hist);
  void NonMaximaSuppression(const cv::Mat &src, cv::Mat &mask,
                            const bool remove_plateaus);
  void FindHistPeaks(cv::InputArray src, cv::OutputArray dst,
                     const float scale = 0.2,
                     const cv::Size &ksize = cv::Size(5, 5),
                     const bool remove_plateus = true);

 private:
  cv::Mat &original_frame_;
  std::vector<std::vector<cv::Point>> &shapes_;
  std::vector<std::vector<cv::Point>> &good_shapes_;
  std::vector<ShapeTemplate> templates_;

  size_t start_index_, end_index_;
  std::mutex &good_shapes_mutex_;
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
