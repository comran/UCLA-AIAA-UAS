#include "shape_detector.h"

namespace vision {
namespace shape_detector {

ShapeTemplate::ShapeTemplate(::std::string filename, double similarity_thres) {
  name_ = filename;
  similarity_thres_ = similarity_thres;

  ::cv::Mat image = ::cv::imread(filename);
  ::cv::cvtColor(image, image, CV_BGR2GRAY);
  ::cv::threshold(image, image, 10, 255, 0);
  ::cv::subtract(::cv::Scalar::all(255), image, image);

  ::std::vector<::std::vector<::cv::Point>> contours;
  ::std::vector<::cv::Vec4i> hierarchy;

  ::cv::findContours(image, contours, hierarchy, CV_RETR_CCOMP,
                     CV_CHAIN_APPROX_SIMPLE);
  template_contour_ = contours.at(0);  // Assuming this is what we want.

  ::cv::Mat dst = ::cv::Mat::zeros(image.rows, image.cols, CV_8UC3);
  ::cv::Scalar color(255, 0, 0);

  drawContours(dst, contours, 0, color, CV_FILLED, 8, hierarchy);
#ifdef DESKTOP_ENVIRONMENT
  ::cv::namedWindow(filename, 1);
  ::cv::imshow(filename, dst);
#endif
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
  ::cv::Mat filtered_frames[3];
  ::cv::Mat original_frame = frame.clone();
  Threshold(frame, filtered_frames);

  for (int i = 0; i < 3; i++) {
    ::std::vector<::std::vector<::cv::Point>> channel_contours;
    ::std::vector<::cv::Vec4i> channel_hierarchy;
    ::cv::findContours(filtered_frames[i], channel_contours, channel_hierarchy,
                       CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

    for (size_t i = 0; i < channel_contours.size(); i++) {
      ::std::vector<::cv::Point> contour = channel_contours.at(i);

      ::cv::approxPolyDP(contour, contour, 2, true);
      shapes.push_back(contour);
    }
  }

  ::std::vector<::std::vector<::std::vector<::cv::Point>>> shape_matches;

  for (size_t i = 0; i < shapes.size(); i++) {
    double area = ::cv::contourArea(shapes[i]);
    if (area < 1) {
      shapes.erase(shapes.begin() + i);
      i--;
    }
  }

  OutlineContours(frame, original_frame, shapes, ::cv::Scalar(0, 255, 0));
}

// Trace out the edges in a colored image for RGB channels.
// Puts the output edges in filtered_frames, with indexes as follows:
// red = 0, green = 1, blue = 2
void ShapeDetector::Threshold(::cv::Mat &frame, ::cv::Mat *filtered_frames) {
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
  }
}

// Trace out contours on the given image.
void ShapeDetector::OutlineContours(
    ::cv::Mat &frame, ::cv::Mat original_frame,
    ::std::vector<::std::vector<::cv::Point>> &contours, ::cv::Scalar color) {
  for (size_t i = 0; i < contours.size(); i++) {
    for (size_t j = 0; j < contours.at(i).size(); j++) {
      ::cv::Point from = contours.at(i).at(j);
      ::cv::Point to = contours.at(i).at((j + 1) % contours.at(i).size());

      ::cv::line(frame, from, to, color, 1);
    }

#ifdef DESKTOP_ENVIRONMENT
    ::cv::namedWindow("Output");
    ::cv::imshow("Output", frame);
    ::cv::moveWindow("Output", 0, 25);
#endif

    // Extract all contours...
    {
      ::cv::Mat mask = ::cv::Mat::zeros(frame.rows, frame.cols, CV_8UC1);
      ::cv::drawContours(
          mask, ::std::vector<::std::vector<::cv::Point>>(1, contours[i]), -1,
          ::cv::Scalar(255), CV_FILLED);

      ::cv::Mat crop = ::cv::Mat::zeros(frame.rows, frame.cols, CV_8UC3);
      crop.setTo(::cv::Scalar(0, 255, 0));
      original_frame.copyTo(crop, mask);

      // Bounding rect and circle.
      ::cv::Rect bounding_rect = ::cv::boundingRect(mask);
      ::cv::rectangle(crop, bounding_rect.tl(), bounding_rect.br(),
                      ::cv::Scalar(0, 0, 255), 2, 8, 0);

      ::cv::Mat crop_cut = ::cv::Mat::zeros(frame.rows, frame.cols, CV_8UC3);
      crop_cut.setTo(::cv::Scalar(0, 0, 0));
      original_frame.copyTo(crop_cut, mask);
      crop_cut = crop_cut(bounding_rect);

      ::cv::normalize(mask.clone(), mask, 0.0, 255.0, CV_MINMAX, CV_8UC1);
#ifdef DESKTOP_ENVIRONMENT
      ::cv::namedWindow("cropped");
      ::cv::imshow("cropped", crop);
      ::cv::moveWindow("cropped", frame.cols, 25);

      ::cv::namedWindow("cropped cut");
      ::cv::imshow("cropped cut", crop_cut);
      ::cv::moveWindow("cropped cut", frame.cols, frame.rows + 30 + 25);
#endif

      // Histogram.
      {
        /// Separate the image in 3 places ( B, G and R )
        ::std::vector<::cv::Mat> bgr_planes;
        split(crop_cut, bgr_planes);

        /// Establish the number of bins
        int histSize = 128;

        /// Set the ranges ( for B,G,R) )
        float range[] = {0, 256};
        const float *histRange = {range};

        bool uniform = true;
        bool accumulate = false;

        ::cv::Mat b_hist, g_hist, r_hist;

        /// Compute the histograms:
        ::cv::calcHist(&bgr_planes[0], 1, 0, ::cv::Mat(), b_hist, 1, &histSize,
                       &histRange, uniform, accumulate);
        ::cv::calcHist(&bgr_planes[1], 1, 0, ::cv::Mat(), g_hist, 1, &histSize,
                       &histRange, uniform, accumulate);
        ::cv::calcHist(&bgr_planes[2], 1, 0, ::cv::Mat(), r_hist, 1, &histSize,
                       &histRange, uniform, accumulate);

        // Draw the histograms for B, G and R
        int hist_w = 512;
        int hist_h = 512;
        int bin_w = cvRound((double)hist_w / histSize);

        ::cv::Mat histImage(hist_h, hist_w, CV_8UC3, ::cv::Scalar(0, 0, 0));

        /// Normalize the result to [ 0, histImage.rows ]
        ::cv::normalize(b_hist, b_hist, 0, histImage.rows, ::cv::NORM_MINMAX,
                        -1, ::cv::Mat());
        ::cv::normalize(g_hist, g_hist, 0, histImage.rows, ::cv::NORM_MINMAX,
                        -1, ::cv::Mat());
        ::cv::normalize(r_hist, r_hist, 0, histImage.rows, ::cv::NORM_MINMAX,
                        -1, ::cv::Mat());

        /// Draw for each channel
        for (int i = 1; i < histSize; i++) {
          ::cv::line(
              histImage, ::cv::Point(bin_w * (i - 1),
                                     hist_h - cvRound(b_hist.at<float>(i - 1))),
              ::cv::Point(bin_w * (i), hist_h - cvRound(b_hist.at<float>(i))),
              ::cv::Scalar(255, 0, 0), 2, 8, 0);
          ::cv::line(
              histImage, ::cv::Point(bin_w * (i - 1),
                                     hist_h - cvRound(g_hist.at<float>(i - 1))),
              ::cv::Point(bin_w * (i), hist_h - cvRound(g_hist.at<float>(i))),
              ::cv::Scalar(0, 255, 0), 2, 8, 0);
          ::cv::line(
              histImage, ::cv::Point(bin_w * (i - 1),
                                     hist_h - cvRound(r_hist.at<float>(i - 1))),
              ::cv::Point(bin_w * (i), hist_h - cvRound(r_hist.at<float>(i))),
              ::cv::Scalar(0, 0, 255), 2, 8, 0);
        }

#ifdef DESKTOP_ENVIRONMENT
        ::cv::namedWindow("histogram", CV_WINDOW_AUTOSIZE);
        ::cv::imshow("histogram", histImage);
        ::cv::moveWindow("histogram", crop.cols * 2, 25);
#endif
      }

      ::std::cout << "Contour #" << ::std::setw(5) << i
                  << " area: " << ::std::setw(15)
                  << ::cv::contourArea(contours[i])
                  << " extent: " << ::std::setw(15)
                  << ::cv::contourArea(contours[i]) / bounding_rect.area()
                  << ::std::endl;

#ifdef DESKTOP_ENVIRONMENT
      ::cv::waitKey(0);
#endif
    }
  }
}

}  // namespace shape_detector
}  // namespace vision
