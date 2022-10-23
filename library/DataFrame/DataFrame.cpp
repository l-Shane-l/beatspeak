#include "../../include/DataFrame/DataFrame.hpp"
#include "spdlog/spdlog.h"

void DataFrame::updateFrame(vector<Point2f> newFrame){

};
vector<Point2f> DataFrame::getFrame() {
  vector<Point2f> tmp;
  tmp.push_back({0.0, 0.0});
  return tmp;
};