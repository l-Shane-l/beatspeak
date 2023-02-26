#include "../include/Concurrent.hpp"
#include "spdlog/spdlog.h"

using namespace cv;

promise<bool> prms;
  int countdown_duration = 10; // 10 seconds
  auto start_time = std::chrono::steady_clock::now();
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
  std::atomic<int> estimatedHeartRate(-1);
  shared_ptr<WebCam> camInputRef(new WebCam(argv[1]));
  shared_ptr<WebCam> camOutputRef = camInputRef;
  shared_ptr<lock_free_queue<vector<Point2f>>> data_points_queue(
      new lock_free_queue<vector<Point2f>>());

  unique_ptr<HeadTracker> tracker(new HeadTracker(data_points_queue));
  unique_ptr<DataProcessor> processor(new DataProcessor(data_points_queue, &estimatedHeartRate));

  thread t1(getInput, ref(camInputRef));
  thread t2(trackHead, ref(camInputRef), ref(tracker), ref(estimatedHeartRate));
  thread t3(sendOutput, ref(camInputRef));
  thread t4(processData, ref(processor));

  t1.join();
  t2.join();
  t3.join();
  t4.join();

  spdlog::info("End");
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

void trackHead(shared_ptr<WebCam> &cam, unique_ptr<HeadTracker> &tracker, std::atomic<int> &estimatedHeartRate) {
  spdlog::info("Track Head");
  bool run = cam->running;
  std::unique_lock<std::mutex> lck(mutexB);
  while (run) {
    condC.notify_one();
    condB.wait(lck);
    run = cam->running;
    tracker->trackHeadInFrame(cam->getframeRGB(), cam->getframeGray());
    estimatedHeartRate.load();
    auto elapsed_time = std::chrono::steady_clock::now() - start_time;
    int remaining_time = countdown_duration - std::chrono::duration_cast<std::chrono::seconds>(elapsed_time).count();
    if (remaining_time <= 0) {
      remaining_time = 10 - std::abs(remaining_time) % 10;
    }
    string nextReading = "Next Reading in: " + to_string(remaining_time) + " seconds";
    string hr = "Calculating your Heart Rate ...";
    if(estimatedHeartRate != -1){
      hr = "Your Estimated Heart Rate is: " + to_string(estimatedHeartRate);
    
    }
    cv::Rect rect(1, 1, 800, 75);
    cv::rectangle(cam->getframeRGB(), rect, cv::Scalar(0, 0, 0), cv::FILLED);
    cv::putText(cam->getframeRGB(), hr, cv::Point(30, 30), cv::FONT_HERSHEY_COMPLEX, 0.8, cv::Scalar(255, 255, 255), 1);
    cv::putText(cam->getframeRGB(), nextReading, cv::Point(30, 60), cv::FONT_HERSHEY_COMPLEX, 0.8, cv::Scalar(255, 255, 255), 1);
  }
}

void processData(unique_ptr<DataProcessor> &processor) {
  spdlog::info("Process Data");
  processor->setup_log();

  for (;;) {
    processor->log_data();
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