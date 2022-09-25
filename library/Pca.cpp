// g++ main.cpp -o app
//  https://medium.com/analytics-vidhya/understanding-principle-component-analysis-pca-step-by-step-e7a4bb4031d9
#include "../Eigen/Dense"
#include "../Eigen/Eigenvalues"
#include <iostream>
#include <math.h>
#include <vector>
using namespace std;

void print(std::vector<double> const &a) {
  for (int i = 0; i < a.size(); i++) {
    std::cout << a.at(i) << ' ';
  }
  cout << endl;
}

double Average(vector<double> v) {
  double total;
  for (int i = 0; i < v.size(); i++) {
    total += v[i];
  }
  return (total / v.size());
}

double Variance(std::vector<double> samples) {
  double size = samples.size();

  double variance = 0;
  double t = samples[0];
  for (int i = 1; i < size; i++) {
    t += samples[i];
    double diff = ((i + 1) * samples[i]) - t;
    variance += (diff * diff) / ((i + 1.0) * i);
  }

  return variance / (size - 1);
}

double StandardDeviation(std::vector<double> samples) {
  return sqrt(Variance(samples));
}

void Standardization(std::vector<std::vector<double>> &v) {
  double Avg, STD;
  for (int i = 0; i < v.size(); i++) {
    Avg = Average(v[i]);
    STD = StandardDeviation(v[i]);
    for (int j = 0; j < v[i].size(); j++) {
      v[i][j] = (v[i][j] - Avg) / STD;
    }
  }
}

double cov(std::vector<double> vA, std::vector<double> vB) {
  double total;
  for (int i = 0; i < vA.size(); i++) {
    total += vA[i] * vB[i];
  }
  return (total / vA.size());
}

std::vector<std::vector<double>>
Covarience(std::vector<std::vector<double>> features) {
  std::vector<std::vector<double>> covMatrix;
  std::vector<double> temp;
  for (int i = 0; i < features.size(); i++) {
    for (int j = 0; j < features.size(); j++) {
      temp.push_back(cov(features[i], features[j]));
    }
    covMatrix.push_back(temp);
    temp.clear();
  }

  return covMatrix;
}

int pca() {
  int EigenVectorsToUse = 2;
  std::vector<std::vector<double>> features{
      {1, 5, 1, 5, 8}, {2, 5, 4, 3, 1}, {3, 6, 2, 2, 2}, {4, 7, 3, 1, 2}};
  Standardization(features);
  std::vector<std::vector<double>> covMat = Covarience(features);

  Eigen::MatrixXd m(covMat[0].size(),
                    covMat.size()); // the nexted element ref is redundant
                                    // because this should always be semetrical
  for (int i = 0; i < covMat.size(); i++) {
    Eigen::Vector4d v(covMat[i].data());
    m.col(i) << v;
  }

  Eigen::EigenSolver<Eigen::MatrixXd> eigensolver;
  eigensolver.compute(m);
  Eigen::VectorXd eigen_values = eigensolver.eigenvalues().real();
  Eigen::MatrixXd eigen_vectors = eigensolver.eigenvectors().real();

  Eigen::MatrixXd toChange(features[0].size(), features.size());
  for (int i = 0; i < features.size(); i++) {
    Eigen::VectorXd o1(features[i].size());
    for (int j = 0; j < features[i].size(); j++) {
      o1[j] = features[i][j];
      toChange.col(i) << o1;
    }
  }
  ///////////////to change
  Eigen::MatrixXd trans(features.size(), EigenVectorsToUse);
  for (int i = 0; i < EigenVectorsToUse; i++) {
    trans.col(i) << eigen_vectors.col(i);
  }
  ////////////////////
  cout << "start" << endl << trans << endl << "end" << endl;

  cout << "Final" << endl << toChange * trans << endl << "Result" << endl;

  return 0;
}
