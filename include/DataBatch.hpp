#ifndef DataBatch_hpp
#define DataBatch_hpp

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

struct DataBatch
{

        vector<vector<double>> Data;


        DataBatch()
        {
                data_out.open("../test_data/pre_spline.dat");

        }

        ~DataBatch()
        {
                data_out.close();

        }
        void Add_Data(vector<double> newData);

private:
        ofstream data_out;
};

#endif
