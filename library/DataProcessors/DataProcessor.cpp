#include "../../include/DataProcessors/DataProcessor.hpp"
#include "spdlog/fmt/ostr.h"
#include "spdlog/spdlog.h"

void DataProcessor::log_data() {
  current = chrono::system_clock::now();
  if (chrono::duration_cast<chrono::seconds>(current - start).count() >=
      time_interval) {
    // spdlog::info("Logging Data " + to_string(data.size()) + " " +
    //              to_string(data[0].size()));
    find_max_distances();
    // spdlog::info(max_distances[0]);
    find_mode();
    filter_by_mode();

    if (data.size() == 0) {
      spdlog::info("No data to log");
      return;
    }
    apply_cublic_spline_to_matrix();
    apply_butterworth_filter();
    spdlog::info(" data shape " + to_string(data.size()) + "S " +
                 to_string(data[0].size()));
    // principal_components = data_transformer.PCA(data);
    // spdlog::info("Principal Components " +
    //              to_string(principal_components.size()));
    // int i = 0;

    // result_file.open("result.dat");
    // float placeholder = 0;
    // result_file << placeholder << endl;
    // for_each(principal_components.begin(), principal_components.end(),
    //          [&](float x) { result_file << x << endl; });
    // result_file.close();
    data.clear();
    start = chrono::system_clock::now();

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
// Function to apply a cubic spline to a vector of values
std::vector<float>
DataProcessor::apply_cubic_spline(const std::vector<float> &y) {
  int n = y.size();
  int output_size = 250 * time_interval;
  std::vector<float> yi(output_size); // Output spline values
  std::vector<double> x(n);
  // Set up the spline object
  tk::spline s;
  for (int i = 0; i < n; ++i) {
    x[i] = i;
  }
  s.set_points(x, std::vector<double>(y.begin(), y.end()));

  // Evaluate the spline at the desired output points
  for (int i = 0; i < output_size; ++i) {
    yi[i] = s(i);
  }

  return yi;
}

void DataProcessor::apply_butterworth_filter() {
  // apply the butterworth filter to the data
  for_each(data.begin(), data.end(), [&](vector<float> &x) {
    x = butterworth_filter_5th_order(x, 0.75, 5);
  });
  spdlog::info("Applied butterworth filter " + to_string(data[0].size()));
}

std::vector<float>
DataProcessor::butterworth_filter_5th_order(std::vector<float> &signal,
                                            float f1, float f2) {
  int N = 5; // filter order
  float Wc1 = 2.0 * M_PI * f1;
  float Wc2 = 2.0 * M_PI * f2;
  float b[N + 1], a[N + 1];

  // calculate filter coefficients
  float Wc0 = sqrt(Wc1 * Wc2);
  float alpha = (Wc2 - Wc1) / (2.0 * tan(M_PI * (Wc2 - Wc1) / (2.0 * Wc0)));
  float k = Wc0 * tan(M_PI * (Wc2 - Wc1) / (2.0 * Wc0));
  float K = k * k + alpha * alpha + 2.0 * alpha * k;
  b[0] = k * k / K;
  b[1] = 2.0 * b[0];
  b[2] = b[0];
  b[3] = 2.0 * (k * k - alpha * alpha) / K;
  b[4] = (k * k - 2.0 * alpha * k + alpha * alpha) / K;
  b[5] = 0.0;
  a[0] = 1.0;
  a[1] = 2.0 * (alpha * alpha - k * k) / K;
  a[2] = (k * k - 2.0 * alpha * k + alpha * alpha) / K;
  a[3] = 2.0 * (k * k - alpha * alpha) / K;
  a[4] = (k * k + alpha * alpha - 2.0 * alpha * k) / K;
  a[5] = (k * k + alpha * alpha + 2.0 * alpha * k) / K;

  // apply filter
  int size = signal.size();
  std::vector<float> y(size);
  for (int n = 0; n < size; n++) {
    float out = 0.0;
    for (int i = 0; i < N + 1; i++) {
      int j = n - i;
      float in = (j < 0) ? 0.0 : signal[j];
      out += b[i] * in - a[i] * y[j];
    }
    y[n] = out;
  }

  return y;
}