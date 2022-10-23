#ifndef DataFrame_hpp
#define DataFrame_hpp
#include "../../include/DataFrame/DataFrame.hpp"
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

struct DataFrame {
public:
  DataFrame(){};
  void updateFrame(vector<Point2f> newFrame);
  vector<Point2f> getFrame();

private:
  vector<Point2f> frame;
};

#endif