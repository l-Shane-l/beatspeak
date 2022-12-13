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
  vector<float> PCA(vector<vector<float>> data);

private:
};

#endif
