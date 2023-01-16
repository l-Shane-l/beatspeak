#ifndef DataProcessor_hpp
#define DataProcessor_hpp
#include "../Utilities/LockFreeStack.hpp"
#include "../Utilities/spline.h"
#include "./DataTransformers.hpp"

#include "opencv2/opencv.hpp"
#include "spdlog/spdlog.h"
#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <thread>

using namespace std;
using namespace cv;
struct DataProcessor {
public:
  DataProcessor(shared_ptr<lock_free_queue<vector<Point2f>>> queue) {
    input_data = move(queue);
    output_file.open("output.dat");
  }
  ~DataProcessor() { output_file.close(); }
  void log_data();
  void setup_log();
  void process_data();

private:
  int time_interval = 20;
  vector<vector<float>> data;

  shared_ptr<lock_free_queue<vector<Point2f>>> input_data;
  ofstream output_file;
  ofstream result_file;
  ofstream post_filter;
  chrono::system_clock::time_point start = chrono::system_clock::now();
  chrono::system_clock::time_point current = chrono::system_clock::now();
};

#endif