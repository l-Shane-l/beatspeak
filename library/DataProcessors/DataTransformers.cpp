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

void DataTransformers::filter_by_mode(std::vector<std::vector<float>> &dataBatch) {
    // Find the max distances for each point
    std::vector<float> max_distances;
    for (auto const& point : dataBatch) {
        std::vector<float> differences;
        std::adjacent_difference(point.begin(), point.end(), std::back_inserter(differences));
        max_distances.push_back(*std::max_element(differences.begin(), differences.end()));
    }

    // Find the mode of the max distances
    std::nth_element(max_distances.begin(), max_distances.begin() + max_distances.size() / 2, max_distances.end());
    float mode = max_distances[max_distances.size() / 2];

    // Filter out the data that is not the mode
    int removed = 0;
    for (int i = 0; i < dataBatch.size(); i++) {
        if (max_distances[i] > mode) {
            dataBatch.erase(dataBatch.begin() + i);
            removed++;
            max_distances.erase(max_distances.begin() + i);
            i--;
        }
    }
    std::cout << "Removed " << removed << " points " << dataBatch.size() << " remaining" << std::endl;
}
void DataTransformers::apply_cubic_spline_to_matrix() {
  // apply the cubic spline to the data
  for (auto& x : dataBatch) {
    apply_cubic_spline(x);
  }
  spdlog::info("Applied cubic spline " + to_string(dataBatch[0].size()));
}

// Function to apply a cubic spline to a vector of values
void DataTransformers::apply_cubic_spline(std::vector<float> &y) {
  int current_sampling_rate = dataBatch.size() / time_interval;
  int output_size = 250 * y.size() / current_sampling_rate;
  y.resize(output_size);
  std::vector<double> x(output_size);

  // Set up the spline object
  tk::spline s;
  std::iota(x.begin(), x.end(), 0);
  s.set_points(x, std::vector<double>(y.begin(), y.end()));

  // Evaluate the spline at the desired output points
  for (int i = 0; i < output_size; ++i) {
    y[i] = s((double)i * current_sampling_rate / 250);
  }
}

void DataTransformers::center_and_scale(vector<vector<float>> &data) {
  for (auto &row : data) {
    float mean = accumulate(row.begin(), row.end(), 0.0) / row.size();
    for (auto &val : row) {
        val -= mean;
    }
    float std_dev = 0;
    for (auto &val : row) {
        std_dev += val * val;
    }
    std_dev = sqrt(std_dev / row.size());
    for (auto &val : row) {
        val /= std_dev;
    }
  }
}

std::vector<float>& DataTransformers::apply_kalman_filter(std::vector<float> &signal, float process_noise, float measurement_noise)
{
    if (signal.empty())
        throw std::invalid_argument("The input signal is empty");
    std::vector<float> filtered(signal.size());
    // Initialize Kalman filter variables
    float x_est_last = signal[0]; // The last estimated value
    float P_last = 1; // The last error estimate
    for (int i = 0; i < signal.size(); i++) {
        // Time update
        float x_temp_est = x_est_last;
        float P_temp = P_last + process_noise;
        // Measurement update
        float K = P_temp / (P_temp + measurement_noise);
        float x_est = x_temp_est + K * (signal[i] - x_temp_est);
        float P = (1 - K) * P_temp;
        filtered[i] = x_est;
        // Update for next iteration
        x_est_last = x_est;
        P_last = P;
    }
    signal = filtered;
    return signal;
}
void DataTransformers::apply_band_pass_filter(std::vector<std::vector<float>>& dataBatch) {

    for (auto& signal : dataBatch) {
        signal = butterworth_filter_5th_order(signal, 0.75, 5.0);
    }

}

void DataTransformers::apply_kalman_filter(std::vector<std::vector<float>>& dataBatch) {

    for (auto& signal : dataBatch) {
        signal = apply_kalman_filter(signal, 0.1, 0.1);
    }

}


std::vector<float> DataTransformers::band_pass_filter(std::vector<float> signal, float low_cut, float high_cut)
{
    // Coefficients for band pass filter
    float a0 = 1;
    float a1 = -4.49755859375;
    float a2 = 8.994140625;
    float a3 = -9.98779296875;
    float a4 = 5.498046875;
    float a5 = -1.24755859375;
    float b0 = 0.0033111572265625;
    float b1 = 0.016655712890625;
    float b2 = 0.032958984375;
    float b3 = 0.032958984375;
    float b4 = 0.016655712890625;
    float b5 = 0.0033111572265625;

    // Initialize filtered signal vector
    std::vector<float> filtered_signal(signal.size());

    // Apply band pass filter to signal
    for (int i = 5; i < signal.size(); i++) {
        filtered_signal[i] = b0*signal[i] + b1*signal[i-1] + b2*signal[i-2] + b3*signal[i-3] + b4*signal[i-4] + b5*signal[i-5] - a1*filtered_signal[i-1] - a2*filtered_signal[i-2] - a3*filtered_signal[i-3] - a4*filtered_signal[i-4] - a5*filtered_signal[i-5];
    }

    return filtered_signal;
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

// std::vector<std::complex<float>> dft(std::vector<float> signal) {
//     // Allocate memory for the input and output arrays
//     fftwf_complex *in, *out;
//     in = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * signal.size());
//     out = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * signal.size());
//     // Copy the input signal to the input array
//     for (int i = 0; i < signal.size(); i++) {
//         in[i][0] = signal[i];
//         in[i][1] = 0;
//     }
//     // Create a plan for the forward DFT
//     fftwf_plan plan = fftwf_plan_dft_1d(signal.size(), in, out, FFTW_FORWARD, FFTW_ESTIMATE);
//     // Execute the plan
//     fftwf_execute(plan);
//     // Copy the output to a vector of complex numbers
//     std::vector<std::complex<float>> dft_result(signal.size());
//     for (int i = 0; i < signal.size(); i++) {
//         dft_result[i] = std::complex<float>(out[i][0], out[i][1]);
//     }
//     // Clean up
//     fftwf_destroy_plan(plan);
//     fftwf_free(in);
//     fftwf_free(out);
//     return dft_result;
// }

// void write_frequency_content(std::vector<std::complex<float>> dft_result, std::string filename) {
//     std::vector<float> magnitude(dft_result.size());
//     for (int i = 0; i < dft_result.size(); i++) {
//         magnitude[i] = std::abs(dft_result[i]);
//     }
//     std::ofstream outfile(filename);
//     for (int i = 0; i < magnitude.size(); i++) {
//         outfile << i << " " << magnitude[i] << std::endl;
//     }
//     outfile.close();
// }



// std::vector<float> band_pass_filter(std::vector<float> signal, float low_cut, float high_cut, float fs) {
//     // Perform DFT
//     std::vector<std::complex<float>> dft_result = dft(signal, fs);
//     // Apply the band-pass filter
//     for (int i = 0; i < dft_result.size(); i++) {
//         if (i < low_cut || i > high_cut) {
//             dft_result[i] = std::complex<float>(0, 0);
//         }
//     }
//     // Perform Inverse DFT
//     std::vector<std::complex<float>> filtered_dft = idft(dft_result, fs);
//     // Convert the result to real numbers
//     std::vector<float> filtered_signal(filtered_dft.size());
//     for (int i = 0; i < filtered_dft.size(); i++) {
//         filtered_signal[i] = filtered_dft[i].real();
//     }
//     return filtered_signal;
// }