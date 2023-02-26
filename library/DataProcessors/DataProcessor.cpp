#include "../../include/DataProcessors/DataProcessor.hpp"

#include "spdlog/fmt/ostr.h"
#include "spdlog/spdlog.h"

void DataProcessor::log_data() {
  current = chrono::system_clock::now();
  if (chrono::duration_cast<chrono::seconds>(current - start).count() >=
      time_interval) {
    start = chrono::system_clock::now();
    process_data();

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

void DataProcessor::process_data() {
  DataTransformers data_transformers(time_interval, data);
  data_transformers.apply_cubic_spline_to_matrix();
  // data_transformers.center_and_scale(data_transformers.dataBatch);
  data_transformers.filter_by_mode(data_transformers.dataBatch);


  if (data_transformers.dataBatch.size() == 0) {
    spdlog::info("No data to log");
    return;
  }
  

  data_transformers.apply_kalman_filter(data_transformers.dataBatch);
  data_transformers.apply_band_pass_filter(data_transformers.dataBatch);
  for (auto &row : data_transformers.dataBatch) {
        for(auto &val : row) {
            post_filter << val << " ";
        }
        post_filter << endl;
    }
  spdlog::info("applying pca");
  auto component = data_transformers.PCA(data_transformers.dataBatch);
  spdlog::info("pca finished");
  int mutiple = 60 / time_interval;

  int heartrate = data_transformers.countPeaks(component) * mutiple;

  spdlog::info("Heartrates is " + to_string(heartrate) + " bpm");
  heart_rate_ptr_->store(heartrate);
  result_file.open("result.dat");
  float placeholder = 0;
  result_file << placeholder << endl;
  for_each(component.begin(), component.end(),
           [&](float x) { result_file << x << endl; });
  result_file.close();
  start = chrono::system_clock::now();
}
