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
using namespace Eigen;

struct DataTransformers {
  DataTransformers() { time_interval = 60; }
  DataTransformers(int time_interval, vector<vector<float>> data)
      : time_interval(time_interval), dataBatch(data) {}
  vector<float> PCA(vector<vector<float>> data);
  int getMostPeriodicSignal(MatrixXf& mat, BDCSVD<MatrixXf>& svd);
void filter_by_mode(std::vector<std::vector<float>> &dataBatch);
  void apply_cubic_spline_to_matrix();
  
  void apply_cubic_spline(std::vector<float> &y);
  std::vector<float>& low_pass_filter_5th_order(std::vector<float> &signal,
                                                  float f1, float f2);
  
  int countPeaks(const std::vector<float> stream);
  vector<float> max_distances;
  vector<vector<float>> dataBatch;
  void apply_kalman_filter(std::vector<std::vector<float>>& dataBatch);
  void apply_band_pass_filter(std::vector<std::vector<float>>& dataBatch);

  vector<vector<float>> principal_components;
  void center_and_scale(vector<vector<float>> &data);
  vector<float>& apply_kalman_filter(std::vector<float> &signal, float process_noise, float measurement_noise);
vector<float> butterworth_filter_5th_order(std::vector<float> &signal,
                                               float fs, float fc);
  vector<float> band_pass_filter(std::vector<float> signal, float low_cut, float high_cut);
  
  int mode = 0;
  int time_interval; // set in constructor
};

#endif
