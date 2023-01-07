#ifndef DataTransformers_hpp
#define DataTransformers_hpp

#include "Spline.h"
#include "eigen3/Eigen/Dense"
#include "eigen3/Eigen/Eigenvalues"
#include "opencv2/opencv.hpp"
#include "spdlog/fmt/ostr.h"
#include "spdlog/spdlog.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <thread>
#include <vector>

using namespace std;
using namespace cv;

struct DataTransformers {
  DataTransformers() { time_interval = 4; }
  DataTransformers(int time_interval, vector<vector<float>> data)
      : time_interval(time_interval), dataBatch(data) {}
  vector<float> PCA(vector<vector<float>> data);

  void find_max_distances();
  void find_mode();
  void filter_by_mode();
  void apply_cublic_spline_to_matrix();
  vector<float> apply_cubic_spline(const vector<float> &x);
  std::vector<float> butterworth_filter_5th_order(std::vector<float> &signal,
                                                  float f1, float f2);
  void apply_butterworth_filter();
  int countPeaks(const std::vector<float> stream);
  vector<float> max_distances;
  vector<vector<float>> dataBatch;
  vector<float> principal_components;
  int mode = 0;
  int time_interval; // set in constructor
};

#endif
