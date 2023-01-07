#include "../../include/DataProcessors/DataProcessor.hpp"
#include "../include/Concurrent.hpp"
#include "spdlog/fmt/ostr.h"
#include "spdlog/spdlog.h"

void DataProcessor::log_data() {
  current = chrono::system_clock::now();
  if (chrono::duration_cast<chrono::seconds>(current - start).count() >=
      time_interval) {
    start = chrono::system_clock::now();
    thread([&]() { process_data(); }).detach();

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
      // sleep
      // this_thread::sleep_for(chrono::seconds(1));
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
  // spdlog::info("Logging Data " + to_string(data.size()) + " " +
  //              to_string(data[0].size()));
  data_transformers.find_max_distances();
  spdlog::info(data_transformers.max_distances[0]);
  data_transformers.find_mode();
  data_transformers.filter_by_mode();

  if (data_transformers.dataBatch.size() == 0) {
    spdlog::info("No data to log");
    return;
  }
  data_transformers.apply_cublic_spline_to_matrix();
  data_transformers.apply_butterworth_filter();
  // post_filter.open("post_filter.dat");
  // for_each(data[0].begin(), data[0].end(),
  //          [&](float y) { post_filter << y << " "; });
  // post_filter << endl;
  // post_filter.close();

  // spdlog::info(" data shape " + to_string(data.size()) + "S " +
  //              to_string(data[0].size()));
  spdlog::info("applying pca");
  data_transformers.principal_components =
      data_transformers.PCA(data_transformers.dataBatch);
  spdlog::info("pca finished");
  int mutiple = 60 / time_interval;

  int heartrate =
      data_transformers.countPeaks(data_transformers.principal_components) *
      mutiple;
  spdlog::info("Heartrate is " + to_string(heartrate));
  result_file.open("result.dat");
  float placeholder = 0;
  result_file << placeholder << endl;
  for_each(data_transformers.principal_components.begin(),
           data_transformers.principal_components.end(),
           [&](float x) { result_file << x << endl; });
  data_transformers.countPeaks(data_transformers.principal_components);
  result_file.close();
  start = chrono::system_clock::now();
}
