#ifndef concurrent_hpp
#define concurrent_hpp
#include "./DataGenerators/HeadTracker.hpp"
#include "./IO/WebCam.hpp"
#include "opencv2/opencv.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <fstream>
#include <future>
#include <iostream>
#include <mutex>
#include <stdio.h>
#include <string>
#include <thread>
#include <vector>

const int downsample{1};
const double TIME_BASE{0.001};

using namespace std;

int run(int argc, char *argv[]);
void fileTest(promise<bool> &&prms);
void getInput(shared_ptr<WebCam> &cam);
void trackHead(shared_ptr<WebCam> &cam, unique_ptr<HeadTracker> &tracker);
void sendOutput(shared_ptr<WebCam> &cam);

void setUp();

#endif
