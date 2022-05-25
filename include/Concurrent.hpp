#ifndef concurrent_hpp
#define concurrent_hpp
#include <stdio.h>
#include <iostream>
#include <condition_variable>
#include <thread>
#include <future>
#include <cmath>
#include <vector>
#include <mutex>
#include <string>
#include <algorithm>
#include "WebCam.hpp"
#include "HeadTracker.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include "RawDataList.hpp"



const int downsample{1};
const double TIME_BASE{0.001};

using namespace std;
int run(int argc, char *argv[]);
void fileTest (promise<bool> &&prms);
void getInput(shared_ptr<WebCam> &cam);
void trackHead(shared_ptr<WebCam> &cam, unique_ptr<HeadTracker> &tracker, shared_ptr<Raw_Data_List> &rawData);
void sendOutput(shared_ptr<WebCam> &cam);
void processData(shared_ptr<Raw_Data_List> &rawData);
void batchData(shared_ptr<Raw_Data_List> &rawData);
void setUp();


#endif /* Heartbeat_hpp */
