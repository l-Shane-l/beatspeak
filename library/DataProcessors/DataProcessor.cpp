#include "../../include/DataProcessors/DataProcessor.hpp"
#include "spdlog/spdlog.h"

void DataProcessor::log_data() {
  auto data = input_data->pop();
  if (data != NULL) {
    output_file << *data << endl;
  }
}

void DataProcessor::setup_log() { output_file << "Output Data" << endl; }