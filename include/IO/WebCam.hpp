#ifndef WebCam_hpp
#define WebCam_hpp
#include "opencv2/opencv.hpp"
#include <algorithm>
#include <iostream>
#include <mutex>
#include <stdio.h>
#include <string>
using namespace std;
using namespace cv;

struct WebCam {
  WebCam(string settings) {
    if (settings == "demo") {
      std::cout << "OpenCV version : " << CV_VERSION << endl;
      cap.open("../BeatReq/chaplin.mp4");
      if (!cap.isOpened()) {
        std::cout << "Video not Found" << std::endl;
        exit(0);
      }
    } else if (settings == "cam" || settings == "movesense") {
      cap.open(0);
      cap.set(CAP_PROP_BUFFERSIZE, 2);
      std::cout << "Camera Found" << std::endl;
      if (!cap.isOpened()) {
        std::cout << "Camera not Found" << std::endl;
        exit(0);
      }
    } else {
      std::cout << "Incorrect Arguments given" << std::endl;
      exit(0);
    }
  };
  int getWidth();
  int getHeigth();
  double getFPS();
  bool running{true};
  void updateFrame();

  double getTime();
  Mat &getframeRGB();

  Mat &getframeGray();
  void release() {
    cap.release();
    destroyAllWindows();
  };

  void toggleRunning() { running = false; }

  ~WebCam() {
    std::cout << "Camera Destuctor" << std::endl;
    cap.release();
    destroyAllWindows();
  }

private:
  Mat frameRGB, frameGray;
  VideoCapture cap;
  mutex mutexframeRGB, mutexframeGray;
};

#endif
