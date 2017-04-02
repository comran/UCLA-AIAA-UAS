#include "shape_detector.h"

#include <chrono>
#include <thread>
#include <mutex>

namespace vision {
namespace shape_detector {
// TODO(comran): Store all the contours in some nice struct and sort in a
// priority queue based on our algorithm's confidence that the contour is a
// target.

Timer::Timer() : start_(std::chrono::high_resolution_clock::now()), count_(0) {}

void Timer::tick() {
  auto finish = std::chrono::high_resolution_clock::now();

  double duration = std::chrono::duration_cast<std::chrono::nanoseconds>(
                        finish - start_).count();

  std::cout << count_ << ": " << (duration / 1e9) << "\n";

  // Start counting time from now until tick is called again.
  start_ = std::chrono::high_resolution_clock::now();
  count_++;
}

void Timer::reset() { count_ = 0; }

ContourFilter::ContourFilter(cv::Mat &original_frame,
                             std::vector<std::vector<cv::Point>> &shapes,
                             std::vector<std::vector<cv::Point>> &good_shapes,
                             std::mutex &good_shapes_mutex, size_t start_index,
                             size_t end_index)
    : original_frame_(original_frame),
      shapes_(shapes),
      good_shapes_(good_shapes),
      start_index_(start_index),
      end_index_(end_index),
      good_shapes_mutex_(good_shapes_mutex) {

  templates_.push_back(ShapeTemplate("./shapes/circle.png"));
  templates_.push_back(ShapeTemplate("./shapes/cross.png"));
  templates_.push_back(ShapeTemplate("./shapes/diamond.png"));
  templates_.push_back(ShapeTemplate("./shapes/heptagon.png"));
  templates_.push_back(ShapeTemplate("./shapes/hexagon.jpg"));
  templates_.push_back(ShapeTemplate("./shapes/octagon.png"));
  templates_.push_back(ShapeTemplate("./shapes/pentagon.png"));
  templates_.push_back(ShapeTemplate("./shapes/quarter_circle.png"));
}

void ContourFilter::operator()() {
  for (size_t i = start_index_; i < end_index_; i++) {
    cv::Mat hist[3];
    ContourHistogram(original_frame_, shapes_.at(i), hist);

    cv::Mat peaks[3];
    bool remove = false;

    double area = cv::contourArea(shapes_.at(i));
    if (area < 25) remove = true;
    if (area > 10000) remove = true;

    for (int i = 0; !remove && i < 3; i++) {
      FindHistPeaks(hist[2], peaks[i]);
      if (peaks[i].rows != 1) {
        remove = true;
      }
    }
/*
    if (!remove) {
      cv::Mat mask =
          cv::Mat::zeros(original_frame_.rows, original_frame_.cols, CV_8UC1);
      cv::drawContours(mask,
                       std::vector<std::vector<cv::Point>>(1, shapes_.at(i)),
                       -1, cv::Scalar(255), CV_FILLED);

#ifdef DESKTOP_ENVIRONMENT
cv::namedWindow(std::to_string(i), 1);
cv::imshow(std::to_string(i), mask);
#endif

      for (size_t j = 0; j < templates_.size(); j++) {
        std::cout << std::setw(10) << templates_.at(j).name() << ": "
                  << std::setw(9)
                  << templates_.at(j).FindSimilarity(shapes_.at(i)) << " ";
      }
      std::cout << std::endl;
    }
*/

    if (!remove) {
      std::lock_guard<std::mutex> guard(good_shapes_mutex_);
      good_shapes_.push_back(shapes_.at(i));
    }
  }
}

void ContourFilter::ContourHistogram(cv::Mat &original_frame,
                                     std::vector<cv::Point> &contour,
                                     cv::Mat *hist) {
  cv::Mat mask =
      cv::Mat::zeros(original_frame.rows, original_frame.cols, CV_8UC1);
  cv::drawContours(mask, std::vector<std::vector<cv::Point>>(1, contour), -1,
                   cv::Scalar(255), CV_FILLED);

  cv::Rect bounding_rect = cv::boundingRect(mask);

  cv::Mat crop_cut = original_frame(bounding_rect);
  cv::Mat mask_cut = mask(bounding_rect);

  // Split up color channels.
  std::vector<cv::Mat> bgr_planes;
  split(crop_cut, bgr_planes);

  // Set the ranges (for B,G,R)
  float range[] = {0, 256};
  const float *histRange = {range};
  const int histSize = 32;  // Number of bins.
  int hist_w = 512;
  int hist_h = 512;

  cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

  // Compute the histograms:
  for (int i = 0; i < 3; i++) {
    cv::calcHist(&bgr_planes[i], 1, 0, mask_cut, hist[i], 1, &histSize,
                 &histRange, true, false);
    cv::normalize(hist[i], hist[i], 0, histImage.rows, cv::NORM_MINMAX, -1,
                  cv::Mat());
  }
}

void ContourFilter::NonMaximaSuppression(const cv::Mat &src, cv::Mat &mask,
                                         const bool remove_plateaus) {
  // Find pixels that are equal to the local neighborhood not maximum.
  cv::dilate(src, mask, cv::Mat());
  cv::compare(src, mask, mask, cv::CMP_GE);

  if (!remove_plateaus) return;

  // Remove pixels that are equal to the local minimum.
  cv::Mat non_plateau_mask;
  cv::erode(src, non_plateau_mask, cv::Mat());
  cv::compare(src, non_plateau_mask, non_plateau_mask, cv::CMP_GT);
  cv::bitwise_and(mask, non_plateau_mask, mask);
}

void ContourFilter::FindHistPeaks(cv::InputArray src, cv::OutputArray dst,
                                  const float scale, const cv::Size &ksize,
                                  const bool remove_plateus) {
  cv::Mat hist = src.getMat();

  // Die if histogram image is not the correct type.
  CV_Assert(hist.type() == CV_32F);

  // Find the min and max values of the hist image.
  double min_val, max_val;
  minMaxLoc(hist, &min_val, &max_val);

  cv::Mat mask;
  GaussianBlur(hist, hist, ksize, 0);                // Smoothing.
  NonMaximaSuppression(hist, mask, remove_plateus);  // Extract local maxima.

  std::vector<cv::Point> maxima;
  cv::findNonZero(mask, maxima);

  for (std::vector<cv::Point>::iterator it = maxima.begin();
       it != maxima.end();) {
    cv::Point pnt = *it;
    float val = hist.at<float>(pnt.y);

    // Filter peaks
    if ((val > max_val * scale))
      it++;
    else
      it = maxima.erase(it);
  }

  cv::Mat(maxima).copyTo(dst);
}

ShapeDetector::ShapeDetector() {}

// Find shapes in the given image and trace them out in the frame.
void ShapeDetector::ProcessImage(cv::Mat &frame,
                                 std::vector<std::vector<cv::Point>> &shapes) {
  cv::Mat filtered_frames[3];

  Threshold(frame, filtered_frames);
  GenerateContours(shapes, filtered_frames);
  FilterContours(frame, shapes);
  OutlineContours(frame, frame, shapes, cv::Scalar(0, 255, 0));
}

// Trace out the edges in a colored image for RGB channels.
// Puts the output edges in filtered_frames, with indexes as follows:
// red = 0, green = 1, blue = 2
void ShapeDetector::Threshold(cv::Mat &frame, cv::Mat *filtered_frames) {
  for (int i = 0; i < 3; i++) {
    cv::extractChannel(frame, filtered_frames[i], 2 - i /* to convert BGR */);

    // Blur channel to deal with camera noise.
    cv::GaussianBlur(filtered_frames[i], filtered_frames[i], cv::Size(3, 3), 0,
                     0);

    // Dynamic canny edge filter with automatic threshold values.
    cv::Mat temp_frame;
    double high_canny_threshold =
        cv::threshold(filtered_frames[i], temp_frame, 0, 255,
                      CV_THRESH_BINARY | CV_THRESH_OTSU);
    temp_frame.release();

    cv::Canny(filtered_frames[i], filtered_frames[i], high_canny_threshold,
              0.5 * high_canny_threshold);
    cv::dilate(filtered_frames[i], filtered_frames[i], cv::Mat(),
               cv::Point(-1, -1));
  }
}

void ShapeDetector::GenerateContours(
    std::vector<std::vector<cv::Point>> &shapes, cv::Mat *filtered_frames) {
  for (int i = 0; i < 3; i++) {
    std::vector<std::vector<cv::Point>> channel_contours;
    std::vector<cv::Vec4i> channel_hierarchy;

    cv::findContours(filtered_frames[i], channel_contours, channel_hierarchy,
                     CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

    for (size_t i = 0; i < channel_contours.size(); i++) {
      std::vector<cv::Point> contour = channel_contours.at(i);

      cv::approxPolyDP(contour, contour, 2, true);
      shapes.push_back(contour);
    }
  }
}

void ShapeDetector::FilterContours(
    cv::Mat &original_frame, std::vector<std::vector<cv::Point>> &shapes) {
  std::vector<std::vector<cv::Point>> good_shapes;
  std::mutex good_shapes_mutex;

  std::vector<ContourFilter> contour_filters;
  std::vector<std::thread> contour_filter_threads;

  const int kNumFilterThreads = 4;
  for (int i = 0; i < kNumFilterThreads; i++) {
    size_t start_index =
        static_cast<double>(shapes.size()) / kNumFilterThreads * i;
    size_t end_index =
        static_cast<double>(shapes.size()) / kNumFilterThreads * (i + 1);

    contour_filters.push_back(ContourFilter(original_frame, shapes, good_shapes,
                                            good_shapes_mutex, start_index,
                                            end_index));
    contour_filter_threads.push_back(std::thread(contour_filters.at(i)));
  }

  for (int i = 0; i < kNumFilterThreads; i++) {
    contour_filter_threads.at(i).join();
  }

  shapes = std::move(good_shapes);
}

// Trace out contours on the given image.
void ShapeDetector::OutlineContours(
    cv::Mat &frame, cv::Mat original_frame,
    std::vector<std::vector<cv::Point>> &contours, cv::Scalar color) {
  (void)color;
  (void)frame;
  (void)original_frame;
  for (size_t i = 0; i < contours.size(); i++) {
#ifdef DESKTOP_ENVIRONMENT
    for (size_t j = 0; j < contours.at(i).size(); j++) {
      cv::Point from = contours.at(i).at(j);
      cv::Point to = contours.at(i).at((j + 1) % contours.at(i).size());

      cv::line(frame, from, to, color, 1);
    }

    cv::Rect bounding_rect = cv::boundingRect(contours.at(i));
    cv::Scalar color_rect(255, 0, 0);
    cv::rectangle(frame, bounding_rect.tl(), bounding_rect.br(), color_rect, 1, 8, 0);

    cv::namedWindow("Output");
    cv::imshow("Output", frame);
    cv::moveWindow("Output", 0, 25);
#endif
/*
    std::cout << "Contour #" << std::setw(5) << i << " area: " << std::setw(15)
              << cv::contourArea(contours[i]) << std::endl;
*/

#ifdef DESKTOP_ENVIRONMENT
//    cv::waitKey(100);
#endif
  }
#ifdef DESKTOP_ENVIRONMENT
  cv::waitKey(0);
#endif
}

ShapeTemplate::ShapeTemplate(std::string filename) {
  name_ = filename;

  cv::Mat image = cv::imread(filename);
  cv::cvtColor(image, image, CV_BGR2GRAY);
  cv::threshold(image, image, 10, 255, 0);
  cv::subtract(cv::Scalar::all(255), image, image);

  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> hierarchy;

  cv::findContours(image, contours, hierarchy, CV_RETR_CCOMP,
                   CV_CHAIN_APPROX_SIMPLE);
  template_contour_ = contours.at(0);  // Assuming this is what we want.

  cv::Mat dst = cv::Mat::zeros(image.rows, image.cols, CV_8UC3);
  cv::Scalar color(255, 0, 0);

  drawContours(dst, contours, 0, color, CV_FILLED, 8, hierarchy);
  #ifdef DESKTOP_ENVIRONMENT
    cv::namedWindow(filename, 1);
    cv::imshow(filename, dst);
  #endif

  std::cout << "NEW SHAPE: " << filename << std::endl;
}

double ShapeTemplate::FindSimilarity(std::vector<cv::Point> contour) {
  double similarity = cv::matchShapes(contour, template_contour_, 3, 0.0);

  return similarity;
}

std::string ShapeTemplate::name() { return name_; }

}  // namespace shape_detector
}  // namespace vision
