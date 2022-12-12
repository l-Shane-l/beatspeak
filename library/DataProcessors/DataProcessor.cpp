#include "../../include/DataProcessors/DataProcessor.hpp"
#include "spdlog/spdlog.h"

void DataProcessor::log_data() {
  current = chrono::system_clock::now();
  if (chrono::duration_cast<chrono::seconds>(current - start).count() >= 5) {
    spdlog::info("Logging Data " + to_string(data.size()) + " " +
                 to_string(data[0].size()));
    start = chrono::system_clock::now();
    data.clear();
  } else {
    auto dataPoint = input_data->pop();
    if (dataPoint != NULL) {
      int i = 0;
      for_each(dataPoint->begin(), dataPoint->end(), [&](Point2f x) {
        output_file << x.y << " ";
        if (data.size() <= i) {
          data.push_back(vector<float>());
        }
        data[i].push_back(x.y);
        i++;
      });

      output_file << endl;
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
