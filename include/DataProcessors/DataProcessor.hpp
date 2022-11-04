#ifndef DataProcessor_hpp
#define DataProcessor_hpp
#include "../Utilities/LockFreeStack.hpp"
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;
struct DataProcessor {
public:
  DataProcessor(shared_ptr<lock_free_queue<Point2f>> queue) {}

private:
  shared_ptr<lock_free_queue<Point2f>> input_data;
};

#endif