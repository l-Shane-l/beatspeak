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
  ofstream result_file;
  ofstream post_filter;
  int time_interval = 4;
  chrono::system_clock::time_point start = chrono::system_clock::now();
  chrono::system_clock::time_point current = chrono::system_clock::now();
  vector<float> max_distances;
  void find_max_distances();
  void find_mode();
  void filter_by_mode();
  void apply_cublic_spline_to_matrix();
  vector<float> apply_cubic_spline(const vector<float> &x);
  std::vector<float> butterworth_filter_5th_order(std::vector<float> &signal,
                                                  float f1, float f2);
  void apply_butterworth_filter();
  int countPeaks(const std::vector<float> stream);
  vector<float> principal_components;
  int mode = 0;
  DataTransformers data_transformer;
};

#endif