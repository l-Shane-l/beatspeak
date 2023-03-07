#include "../include/Concurrent.hpp"
#include "spdlog/spdlog.h"
#include "curl/curl.h"

using namespace cv;

#include <sstream>

size_t writeCallback(char *data, size_t size, size_t nmemb, void *userdata) {
  std::ostringstream *stream = reinterpret_cast<std::ostringstream*>(userdata);
  size_t count = size * nmemb;
  stream->write(data, count);
  return count;
}

size_t curlWriteCallback(char *ptr, size_t size, size_t nmemb, void *userdata) {
  return size * nmemb;
}

string makeGetRequest() {
  CURL *curl;
  CURLcode res;
  std::ostringstream response;

  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.0.118:8080/heartrate");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L); // Set timeout to 5 seconds
    res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
      spdlog::error("curl_easy_perform() failed: {}", curl_easy_strerror(res));
      curl_easy_cleanup(curl);
      return "";
    }
    curl_easy_cleanup(curl);
  }

  // Parse the response as an array of numbers
  std::string response_str = response.str();
  std::vector<int> numbers;
  std::istringstream iss(response_str.substr(1, response_str.size() - 2));
  std::string token;
  while(std::getline(iss, token, ',')) {
    numbers.push_back(std::stoi(token));
  }

  // Compute the average of the numbers
  double sum = 0.0;
  for (int n : numbers) {
    sum += n;
  }
  double avg = sum / numbers.size();

  // Send a new HTTP GET request to clear the buffer at the URL
  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.0.118:8080/heartrate");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L); // Set timeout to 5 seconds
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
  }

  // Return the average as a string
  return std::to_string(avg);
}





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
              << "for demo of software without camera" << std::endl
              << argv[0] << " movesense " << std::endl
              << "use with movesense wearable sensor" << std::endl;
              
    return 1;
  }
  std::atomic<int> estimatedHeartRate(-1);
  shared_ptr<WebCam> camInputRef(new WebCam(argv[1]));
  shared_ptr<WebCam> camOutputRef = camInputRef;
  shared_ptr<lock_free_queue<vector<Point2f>>> data_points_queue(
      new lock_free_queue<vector<Point2f>>());

  unique_ptr<HeadTracker> tracker(new HeadTracker(data_points_queue));
  unique_ptr<DataProcessor> processor(new DataProcessor(data_points_queue, &estimatedHeartRate));
  string option = argv[1];
  thread t1(getInput, ref(camInputRef));
  thread t2(trackHead, ref(camInputRef), ref(tracker), ref(estimatedHeartRate), ref(option));
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

void trackHead(shared_ptr<WebCam>& cam, unique_ptr<HeadTracker>& tracker, std::atomic<int>& estimatedHeartRate, string mode) {
  spdlog::info("Track Head");
  bool run = cam->running;
  std::unique_lock<std::mutex> lck(mutexB);
  int count = 0;
  std::string actualHR = "";
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
    string hr = "Calculating Heart Rate ...";
    if(estimatedHeartRate != -1){
      hr = "Your Estimated Heart Rate is: " + to_string(estimatedHeartRate);
    }

    if(count % 100 == 0){
      spdlog::info("Getting actual heart rate ...");
      spdlog::info(actualHR);
      actualHR = makeGetRequest();
      spdlog::info("updated actual rate is: ");
      spdlog::info(actualHR);
      if (actualHR.empty()) {
        actualHR = "Actual Heart Rate is: unavailable";
      } else {
        actualHR = "Actual Heart Rate is: " + std::to_string(static_cast<int>(std::round(std::stod(actualHR))));
      }
    }
    ++count;

    // Compute the percentage error
    std::string errorStr = "";
    if (estimatedHeartRate != -1 && !actualHR.empty() && actualHR != "Actual Heart Rate is: unavailable") {
      int actual = stoi(actualHR.substr(22));
      int error = (estimatedHeartRate - actual) * 100 / actual;
      errorStr = "Percentage Error: " + to_string(error) + "%";
    }

    cv::Rect rect(1, 1, 800, 130);

    cv::rectangle(cam->getframeRGB(), rect, cv::Scalar(0, 0, 0), cv::FILLED);
    cv::putText(cam->getframeRGB(), hr, cv::Point(30, 30), cv::FONT_HERSHEY_COMPLEX, 0.8, cv::Scalar(255, 255, 255), 1);
    cv::putText(cam->getframeRGB(), actualHR, cv::Point(30, 60), cv::FONT_HERSHEY_COMPLEX, 0.8, cv::Scalar(255, 255, 255), 1);
    cv::putText(cam->getframeRGB(), errorStr, cv::Point(30, 90), cv::FONT_HERSHEY_COMPLEX, 0.8, cv::Scalar(255, 255, 255), 1);
    cv::putText(cam->getframeRGB(), nextReading, cv::Point(30, 120), cv::FONT_HERSHEY_COMPLEX, 0.8, cv::Scalar(255, 255, 255), 1);
    if(mode == "movesense"){
      cv::Rect rect(1, 120, 800, 30);
      cv::rectangle(cam->getframeRGB(), rect, cv::Scalar(0, 0, 0), cv::FILLED);
      string wearable = makeGetRequest();
      cv::putText(cam->getframeRGB(), wearable, cv::Point(30, 150), cv::FONT_HERSHEY_COMPLEX, 0.8, cv::Scalar(255, 255, 255), 1);
    }
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