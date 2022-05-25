#ifndef butterworth_hpp
#define butterworth_hpp

#include<iostream>
#include<opencv/cv.hpp>
#include<opencv2/opencv.hpp>
#include<fstream>
#include<math.h>

using namespace std;
using namespace cv;

#define PI 3.14159

vector<double> ComputeDenCoeffs(int FilterOrder, double Lcutoff, double Ucutoff);

vector<double> TrinomialMultiply(int FilterOrder, vector<double> b, vector<double> c);

vector<double> ComputeNumCoeffs(int FilterOrder, double Lcutoff, double Ucutoff, vector<double> DenC);

vector<double> ComputeLP(int FilterOrder);

vector<double> ComputeHP(int FilterOrder);

//vector<double> filter(int ord, vector<double> a, vector<double> b, int np, vector<double> x);

vector<double> filter(vector<double>x, vector<double> coeff_b, vector<double> coeff_a);


#endif 