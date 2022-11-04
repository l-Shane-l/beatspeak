#include "../include/Concurrent.hpp"
#include "spdlog/spdlog.h"

using namespace cv;

promise<bool> prms;
future<bool> ftr = prms.get_future();
std::mutex mutexA, mutexB, mutexC;
std::condition_variable condA, condB, condC;

int run(int argc, char *argv[]) {

  if (argc < 2) {
    spdlog::info("Welcome to Beatspeak!");
    std::cerr << "enter the command like " << endl
              << argv[0] << " cam" << std::endl
              << "for webcam or " << std::endl
              << argv[0] << " demo " << std::endl
              << "for demo of software without camera" << std::endl;
    return 1;
  }

  shared_ptr<WebCam> camInputRef(new WebCam(argv[1]));
  shared_ptr<WebCam> camOutputRef = camInputRef;
  shared_ptr<lock_free_queue<Point2f>> data_points_queue{
      make_shared<lock_free_queue<Point2f>>()};
  unique_ptr<HeadTracker> tracker(new HeadTracker(data_points_queue));

  std::thread t1(getInput, ref(camInputRef));
  std::thread t2(trackHead, ref(camInputRef), ref(tracker));
  std::thread t3(sendOutput, ref(camInputRef));

  t1.join();
  t2.join();
  t3.join();

  cout << "End" << std::endl;
  return 0;
}

void getInput(shared_ptr<WebCam> &cam) {
  spdlog::info("Get Info");
  bool run = cam->running;
  std::unique_lock<std::mutex> lck(mutexA);
  while (run) {
    condB.notify_one();
    condA.wait(lck);
    run = cam->running;
    cam->updateFrame();
  }
}

void trackHead(shared_ptr<WebCam> &cam, unique_ptr<HeadTracker> &tracker) {
  spdlog::info("Track Head");
  bool run = cam->running;
  std::unique_lock<std::mutex> lck(mutexB);
  while (run) {
    condC.notify_one();
    condB.wait(lck);
    run = cam->running;
    tracker->trackHeadInFrame(cam->getframeRGB(), cam->getframeGray());
    string hr = "HeartRate Unknown";
    putText(cam->getframeRGB(), hr, Point(30, 30), FONT_HERSHEY_COMPLEX, 0.8,
            Scalar(0, 0, 255), 1);
  }
}

void sendOutput(shared_ptr<WebCam> &cam) {
  spdlog::info("Send Output");
  bool run = cam->running;
  std::unique_lock<std::mutex> lck(mutexC);
  while (run) {
    condA.notify_one();
    condC.wait(lck);
    if (!cam->getframeRGB().empty()) {

      imshow("Beatspeak", cam->getframeRGB());
      char c = (char)waitKey(25);
      if (c == 27) {
        cam->toggleRunning();
        condA.notify_one();
        condB.notify_one();
        break;
      };
    }
  }
}