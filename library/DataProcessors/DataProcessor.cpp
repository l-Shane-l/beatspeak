#include "../../include/DataProcessors/DataProcessor.hpp"
#include "spdlog/spdlog.h"

void DataProcessor::log_data() {
  current = chrono::system_clock::now();
  if (chrono::duration_cast<chrono::seconds>(current - start).count() >= 5) {
    spdlog::info("Logging Data");
    start = chrono::system_clock::now();
  } else {
    auto dataPoint = input_data->pop();
    if (dataPoint != NULL) {
      for_each(dataPoint->begin(), dataPoint->end(), [&](Point2f x) {
        output_file << x.y << " ";
        data.push_back(x.y);
      });
      output_file << endl;
      data_transformers.Modal_Dist_Filter(data);
    }
  }
}

void DataProcessor::setup_log() {

  output_file << "Output Data" << endl;
  for (int i = 0; i <= 50; i++) {
    output_file << i << " ";
  }
  output_file << endl;
}
