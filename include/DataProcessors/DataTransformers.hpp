#ifndef DataTransformers_hpp
#define DataTransformers_hpp


#include "Spline.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <fstream>


using namespace std;

struct DataTransformers
{
        double modal_dist;
        vector<int> max_dist;
        vector<int> modalTracker;

       
        void Modal_Dist_Filter();
        void Butterworth_Filter();
        void Add_Spline();
        void PCA();
        

};

#endif
