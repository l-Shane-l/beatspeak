#ifndef DataProcessor_hpp
#define DataProcessor_hpp
#include "../Utilities/LockFreeStack.hpp"
#include "opencv2/opencv.hpp"
#include "spdlog/spdlog.h"
#include <fstream>
#include <iostream>

using namespace std;
using namespace cv;
struct DataProcessor {
public:
  DataProcessor(shared_ptr<lock_free_queue<Point2f>> queue) {
    input_data = move(queue);
    output_file.open("output.dat");
  }
  ~DataProcessor() { output_file.close(); }
  void log_data();
  void setup_log();

private:
  shared_ptr<lock_free_queue<Point2f>> input_data;
  ofstream output_file;
};

#endif