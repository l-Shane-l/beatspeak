#ifndef TrackerData_hpp
#define TrackerData_hpp

#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

struct TrackerData {
        vector<Rect> faces, eyes, mouths;
        Rect face, mouth, eyeOne, eyeTwo;
        Mat frameRGB, frameGray, oldFrame;
        Point faceCenter, mouthCenter;
        vector<Point>  eyeCenter;
        Size radius;
        vector<int> eyeRadius;
        Scalar trackerColor = Scalar(255, 0, 0);
        vector<vector<float>> vertData;
};


#endif
