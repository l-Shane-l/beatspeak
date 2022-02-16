#ifndef FacialPointsGenerator_hpp
#define FacialPointsGenerator_hpp

#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/video/tracking.hpp"
#include "TrackerData.hpp"
#include <iostream>
#include "raw_data_list.hpp"
#include <memory>

using namespace std;
using namespace cv;

struct FacialPointsGenerator
{
    FacialPointsGenerator(shared_ptr<TrackerData> data)
    {
        Data = data;
    }

public:
    void trackPoints(vector<Point2f> &newPoints, shared_ptr<Raw_Data_List> &rawdata, bool update);
    void setNumberOfPoints(int num)
    {
        numOfPoints = num;
    }

private:
    void generateMask();
    void generateTrackingPoints();
    void updateTrackedPoints(shared_ptr<Raw_Data_List> &rawdata);
    void drawPoints();
    Mat maskedFrame, errors;
    vector<Point2f> prevPoints, currentPoints, generatedPoints;
    vector<Point2f> none{'0'};
    vector<Point> faceRegion, eyeRegion;
    vector<uchar> trackingStatus;
    uchar notFound{'0'};
    int numOfPoints;
    shared_ptr<TrackerData> Data;
};

#endif
