#include "../../include/DataProcessors/DataProcessor.hpp"
#include "spdlog/spdlog.h"

void DataProcessor::log_data() {
  auto data = input_data->pop();
  if (data != NULL) {
    for_each(data->begin(), data->end(),
             [&](Point2f x) { output_file << x.y << " "; });
    output_file << endl;
  }
}

void DataProcessor::setup_log() {

  output_file << "Output Data" << endl;
  for (int i = 0; i <= 50; i++) {
    output_file << i << " ";
  }
  output_file << endl;
}