#ifndef DataTransformers_hpp
#define DataTransformers_hpp

#include "Spline.h"
#include "opencv2/opencv.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>

using namespace std;
using namespace cv;

struct DataTransformers {
  double modal_dist;
  vector<int> max_dist;
  vector<int> modalTracker;

  void Modal_Dist_Filter(vector<float> &points);
  void Butterworth_Filter(vector<float> &points);
  void Add_Spline(vector<float> &points);
  void PCA();

private:
  float findMode(vector<float> &points);
};

#endif
