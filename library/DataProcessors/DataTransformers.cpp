#include "../../include/DataProcessors/DataTransformers.hpp"
#include "eigen3/Eigen/Dense"
#include "eigen3/Eigen/Eigenvalues"
#include "spdlog/fmt/ostr.h"
#include "spdlog/spdlog.h"
#include <iostream>
#include <string>
#include <vector>

using namespace Eigen;
using namespace std;

// Function to carry out PCA on a vector of vectors of floats
vector<float> DataTransformers::PCA(vector<vector<float>> data) {
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
    cerr << "Failed to compute eigenvalues and eigenvectors" << endl;
    return {};
  }

  // Print the eigenvalues and eigenvectors
  auto values = eigensolver.eigenvalues();
  auto vectors = eigensolver.eigenvectors();
  auto selectedVector = vectors.col(0);
  VectorXf reduced = untouched * selectedVector;
  return vector<float>(reduced.begin(), reduced.end());
}