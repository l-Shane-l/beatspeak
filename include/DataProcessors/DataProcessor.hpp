#ifndef DataProcessor_hpp
#define DataProcessor_hpp
#include "../Utilities/LockFreeStack.hpp"
#include "./DataTransformers.hpp"
#include "opencv2/opencv.hpp"
#include "spdlog/spdlog.h"
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>

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

private:
  DataTransformers data_transformers;
  shared_ptr<lock_free_queue<vector<Point2f>>> input_data;
  vector<vector<float>> data;
  ofstream output_file;
  chrono::system_clock::time_point start = chrono::system_clock::now();
  chrono::system_clock::time_point current = chrono::system_clock::now();
};

#endif