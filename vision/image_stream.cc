#include "image_stream.h"

#include <thread>
#include <cassert>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
  // Check if given an argument for the directory to view streamed images.
  assert(argc == 2);

  std::string directory = argv[1];

  // Check if directory provided exists.
  struct stat statbuf;
  assert(stat(directory.c_str(), &statbuf) != -1);
  if (!S_ISDIR(statbuf.st_mode)) {
    std::cout << "NOT A DIRECTORY\n" << std::endl;
    return 1;
  }

  int current_image = -1, last_current_image = -2;
  while (true) {
    while (std::ifstream(directory + "/" + std::to_string(current_image + 1) +
                         ".jpg")) {
      current_image++;
    }

    std::cout << "current image: " << current_image << ".jpg" << std::endl;
    usleep(1e5);

    if (current_image < 1 || current_image == last_current_image) continue;
    last_current_image = current_image;
    current_image--;

    std::cout << current_image << std::endl;

    cv::Mat frame =
        cv::imread(directory + "/" + std::to_string(current_image) + ".jpg");
    cv::resize(frame, frame, cv::Size(480, 360));

    vision::shape_detector::ShapeDetector shape_detector;

    std::vector<std::vector<cv::Point>> shapes;
    shape_detector.ProcessImage(frame, shapes);

    std::vector<cv::Rect> bounding_boxes;
    for (size_t i = 0; i < shapes.size(); i++) {
      bounding_boxes.push_back(cv::boundingRect(shapes.at(i)));
    }

    std::cout << "For " << current_image << ": Found " << shapes.size()
              << " shapes.\n";

    // Make a shapes directory, if it doesn't exist yet.
    std::string shapes_directory = directory + "/shapes/";
    if (stat(shapes_directory.c_str(), &statbuf) == -1) {
      mkdir(shapes_directory.c_str(), 0777);
    }
    shapes_directory += current_image + "/";
    if (stat(shapes_directory.c_str(), &statbuf) == -1) {
      mkdir(shapes_directory.c_str(), 0777);
    }

    for (size_t i = 0; i < shapes.size(); i++) {
      cv::Mat shape_cut = frame(bounding_boxes.at(i));

      cv::imwrite(shapes_directory + std::to_string(i) + ".jpg", shape_cut);
    }

    current_image++;
  }

  return 0;
}
