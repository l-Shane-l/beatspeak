#include "../include/Concurrent.hpp"

int WebCam::getWidth() { return cap.get(cv::CAP_PROP_FRAME_WIDTH); };
int WebCam::getHeigth() { return cap.get(cv::CAP_PROP_FRAME_HEIGHT); };
double WebCam::getFPS() { return cap.get(cv::CAP_PROP_FPS); }

void WebCam::updateFrame() {
  std::lock_guard<std::mutex> lock1(mutexframeRGB, std::adopt_lock);
  cap.read(frameRGB);
  std::lock_guard<std::mutex> lock2(mutexframeGray, std::adopt_lock);
  cvtColor(frameRGB, frameGray, COLOR_BGR2GRAY);
  equalizeHist(frameGray, frameGray);
  if (frameRGB.empty()) {
    std::cout << "Frame not found" << std::endl;
  }
}

double WebCam::getTime() {
  return (cv::getTickCount() * 1000.0) / cv::getTickFrequency();
}
Mat &WebCam::getframeRGB() {
  std::lock_guard<std::mutex> lock1(mutexframeRGB, std::adopt_lock);
  return this->frameRGB;
};

Mat &WebCam::getframeGray() {
  std::lock_guard<std::mutex> lock2(mutexframeGray, std::adopt_lock);
  return this->frameGray;
};
