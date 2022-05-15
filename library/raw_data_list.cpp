#include "raw_data_list.hpp"
#include <chrono>
#include <thread>

void Raw_Data_List::Add_Data(vector<double> newData)
{
        // better off batching and keeping this as simple as possible
        vector<vector<double>> tmpCurrent;
        vector<double> tmpCurrentSeg;
        if (inputData.size() < newData.size())
        {
                vector<vector<double>> tmp;
                vector<int> modalTmp;
                cout << inputData.size() << newData.size() << " sizes " << endl;
                for (int i = 0; i < newData.size(); i++)
                {

                        tmp.push_back({newData[i]});
                        modalTmp.push_back(0);
                }
                inputData = tmp;
                max_dist = modalTmp;
        }
        else
        {
                auto start = inputData.begin();
                for (int i = 0; i < newData.size(); i++)
                {
                        double newValue = newData[i];
                        pre_spline << newValue << ", ";
                        int dist = round(abs(start->back() - newValue)); // what about initial conditions
                        max_dist[i] = max(max_dist[i], dist);
                        start->push_back(newData[i]); // fix this -- few months later and havent got a clue why i said this, what idiot wrote this :-);update  start is the var for the iterator
                        start++;
                }

                //         cout<<start->size()<<endl;
                // for(double elm : max_dist){
                //        cout << elm << " ";
                // }
                // cout << endl;
                pre_spline << endl;
        }
}

void Raw_Data_List::Butterworth_Filter(){}

// void Raw_Data_List::PCA(){}

void Raw_Data_List::Modal_Dist_Filter() // tested for time taken, fine up to 5 seconds
{
        for (int i = 0; i < max_dist.size(); i++)
        {
                vector<int> tmp = max_dist;
                sort(tmp.begin(), tmp.end());
                int count;
                int current_max = 0;
                int mode = 0;
                for (int j = 1; j < tmp.size(); j++)
                {
                        if (tmp[j - 1] == tmp[j])
                        {
                                count++;
                        }
                        else
                        {
                                if (count > current_max)
                                {
                                        current_max = count;
                                        mode = tmp[j - 1];
                                }
                                count = 0;
                        }
                }

                modal_dist = mode;
                int deleted = 0;
                if(mode > 1 && inputData.size() >= max_dist.size()){
                for (int k = 0; k < max_dist.size(); k++)
                {
                        cout << k << endl;

                        // need to iterate through both max_dist and also inputData and remove any array greater than the mode.
                        if (max_dist[k] > mode) 
                        {
                                int toDelete = k - deleted;
                                inputData.erase(inputData.begin() + toDelete); // this is causing a crash
                                cout <<"toDelete " << toDelete << " inputData " << inputData.size() << " maxDist " << max_dist.size() << endl;
                                deleted++;

                                cout << " deleted " << endl;
                        }
                }
                }
        }
}

void Raw_Data_List::Add_Spline()
{

        tk::spline spl;
        for (int i = 0; i < inputData.size(); i++)
        {
                vector<double> v(inputData[i].size());
                iota(begin(v), end(v), 1);
                spl.set_points(v, inputData[i]);
                double denom = 1250 / (inputData[i].size());
                vector<double> tmpVec;
                for (int j = 0; j < 1250; j++)
                {
                        double val = j / denom;
                        double val2 = spl(val);
                        tmpVec.push_back(val2);
                        post_spline << val2 << ", ";
                }
                post_spline << endl;
                postSpline.push_back(tmpVec);
                
        }
}
