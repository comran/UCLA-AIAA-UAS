#include <iostream>

#include "opencv/cv.h"
#include "opencv2/highgui/highgui_c.h"

int main() {
  IplImage *img = cvLoadImage("./auvsi_suas.png");

  // Show the original image.
  cvNamedWindow("Raw");
  cvShowImage("Raw", img);

  // Convert the original image into grayscale.
  IplImage *imgGrayScale = cvCreateImage(cvGetSize(img), 8, 1);
  cvCvtColor(img, imgGrayScale, CV_BGR2GRAY);

  // Threshold the grayscale image to get better results.
  cvThreshold(imgGrayScale, imgGrayScale, 110, 255, CV_THRESH_BINARY);

  CvSeq *contours;  // Hold the pointer to a contour in the memory block.
  CvSeq *result;    // Hold sequence of points of a contour.

  CvMemStorage *storage = cvCreateMemStorage(0);  // Store the contours.

  // Find all contours in the image.
  cvFindContours(imgGrayScale, storage, &contours, sizeof(CvContour),
                 CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));

  // Iterate through each contour.
  while (contours) {
    // Obtain a sequence of points in the contour.
    result =
        cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP,
                     cvContourPerimeter(contours) * 0.02, 0);

    CvPoint *pt[100];
    bool known_shape = true;
    short color[3];
    double area = cvContourArea(contours, CV_WHOLE_SEQ, 0);
    ::std::cout << "Found contour: " << result->total << " with area: " << area
                << ::std::endl;

    if (result->total < 3 || result->total > 99 || area < 1000) {
      known_shape = false;
    }

    if (area < 10) {
      color[0] = 0;
      color[1] = 0;
      color[2] = 0;
    } else if (area < 100) {
      color[0] = 50;
      color[1] = 0;
      color[2] = 0;
    } else if (area < 1000) {
      color[0] = 150;
      color[1] = 0;
      color[2] = 0;
    } else {
      color[0] = 255;
      color[1] = 0;
      color[2] = 0;
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
  cvReleaseImage(&imgGrayScale);

  return 0;
}
