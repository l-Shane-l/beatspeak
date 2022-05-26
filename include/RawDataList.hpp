#ifndef raw_data_list_hpp
#define raw_data_list_hpp

#include <list>
#include "Spline.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <fstream>


using namespace std;

struct Raw_Data_List
{
        double modal_dist;
        vector<int> max_dist;
        vector<vector<double>> currentData;
        vector<vector<double>> inputData;
        vector<vector<double>> postSpline;
        vector<int> modalTracker;
        vector<vector<double>> postButterworth;

        Raw_Data_List()
        {
                pre_spline.open("../test_data/pre_spline.dat");
                post_spline.open("../test_data/post_spline.dat");
                post_butterworth.open("../test_data/post_butterworth.dat");
                post_modal_dist.open("../test_data/post_modal_dist");
        }

        ~Raw_Data_List()
        {
                pre_spline.close();
                post_spline.close();
                post_butterworth.close();
                post_modal_dist.close();
        }
        void Add_Data(vector<double> newData);
        void Modal_Dist_Filter();
        void Butterworth_Filter();
        void Add_Spline();
        void PCA();

private:
        ofstream pre_spline, post_spline, post_butterworth, post_modal_dist;
};

#endif
