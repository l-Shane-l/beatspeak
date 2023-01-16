#include "../../include/DataProcessors/DataTransformers.hpp"

#include <fftw3.h>
using namespace std;

int DataTransformers::getMostPeriodicSignal(MatrixXf& mat, BDCSVD<MatrixXf>& svd) {
    int max_index = 0;
    float max_power = 0;
    for (int i = 0; i < svd.singularValues().size(); i++) {
        // Transform the data onto the new 1-dimensional space
        MatrixXf transformed = mat * svd.matrixV().col(i);

        // Allocate memory for the input and output arrays
        int rows = transformed.rows();
        int cols = transformed.cols();
        double* input = fftw_alloc_real(rows * cols);
        fftw_complex* output = fftw_alloc_complex(rows * (cols/2+1));

        // Copy the data from the Eigen matrix to the input array
        for (int j = 0; j < rows; j++) {
            for (int k = 0; k < cols; k++) {
                input[j*cols + k] = transformed(j, k);
            }
        }

        // Create the plan and execute the FFT
        fftw_plan plan = fftw_plan_dft_r2c_2d(rows, cols, input, output, FFTW_ESTIMATE);
        fftw_execute(plan);

        // Calculate the power spectral density
        VectorXf psd(rows * (cols/2+1));
        for (int j = 0; j < rows; j++) {
            for (int k = 0; k < cols/2+1; k++) {
                double re = output[j*(cols/2+1) + k][0];
                double im = output[j*(cols/2+1) + k][1];
                psd(j*(cols/2+1) + k) = re*re + im*im;
            }
        }
        psd /= (rows*cols);

        // Find the index of the first harmonic
        int first_harmonic = psd.size()/2;
        // Calculate the total power
        float total_power = psd.sum();
        // Calculate the percentage of power in the first harmonic
        float power_in_first_harmonic = psd[first_harmonic] / total_power;
        // Keep track of the signal with the highest percentage of power in the first harmonic
        if (power_in_first_harmonic > max_power) {
            max_index = i;
            max_power = power_in_first_harmonic;
        }

        // Cleanup
        fftw_destroy_plan(plan);
        fftw_free(input);
        fftw_free(output);
    }
    return max_index;
}

// Function to carry out PCA on a vector of vectors of floats
vector<float> DataTransformers::PCA(vector<vector<float>> data) {
  // Convert the vector of vectors into an Eigen matrix
  MatrixXf mat(data.size(), data[0].size());
  for (int i = 0; i < data.size(); i++) {
    for (int j = 0; j < data[i].size(); j++) {
      mat(i, j) = data[i][j];
    }
  }

  // Standardize the data
  VectorXf mean = mat.colwise().mean();
  mat = mat.rowwise() - mean.transpose();
  VectorXf stddev = mat.colwise().norm() / sqrt(mat.rows() - 1);
  mat = mat.array().rowwise() / stddev.transpose().array();

  // Perform SVD
  BDCSVD<MatrixXf> svd(mat, ComputeThinU | ComputeThinV);

  // Call the function that returns the index of the most periodic signal
  int max_index = getMostPeriodicSignal(mat, svd);

  // Get the eigenvector corresponding to the most periodic signal
  MatrixXf eigenvector = svd.matrixV().col(max_index);

  // Transform the data onto the new 1-dimensional space
  MatrixXf transformed = mat * eigenvector;

  // Convert the transformed data back to a vector
  return vector<float>(transformed.data(),
                       transformed.data() + transformed.size());
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
    int peaks = 0;

    // Check for peak at the first data point
    if ((stream[0] > stream[1]) || (stream[0] < stream[1])) {
        peaks++;
    }

    // Iterate through the stream and count the number of zero crossings
    for (int i = 1; i < stream.size() - 1; i++) {
        if ((stream[i] > stream[i - 1] && stream[i] > stream[i + 1]) ||
            (stream[i] < stream[i - 1] && stream[i] < stream[i + 1])) {
            peaks++;
        }
    }
    // Check for peak at the last data point
    if ((stream[stream.size()-1] > stream[stream.size()-2]) || (stream[stream.size()-1] < stream[stream.size()-2])) {
        peaks++;
    }
    return peaks;
}