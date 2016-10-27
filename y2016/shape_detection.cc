#include <iostream>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"

void show_frame(const ::std::string window_name, ::cv::Mat &frame) {
  ::cv::namedWindow(window_name);
  ::cv::imshow(window_name, frame);
}

int main(int, char **) {
  ::cv::Mat original_frame =
                ::cv::imread("./auvsi_suas.png", CV_LOAD_IMAGE_COLOR),
            filtered_frame = ::cv::Mat::zeros(original_frame.rows,
                                              original_frame.cols, CV_8UC3);

  show_frame("Original frame", original_frame);

  // Convert the original image into grayscale and do thresholding.
  ::cv::cvtColor(original_frame, filtered_frame, CV_BGR2GRAY);
  show_frame("Black and white frame", filtered_frame);
  ::cv::threshold(filtered_frame, filtered_frame, 110, 255, CV_THRESH_BINARY);
  show_frame("Thresholded frame", filtered_frame);

  ::std::vector<::std::vector<::cv::Point>> contours;
  ::std::vector<::cv::Vec4i> hierarchy;

  ::cv::findContours(filtered_frame, contours, hierarchy, CV_RETR_CCOMP,
                     CV_CHAIN_APPROX_SIMPLE);

/*
  for (int idx = 0; idx >= 0; idx = hierarchy[idx][0]) {
    ::cv::Scalar color(rand() & 255, rand() & 255, rand() & 255);
    drawContours(dst, contours, idx, color, CV_FILLED, 8, hierarchy);
  }

  ::cv::namedWindow("Components", 1);
  ::cv::imshow("Components", dst);*/
  ::cv::waitKey(0);
  /*
    // Iterate through each contour.
    while (contours) {
      // Obtain a sequence of points in the contour.
      result =
          cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP,
                       cvContourPerimeter(contours) * 0.02, 0);

      CvPoint *pt[100];
      bool known_shape = true;
      short color[] = {0, 0, 0};
      double area = cvContourArea(contours, CV_WHOLE_SEQ, 0);
      ::std::cout << "Found contour: " << result->total << " with area: " <<
    area
                  << ::std::endl;

      if (result->total < 3 || result->total > 99 || area < 1000) {
        known_shape = false;
      }

      if (area < 10) {
        color[0] = 10;
      } else if (area < 100) {
        color[0] = 50;
      } else if (area < 1000) {
        color[0] = 150;
      } else {
        color[0] = 255;
      }

      if (known_shape) {
        // Iterate through each point.
        for (int i = 0; i < result->total; i++) {
          pt[i] = (CvPoint *)cvGetSeqElem(result, i);
        }

        // Draw lines around the polygon.
        for (int i = 0; i < result->total; i++) {
          cvLine(img, *pt[i], *pt[(i + 1) % result->total],
                 cvScalar(color[0], color[1], color[2]), 4);
        }
      }

      // Obtain the next contour.
      contours = contours->h_next;
    }

    // Show the image in which identified shapes are marked.
    cvShowImage("Tracked", img);
    cvShowImage("Tracked Gray", imgGrayScale);

    cvWaitKey(0);  // Wait for a keypress to exit.

    cvDestroyAllWindows();
    cvReleaseMemStorage(&storage);
    cvReleaseImage(&img);
    cvReleaseImage(&imgGrayScale);*/

  return 0;
}
