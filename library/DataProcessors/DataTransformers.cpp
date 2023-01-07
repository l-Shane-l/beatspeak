#include "../../include/DataProcessors/DataTransformers.hpp"

using namespace Eigen;
using namespace std;

// Function to carry out PCA on a vector of vectors of floats
vector<float> DataTransformers::PCA(vector<vector<float>> data) {
  spdlog::info("Performing PCA on data");
  // Convert the vector of vectors into an Eigen matrix
  MatrixXf mat(data.size(), data[0].size());
  for (int i = 0; i < data.size(); i++) {
    for (int j = 0; j < data[i].size(); j++) {
      mat(i, j) = data[i][j];
    }
  }
  auto untouched = mat;

  // Compute the mean of the matrix
  VectorXf mean = mat.colwise().mean();

  // Subtract the mean from each column of the matrix
  mat = mat.rowwise() - mean.transpose();

  // Compute the covariance matrix
  MatrixXf cov = (mat.transpose() * mat) / float(mat.rows() - 1);

  // Compute the eigenvalues and eigenvectors of the covariance matrix
  SelfAdjointEigenSolver<MatrixXf> eigensolver(cov);
  if (eigensolver.info() != Success) {
    spdlog::info("Failed to compute eigenvalues and eigenvectors");
    return {};
  }

  // Print the eigenvalues and eigenvectors
  auto values = eigensolver.eigenvalues();
  auto vectors = eigensolver.eigenvectors();
  auto selectedVector = vectors.col(0);
  VectorXf reduced = untouched * selectedVector;
  return vector<float>(reduced.begin(), reduced.end());
}

void DataTransformers::find_max_distances() {
  max_distances.clear();
  for_each(dataBatch.begin(), dataBatch.end(), [&](vector<float> x) {
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

void DataTransformers::filter_by_mode() {
  // filter out the data that is not the mode
  int removed = 0;
  for (int i = 0; i < dataBatch.size(); i++) {
    if (max_distances[i] > mode) {
      dataBatch.erase(dataBatch.begin() + i);
      removed++;
      max_distances.erase(max_distances.begin() + i);
      i--;
    }
  }
  spdlog::info("Removed " + to_string(removed) + " points " +
               to_string(dataBatch.size()) + " remaining");
}

void DataTransformers::find_mode() {
  // find the mode of the max distances
  spdlog::info("Finding mode");
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

void DataTransformers::apply_cublic_spline_to_matrix() {
  // apply the cubic spline to the data
  for_each(dataBatch.begin(), dataBatch.end(),
           [&](vector<float> &x) { x = apply_cubic_spline(x); });
  spdlog::info("Applied cubic spline " + to_string(dataBatch[0].size()));
}
// Function to apply a cubic spline to a vector of values
std::vector<float>
DataTransformers::apply_cubic_spline(const std::vector<float> &y) {
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

void DataTransformers::apply_butterworth_filter() {
  // apply the butterworth filter to the data
  for_each(dataBatch.begin(), dataBatch.end(), [&](vector<float> &x) {
    x = butterworth_filter_5th_order(x, 0.75, 5);
  });
  spdlog::info("Applied butterworth filter " + to_string(dataBatch[0].size()));
}

std::vector<float>
DataTransformers::butterworth_filter_5th_order(std::vector<float> &signal,
                                               float fs, float fc) {
  // Compute the filter coefficients
  float wc = 2 * M_PI * fc / fs;
  float alpha = sin(wc) / (2 * 5); // 5 is the filter order

  // Create the filter transfer function
  std::vector<float> b(3), a(3);
  b[0] = 1 + alpha;
  b[1] = -2 * cos(wc);
  b[2] = 1 - alpha;
  a[0] = 1;
  a[1] = -2 * cos(wc);
  a[2] = 1;

  // Apply the filter to the signal
  std::vector<float> filtered(signal.size());
  filtered[0] = (b[0] * signal[0] + b[1] * signal[1] + b[2] * signal[2]) / a[0];
  for (int i = 1; i < signal.size() - 1; i++) {
    filtered[i] =
        (b[0] * signal[i] + b[1] * signal[i - 1] + b[2] * signal[i - 2] -
         a[1] * filtered[i - 1] - a[2] * filtered[i - 2]) /
        a[0];
  }
  filtered[signal.size() - 1] =
      (b[0] * signal[signal.size() - 1] + b[1] * signal[signal.size() - 2] +
       b[2] * signal[signal.size() - 3] - a[1] * filtered[signal.size() - 2] -
       a[2] * filtered[signal.size() - 3]) /
      a[0];

  // Replace the original signal with the filtered signal
  signal = filtered;
  return signal;
}

int DataTransformers::countPeaks(const std::vector<float> stream) {
  // Initialize the peak count
  spdlog::info("Counting peaks");
  int peaks = 0;

  // Iterate through the stream and count the number of zero crossings
  for (int i = 1; i < stream.size() - 1; i++) {
    if ((stream[i] > stream[i - 1] && stream[i] > stream[i + 1]) ||
        (stream[i] < stream[i - 1] && stream[i] < stream[i + 1])) {
      peaks++;
    }
  }
  spdlog::info("Peaks: " + to_string(peaks));
  return peaks / 2;
}