#include "../../include/DataProcessors/DataProcessor.hpp"
#include "spdlog/spdlog.h"

void DataProcessor::log_data() {
  current = chrono::system_clock::now();
  if (chrono::duration_cast<chrono::seconds>(current - start).count() >= 5) {
    spdlog::info("Logging Data " + to_string(data.size()) + " " +
                 to_string(data[0].size()));
    find_max_distances();
    spdlog::info(max_distances[0]);
    find_mode();
    filter_by_mode();
    if (data.size() == 0) {
      spdlog::info("No data to log");
      return;
    }
    apply_cublic_spline_to_matrix();
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

void DataProcessor::find_max_distances() {
  max_distances.clear();
  for_each(data.begin(), data.end(), [&](vector<float> x) {
    float max = 0;
    for (int i = 0; i < x.size() - 1; i++) {
      float distance = abs(x[i] - x[i + 1]);
      if (distance > max) {
        max = distance;
      }
    };
    max_distances.push_back(max);
  });
}

void DataProcessor::filter_by_mode() {
  // filter out the data that is not the mode
  int removed = 0;
  for (int i = 0; i < data.size(); i++) {
    if (max_distances[i] > mode) {
      data.erase(data.begin() + i);
      removed++;
      max_distances.erase(max_distances.begin() + i);
      i--;
    }
  }
  spdlog::info("Removed " + to_string(removed) + " points " +
               to_string(data.size()) + " remaining");
}

void DataProcessor::find_mode() {
  // find the mode of the max distances
  map<int, int> count;
  int max = 0;

  for_each(max_distances.begin(), max_distances.end(), [&](float y) {
    int x = (int)(round(y));
    if (count.find(x) == count.end()) {
      count[x] = 1;
    } else {
      count[x] += 1;
      if (count[x] > max) {
        max = count[x];
        mode = x;
      };
    }
  });
  spdlog::info("Mode: freq " + to_string(max) + "  value " + to_string(mode));
}

void DataProcessor::apply_cublic_spline_to_matrix() {
  // apply the cubic spline to the data
  for_each(data.begin(), data.end(),
           [&](vector<float> &x) { x = apply_cubic_spline(x); });
  spdlog::info("Applied cubic spline " + to_string(data[0].size()));
}
vector<float> DataProcessor::apply_cubic_spline(const vector<float> &x) {
  // Initialize the vector for the cubic spline
  vector<float> y(250);

  // Loop through the elements in the input vector
  for (int i = 0; i < x.size(); i++) {
    // Apply the cubic spline formula to each element
    y[i] = x[i] * x[i] * x[i];
  }

  // Return the vector with the cubic spline applied
  return y;
}