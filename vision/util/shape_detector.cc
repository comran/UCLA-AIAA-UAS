#include "shape_detector.h"

namespace vision {
namespace shape_detector {

ShapeTemplate::ShapeTemplate(::std::string filename, double similarity_thres) {
  name_ = filename;
  similarity_thres_ = similarity_thres;

  ::cv::Mat image = ::cv::imread(filename);
  ::cv::cvtColor(image, image, CV_BGR2GRAY);
  ::cv::threshold(image, image, 127, 255, 0);
  ::cv::subtract(::cv::Scalar::all(255), image, image);

  ::std::vector<::std::vector<::cv::Point>> contours;
  ::std::vector<::cv::Vec4i> hierarchy;

  ::cv::findContours(image, contours, hierarchy, CV_RETR_CCOMP,
                     CV_CHAIN_APPROX_SIMPLE);
  template_contour_ = contours.at(0);  // Assuming this is what we want.

  ::cv::Mat dst = ::cv::Mat::zeros(image.rows, image.cols, CV_8UC3);
  ::cv::Scalar color(255, 0, 0);

  /*
  drawContours(dst, contours, 0, color, CV_FILLED, 8, hierarchy );
  ::cv::namedWindow(filename, 1);
  ::cv::imshow(filename, dst);
  //*/
}

double ShapeTemplate::FindSimilarity(::std::vector<::cv::Point> contour) {
  double similarity = ::cv::matchShapes(contour, template_contour_, 3, 0.0);

  return similarity < similarity_thres_
             ? similarity
             : ::std::numeric_limits<double>::infinity();
}

::std::string ShapeTemplate::name() { return name_; }

ShapeDetector::ShapeDetector() {}

// Find shapes in the given image and trace them out in the frame.
void ShapeDetector::ProcessImage(
    ::cv::Mat &frame, ::std::vector<::std::vector<::cv::Point>> &shapes) {
  ::std::vector<ShapeTemplate> shape_templates;
  shape_templates.push_back(ShapeTemplate("./shapes/hexagon.jpg", 1e-3));
  shape_templates.push_back(ShapeTemplate("./shapes/star.jpg", 1e-1));
  shape_templates.push_back(ShapeTemplate("./shapes/triangle.jpg", 1e1));
//  shape_templates.push_back(ShapeTemplate("./shapes/semicircle.png", 7e-2));
//  shape_templates.push_back(ShapeTemplate("./shapes/diamond.png"));
//  shape_templates.push_back(ShapeTemplate("./shapes/plus.jpg", 1e-2));

  ::cv::Mat filtered_frames[3];
  Threshold(frame, filtered_frames);

  for (int i = 0; i < 3; i++) {
    ::std::vector<::std::vector<::cv::Point>> channel_contours;
    ::std::vector<::cv::Vec4i> channel_hierarchy;
    ::cv::findContours(filtered_frames[i], channel_contours, channel_hierarchy,
                       CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

    for (size_t i = 0; i < channel_contours.size(); i++) {
      ::std::vector<::cv::Point> contour = channel_contours.at(i);

      if (!ApproveContour(contour)) continue;

      ::cv::approxPolyDP(contour, contour, 2, true);
      shapes.push_back(contour);
    }
  }

  ::std::cout << shapes.size() << " shapes\n";
  ::std::vector<::std::vector<::std::vector<::cv::Point>>> shape_matches;
  for (size_t i = 0; i < shape_templates.size(); i++) {
    shape_matches.push_back(::std::vector<::std::vector<::cv::Point>>());
  }

  for (size_t i = 0; i < shapes.size(); i++) {
    double area = ::cv::contourArea(shapes[i]);
    if (area < 500) {
      shapes.erase(shapes.begin() + i);
      i--;

      continue;
    }

    double lowest_similarity = ::std::numeric_limits<double>::infinity();
    int most_similar_shape_index = 0;
    for (size_t j = 0; j < shape_templates.size(); j++) {
      double similarity = shape_templates[j].FindSimilarity(shapes[i]);

      if (similarity < lowest_similarity) {
        lowest_similarity = similarity;
        most_similar_shape_index = j;
      }
    }

    const double similarity_thres = 1e5;
    if (lowest_similarity > similarity_thres) {
      shapes.erase(shapes.begin() + i);
      i--;
    } else {
      ::std::cout << "i: " << i << " area: " << area << " type: "
                  << shape_templates[most_similar_shape_index].name()
                  << " similarity: " << lowest_similarity << ::std::endl;

      shape_matches[most_similar_shape_index].push_back(shapes[i]);
    }
  }

  ::std::vector<::cv::Scalar> colors;
  colors.push_back(::cv::Scalar(255, 0, 0));
  colors.push_back(::cv::Scalar(0, 255, 0));
  colors.push_back(::cv::Scalar(0, 0, 255));
  colors.push_back(::cv::Scalar(255, 255, 0));
  colors.push_back(::cv::Scalar(0, 255, 255));
  colors.push_back(::cv::Scalar(255, 0, 255));
  colors.push_back(::cv::Scalar(255, 255, 255));
  for (size_t i = 0; i < shape_matches.size(); i++) {
    OutlineContours(frame, shape_matches[i], colors[i]);
  }
  // OutlineContours(frame, shape_matches[3], ::cv::Scalar(255, 0, 255));
}

// Trace out the edges in a colored image for RGB channels.
// Puts the output edges in filtered_frames, with indexes as follows:
// red = 0, green = 1, blue = 2
void ShapeDetector::Threshold(::cv::Mat &frame, ::cv::Mat *filtered_frames) {
  const char *channels = "RGB";

  for (int i = 0; i < 3; i++) {
    ::cv::extractChannel(frame, filtered_frames[i], 2 - i /* to convert BGR */);

    // Blur channel to deal with camera noise.
    ::cv::GaussianBlur(filtered_frames[i], filtered_frames[i], ::cv::Size(3, 3),
                       0, 0);

    // Dynamic canny edge filter with automatic threshold values.
    ::cv::Mat temp_frame;  // Required for ::cv::threshold to work.
    double high_canny_threshold =
        ::cv::threshold(filtered_frames[i], temp_frame, 0, 255,
                        CV_THRESH_BINARY | CV_THRESH_OTSU);
    temp_frame.release();
    ::cv::Canny(filtered_frames[i], filtered_frames[i], high_canny_threshold,
                0.5 * high_canny_threshold);
    ::cv::dilate(filtered_frames[i], filtered_frames[i], ::cv::Mat(),
                 ::cv::Point(-1, -1));

    // Display the channel edges for debugging.
    ::std::string window_name = "Threshold channel ";
    window_name += channels[i];
    ::cv::namedWindow(window_name);
    /*::cv::moveWindow(window_name, 300 + filtered_frames[0].cols,
                     filtered_frames[0].rows * i);*/
    ::cv::imshow(window_name, filtered_frames[i]);
  }
}

// Returns whether a contour is good or not based off some sort of filter.
bool ShapeDetector::ApproveContour(::std::vector<::cv::Point> contour) {
  return true;  // Show all contours for debugging purposes.

  double area = ::cv::contourArea(contour, false);
  return !(area < 80 || area > 10000);
}

// Trace out contours on the given image.
void ShapeDetector::OutlineContours(
    ::cv::Mat &frame, ::std::vector<::std::vector<::cv::Point>> &contours,
    ::cv::Scalar color) {
  for (size_t i = 0; i < contours.size(); i++) {
    for (size_t j = 0; j < contours.at(i).size(); j++) {
      ::cv::Point from = contours.at(i).at(j);
      ::cv::Point to = contours.at(i).at((j + 1) % contours.at(i).size());

      ::cv::line(frame, from, to, color, 1);
    }
  }
}

}  // namespace shape_detector
}  // namespace vision
